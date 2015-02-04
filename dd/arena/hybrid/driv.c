#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/kmod.h>
#include <linux/slab.h>
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
static int tmp;
static char c;

//loff is length offset, __user is from init
static ssize_t read_driver(struct file *f,char __user *buf, size_t len,loff_t *off){
printk(KERN_INFO "Driver: read()\n");

if(tmp==0)
    c='0';
else if(tmp==1)
    c='1';
copy_to_user(buf,&c,1);

return 0;
}
//loff is length offset, __user is from init
static ssize_t write_driver(struct file *f,char __user *buf, size_t len,loff_t *off){
printk(KERN_INFO "Driver: write()\n");
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



static int pen_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    printk(KERN_INFO "Pen drive (%04X:%04X) plugged\n", id->idVendor, id->idProduct);
    tmp=1;
    return 0;
}


static void pen_disconnect(struct usb_interface *interface)
{
    tmp=0;
    printk(KERN_INFO "Pen drive removed\n");
}
 
static struct usb_device_id pen_table[] =
{
    { USB_DEVICE(0x0951, 0x1665) },
    {} /* Terminating entry */
};
MODULE_DEVICE_TABLE (usb, pen_table);
 
static struct usb_driver pen_driver =
{
    .name = "pen_driver",
    .id_table = pen_table,
    .probe = pen_probe,
    .disconnect = pen_disconnect,
};

char *name="chardevice";

static int __init pen_init(void)
{
    printk(KERN_INFO "VDriver: Registered\n");
    if(alloc_chrdev_region(&first, 0 ,3, name)){
        return -1;
    }
    if((dev_class = class_create(THIS_MODULE,"vchrdrive"))==NULL){
        unregister_chrdev_region(first,1);
        return -1;
    }
    if(device_create(dev_class,NULL,first,NULL,"vchrdev")==NULL){
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

    return usb_register(&pen_driver);
}
 
static void __exit pen_exit(void)
{
    usb_deregister(&pen_driver);
    cdev_del(&char_dev_st);
    device_destroy(dev_class,first);
    class_destroy(dev_class);
    unregister_chrdev_region(first,1);
    printk(KERN_INFO "VDriver: unregistered");
}
 
module_init(pen_init);
module_exit(pen_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("KRANTHI");
MODULE_DESCRIPTION("USB Pen Hybrid Virtual adhoc Hardware Driver");
