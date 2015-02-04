#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/stat.h> //for permissions
#include <linux/moduleparam.h>

// name,type,permissions in sysfs
static int var=3;
module_param(var,int,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
MODULE_PARM_DESC(var,"An integer");
// name,type,pointer size of array at loading,permission
static int var_array[3];
static int count=0;
module_param_array(var_array,int,&count,0);
MODULE_PARM_DESC(var_array,"An integer array");
//name,type,permissions in sysfs
//charp is char*
static char *string = "hello world\n";
module_param(string,charp,0000);

static int __init third_init(void){
	printk(KERN_INFO "Third driver registered");
	printk(KERN_INFO "value of `var` integer is: %d\n",var);
	printk(KERN_INFO "string : %s\n",string);
	int i;
	for (i = 0; i < (sizeof var_array/sizeof(int)); ++i)
	{
		printk(KERN_INFO "var_array[%d] = %d \n",i,var_array[i]);
	}
	printk(KERN_INFO "got %d arguments for var_array \n",count);
	return 0;
}

static void __exit third_exit(void){
	printk(KERN_INFO "Third driver unregistered\n");
}

module_init(third_init);
module_exit(third_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KRANTHI");