
KDIR := /lib/modules/$(shell uname -r)/build

obj-m += testing_using_temp_code.o

all :
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean