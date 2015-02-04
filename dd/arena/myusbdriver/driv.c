#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/kmod.h>
#include <linux/slab.h>
#include <asm/siginfo.h>  //siginfo
#include <linux/rcupdate.h> //rcu_read_lock
#include <linux/sched.h>  //find_task_by_pid_type
#include <linux/debugfs.h>
#include <linux/uaccess.h>


#define SIG_TEST 44 // we choose 44 as our signal number (real-time signals are in the range of 33 to 64)

struct dentry *file;

static ssize_t write_pid(struct file *file, const char __user *buf,
                                size_t count, loff_t *ppos)
{
  char mybuf[10];
  int pid = 0;
  int ret;
  struct siginfo info;
  struct task_struct *t;
  /* read the value from user space */
  if(count > 10)
    return -EINVAL;
  copy_from_user(mybuf, buf, count);
  sscanf(mybuf, "%d", &pid);
  printk("pid = %d\n", pid);

  /* send the signal */
  memset(&info, 0, sizeof(struct siginfo));
  info.si_signo = SIG_TEST;
  info.si_code = SI_QUEUE;  
  info.si_int = 1234;     //real time signals may have 32 bits of data.

  rcu_read_lock();
//  t = find_task_by_pid_type(PIDTYPE_PID, pid);  //find the task_struct associated with this pid
  t = pid_task(find_pid_ns(pid, &init_pid_ns), PIDTYPE_PID);  
  if(t == NULL){
    printk("no such pid\n");
    rcu_read_unlock();
    return -ENODEV;
  }
  rcu_read_unlock();
  ret = send_sig_info(SIG_TEST, &info, t);    //send the signal
  if (ret < 0) {
    printk("error sending signal\n");
    return ret;
  }
  return count;
}


static const struct file_operations my_fops = {
  .write = write_pid,
};

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
    file = debugfs_create_file("signalconfpid", 0200, NULL, NULL, &my_fops);
    return 0;
}


static void pen_disconnect(struct usb_interface *interface)
{
    debugfs_remove(file);
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
MODULE_DESCRIPTION("USB Pen Watchman");
