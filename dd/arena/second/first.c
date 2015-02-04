#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init first_init(void){
	printk(KERN_INFO "First driver registered\n");
	return 0;
}

static void __exit first_exit(void){
	printk(KERN_INFO "First driver says goodbye!\n");
}

module_init(first_init);
module_exit(first_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KRANTHI");
MODULE_DESCRIPTION("Testing the tutorial for creating,loading and unloading modules");