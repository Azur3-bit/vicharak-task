// -configurator.c

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DRIVER_NAME "/dev/sunil"
#define SET_SIZE_OF_QUEUE _IOW('a', 'a', int * )

int main(void) {
    int fd = open(DRIVER_NAME, O_RDWR);

    if(fd < 0){
        printf("the fd value is %d \n", fd);
        perror("failed to open the device \n");
        return -1;
    }

    int size = 100;

    if(ioctl(fd, SET_SIZE_OF_QUEUE, &size) < 0){
        perror("failed to set the size of the queue \n");
        close(fd);
        return -1;
    }

    printf("QUEUE size set to %d\n", size);

    close(fd);
    return 0;
}
