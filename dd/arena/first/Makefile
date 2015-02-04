obj-m += driver.o
KERNELDIR = /usr/src/linux-headers-$(shell uname -r)/
all:
	make -C $(KERNELDIR) SUBDIRS=$(PWD) modules

clean:
	make -C $(KERNELDIR) SUBDIRS=$(PWD) clean