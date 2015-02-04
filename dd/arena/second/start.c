#include <linux/kernel.h>
#include <linux/module.h>

int init_module(void){
	printk(KERN_INFO "The kernel observed start object");
	return 0;
}