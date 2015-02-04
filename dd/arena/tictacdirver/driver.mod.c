#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x45772e10, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x69f2de97, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0xcdfdd5c2, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x9d173faa, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0xa562dec1, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0x64004681, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x8872adc7, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x28b3b2f3, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0xb742fd7, __VMLINUX_SYMBOL_STR(simple_strtol) },
	{ 0x50eedeb8, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xb4390f9a, __VMLINUX_SYMBOL_STR(mcount) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "9A8776D25928D5C07C18698");
