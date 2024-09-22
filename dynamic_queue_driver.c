// including files

#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/uaccess.h>
#include<linux/slab.h>
#include<linux/mutex.h>
#include<linux/wait.h>




// definations 

#define DEVICE_NAME "sunil"
#define CLASS_NAME "sunil_class"
#define SET_SIZE_OF_QUEUE _IOW('a', 'a', int * )
#define PUSH_DATA _IOW('a', 'b', struct data * )
#define POP_DATA _IOR('a', 'c', struct data * )
#define QUEUE_EMPTY -1



struct data{
	int lenght;
	char *data;
};



struct circular_queue{

	// [head                 tail]  elements 
		// size => ? 
	struct data *buffer;
	int head;
	int tail;
	int size;
	int count;

};

static int majorNumber;
static struct class* charClass = NULL;
static struct device* charDevice = NULL;
static struct circular_queue queue;
static struct mutex queue_mutex;
static wait_queue_head_t wait_queue;

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static long dev_ioctl(struct file *, unsigned int, unsigned long);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);


// file opertions

static struct file_operations fops={
	.open = dev_open,
	.read = dev_read,
	.release = dev_release,
	.unlocked_ioctl = dev_ioctl,
};

// queue 

static int init_queue(int size){
	if(size <= 0){
		printk(KERN_ALERT "Invlaid size - queue \n");
		return -EINVAL;
	}

	mutex_lock(&queue_mutex);

	kfree(queue.buffer);
	queue.size = size;
	queue.head = 0;
	queue.tail = 0;
	queue.count = 0;

	queue.buffer = kmalloc_array(size, sizeof(struct data), GFP_KERNEL);


	if(!queue.buffer){
		mutex_unlock(&queue_mutex);
		return -ENOMEM;
	}
	mutex_unlock(&queue_mutex);
	return 0;
}


// adding data in the queue
static int push_data(struct data *input){
	int err = 0;
	mutex_lock(&queue_mutex);

	if(queue.count == queue.size){
		err = -ENOSPC;
	}
	else {
		queue.buffer[queue.tail] = *input;
		queue.tail = (queue.tail + 1) % queue.size;
		queue.count++;
		wake_up_interruptible(&wait_queue);		
	}
	mutex_unlock(&queue_mutex);
	return err;
}

// removing data from queue

static int pop_data(struct data *output){
	int err = 0;

	mutex_lock(&queue_mutex);
	if(queue.count == 0){
		// queue is empty
		// underflow condition 
		err = -EAGAIN;
	}
	else{
		*output = queue.buffer[queue.head];
		queue.head = (queue.head + 1) % queue.size;
		queue.count--;
	}
	mutex_unlock(&queue_mutex);
	return err;
} 


static int dev_open(struct inode * inodep, struct file * filep){
	return 0;
}

int dev_release(struct inode *inodep, struct file *filep){
	return 0;
}



static long dev_ioctl(struct file *filep, unsigned int cmd, unsigned long arg){
	int err = 0;
	struct data input_data;
	struct data *user_data = (struct data *)arg;


	switch(cmd){
		case SET_SIZE_OF_QUEUE:
			err = init_queue(*(int*) arg);
			break;


		case PUSH_DATA:
			if(copy_from_user(&input_data, user_data, sizeof(struct data))){
				return -EFAULT;
			}
			err = push_data(&input_data);
			break;

		case POP_DATA:
			while((err = pop_data(&input_data)) == -EAGAIN){
				if(wait_event_interruptible(wait_queue, queue.count > 0)){
					return -ERESTARTSYS;
				}
			}

			if(!err && copy_to_user(user_data, &input_data, sizeof(struct data))){
				return -EFAULT;
			}
			break;

		default:
			err = - EINVAL;
			break;
	}

	return err;
}


static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
	return 0;
}



static __init int char_init(void){
	printk(KERN_INFO "starting the character device \n");
	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);

	// corrected > to '<' - silly mistake (note for later)
	if(majorNumber < 0){
		printk(KERN_ALERT "failed to register a major numeber \n");
		return majorNumber;
	}


	// (done) todo :
		// (done) get the major number 

	printk(KERN_INFO "[Major Number =>] Dynamic Queue registered with Major Number %d\n", majorNumber);


	charClass = class_create(CLASS_NAME);
		if(IS_ERR(charClass)){
			unregister_chrdev(majorNumber, DEVICE_NAME);
			printk(KERN_ALERT "failed to register device class - unregistering the char_dev \n");

			return PTR_ERR(charClass);
		}



	// made the major number fail
	charDevice = device_create(charClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);

	if(IS_ERR(charDevice)){
		class_destroy(charClass);

		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "failed to create the char Device\n");
		return PTR_ERR(charDevice);
	}




	mutex_init(&queue_mutex);
	init_waitqueue_head(&wait_queue);


	printk(KERN_INFO "character Device Driver Started Successfully\n");


	return 0;

}

static void __exit char_exit(void){
	mutex_destroy(&queue_mutex);
	kfree(queue.buffer);
	device_destroy(charClass, MKDEV(majorNumber, 0));

	class_unregister(charClass);
	class_destroy(charClass);
	unregister_chrdev(majorNumber, DEVICE_NAME);
	printk(KERN_INFO"character device driver removed safely\n");
} 


module_init(char_init);
module_exit(char_exit);



MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("dynamic circular queue in character device for sunil \n");
MODULE_AUTHOR("Sunil");
