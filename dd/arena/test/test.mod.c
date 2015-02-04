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
	{        0, __VMLINUX_SYMBOL_STR(sys_close) },
	{ 0xdcf68015, __VMLINUX_SYMBOL_STR(fput) },
	{ 0x8350fc86, __VMLINUX_SYMBOL_STR(vfs_write) },
	{ 0xfd97284a, __VMLINUX_SYMBOL_STR(fget) },
	{ 0xd0d8621b, __VMLINUX_SYMBOL_STR(strlen) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "73200028BC5E5974C897F56");
