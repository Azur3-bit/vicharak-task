 // - reader.c

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>


#define DRIVER_NAME "/dev/sunil"
#define POP_DATA _IOR('a', 'c', struct data * )

struct data {
    int length;
    char * data;
};

int main(void) {
    int fd = open(DRIVER_NAME, O_RDWR);

    if(fd < 0){
        perror("failed to open device \n");
        return -1;
    }


    struct data d;
    d.length = 3;

    d.data = malloc(d.length);

    if(!d.data){
        perror("failed to allocate memory \n");
        close(fd);
        return -1;
    }

    if(ioctl(fd, POP_DATA, &d) < 0){
        perror("failed to pop data \n");
        free(d.data);
        close(fd);
        return -1;
    }


    printf("data poped %s\n", d.data);
    close(fd);
    free(d.data);
    return 0;
}