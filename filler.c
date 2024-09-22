 // - filler.c

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>

#define DRIVER_NAME "/dev/sunil"
#define PUSH_DATA _IOW('a', 'b', struct data * )

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
        perror("failed to allocated memory \n");
        close(fd);
        return -1;
    }

    memcpy(d.data, "xyz", d.length);

    if(ioctl(fd, PUSH_DATA, &d) < 0){
        perror("failed to push data \n");
        free(d.data);
        close(fd);
        return -1;
    }

    printf("data pushed %s\n", d.data);
    
    free(d.data);
    close(fd);
    return 0;
}