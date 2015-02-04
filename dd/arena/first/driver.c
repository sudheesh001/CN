#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init mydriver_init(void){
	printk(KERN_INFO "Driver registered\n");
	return 0;
}

static void __exit mydriver_exit(void){
	printk(KERN_INFO "Driver says goodbye!\n");
}

module_init(mydriver_init);
module_exit(mydriver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KRANTHI");
MODULE_DESCRIPTION("Testing the driver template for creating,loading and unloading modules");