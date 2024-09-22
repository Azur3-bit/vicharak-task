# Makefile for dynamic_queue_driver kernel module and configurator

KDIR := /lib/modules/$(shell uname -r)/build
MODULE_NAME := dynamic_queue_driver

obj-m := $(MODULE_NAME).o

EXTRA_CFLAGS += -g

all: module configurator

$(MODULE_NAME).ko: $(MODULE_NAME).c
	$(MAKE) -C $(KDIR) M=$(PWD) modules

module:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

configurator: configurator.c
	gcc -o configurator configurator.c -Wall

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f configurator