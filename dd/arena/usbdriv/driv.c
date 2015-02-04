#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/kmod.h>
#include <linux/slab.h>

int leng(char* buf){
    int i=0;
    while(buf[i]!='\0'){
        i++;
    }
    return i;
}

static int pen_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    printk(KERN_INFO "Pen drive (%04X:%04X) plugged\n", id->idVendor, id->idProduct);
    static char buffer[2];
    buffer[0]='1';
    buffer[1]='\0';
    char *b;
    b=kmalloc(sizeof(char)*40,GFP_KERNEL);
    copy_to_user(buffer,b,leng(buffer));
    return 0;
}


static void pen_disconnect(struct usb_interface *interface)
{
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
 
static int __init pen_init(void)
{
    return usb_register(&pen_driver);
}
 
static void __exit pen_exit(void)
{
    usb_deregister(&pen_driver);
}
 
module_init(pen_init);
module_exit(pen_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("KRANTHI");
MODULE_DESCRIPTION("USB Pen Registration Driver");
