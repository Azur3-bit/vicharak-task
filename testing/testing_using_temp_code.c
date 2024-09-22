#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>


MODULE_LICENSE("GPL");


static int __init func_init(void){

	printk(KERN_INFO "func start\n");
	return 0;
}


static void __exit func_end(void){

	printk(KERN_INFO "func end\n");
	// return 0;
}

module_init(func_init);
module_exit(func_end);