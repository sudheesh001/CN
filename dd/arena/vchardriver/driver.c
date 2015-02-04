#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/version.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/cdev.h>

static dev_t first;
static struct cdev char_dev_st;
static struct class *dev_class;

static int open_driver(struct inode *i,struct file *f){
	printk(KERN_INFO "Driver: open()\n");
	return 0;
}
static int close_driver(struct inode *i,struct file *f){
	printk(KERN_INFO "Driver: close()\n");
	return 0;
}

//loff is length offset, __user is from init
static ssize_t read_driver(struct file *f,char __user *buf, size_t len,loff_t *off){
printk(KERN_INFO "Driver: read()\n");
return 0;
}


//loff is length offset, __user is from init
static ssize_t write_driver(struct file *f,char __user *buf, size_t len,loff_t *off){
printk(KERN_INFO "Driver: write()\n");
printk(KERN_INFO "Buffer: %s\n",buf);
return len;
}

static struct file_operations chr_fops =
{
.owner = THIS_MODULE,
.open=open_driver,
.release=close_driver,
.read=read_driver,
.write=write_driver
};

char *name="char";

static int __init mydriver_init(void){
	printk(KERN_INFO "charDriver: Registered\n");
	if(alloc_chrdev_region(&first, 0 ,3, name)){
		return -1;
	}
	if((dev_class = class_create(THIS_MODULE,"chrdrive"))==NULL){
		unregister_chrdev_region(first,1);
		return -1;
	}
	if(device_create(dev_class,NULL,first,NULL,"chrdev")==NULL){
		class_destroy(dev_class);
		unregister_chrdev_region(first,1);
		return -1;
	}
	cdev_init(&char_dev_st,&chr_fops);
	if(cdev_add(&char_dev_st,first,1)==-1){
		device_destroy(dev_class,first);
		class_destroy(dev_class);
		unregister_chrdev_region(first,1);
		return -1;
	}
 printk(KERN_INFO "<major, minor> : <%d, %d>\n",MAJOR(first),MINOR(first));
	return 0;
}

static void __exit mydriver_exit(void){
	cdev_del(&char_dev_st);
	device_destroy(dev_class,first);
	class_destroy(dev_class);
	unregister_chrdev_region(first,1);
	printk(KERN_INFO "charDriver: unregistered");
}

module_init(mydriver_init);
module_exit(mydriver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KRANTHI");
MODULE_DESCRIPTION("Character Driver sample driver");