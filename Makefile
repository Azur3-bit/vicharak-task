
KDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)


obj-m += dynamic_queue_driver.o
USERAPPS = configurator filler reader


all : module $(USERAPPS)

module : 
	$(MAKE) -C $(KDIR) M=$(PWD) modules

%: %.c
	gcc -o $@ $<


clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f $(USERAPPS)