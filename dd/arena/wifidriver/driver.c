#define REALTEK_VENDER_ID  0x0a5c
#define REALTEK_DEVICE_ID   0x5800

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/stddef.h>
#include <linux/pci.h>
int init_module(void)
{
    struct pci_dev *pdev;
    pdev = pci_find_device(REALTEK_VENDER_ID, REALTEK_DEVICE_ID, NULL);
    if(!pdev)
        printk("<1>Device not found\n");
    else
        printk("<1>Device found\n");
    return 0;
}