
KDIR := /lib/modules/$(shell uname -r)/build
MODULE_NAME := dynamic_queue_driver
# PWD := $(shell pwd)

obj-m := $(MODULE_NAME).o

EXTRA_CFLAGS += -g

# obj-m += dynamic_queue_driver.o
# USERAPPS = configurator filler reader


all : module $(MODULE_NAME).ko configurator

$(MODULE_NAME).ko : $(MODULE_NAME).c 

$(MAKE) -C $(KDIR) M=$(PWD) 
modules

module : 
$(MAKE) -C $(KDIR) M=$(PWD) modules

configurator: configurator.g gcc -o configurator configurator.c -Wall

clean: $(MAKE) -C $(KDIR) M=$(PWD) clean rm -f configurator