#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int var_data __initdata = 3;

static int __init second_init(void){
	var_data+=2;
	printk(KERN_INFO "second driver registered, var_data is %d\n",var_data);
	return 0;
}
static void __exit second_exit(void){
	printk(KERN_INFO "second driver unregistered\n");
}

module_init(second_init);
module_exit(second_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KRANTHI");