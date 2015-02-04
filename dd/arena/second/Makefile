obj-m += startstop.o
startstop-objs := start.o stop.o
obj-m += first.o
obj-m += second.o
obj-m += third.o
KERNELDIR = /usr/src/linux-headers-$(shell uname -r)/
all:
	make -C $(KERNELDIR) SUBDIRS=$(PWD) modules

clean:
	make -C $(KERNELDIR) SUBDIRS=$(PWD) clean