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
	{ 0x106a5013, __VMLINUX_SYMBOL_STR(usb_deregister) },
	{ 0xc03fcaf3, __VMLINUX_SYMBOL_STR(usb_register_driver) },
	{ 0x4f8b5ddb, __VMLINUX_SYMBOL_STR(_copy_to_user) },
	{ 0xc5e1758f, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x58ce3621, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x50eedeb8, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xb4390f9a, __VMLINUX_SYMBOL_STR(mcount) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("usb:v0951p1665d*dc*dsc*dp*ic*isc*ip*in*");

MODULE_INFO(srcversion, "00252B1C0D010B08C9E980A");
