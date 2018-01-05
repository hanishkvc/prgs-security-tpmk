/* TPMK - A minimal tpm driver
 * HanishKVC, 2018
 */
#include <linux/module.h>
#include <linux/kernel.h>

#define MODULE_NAME ":tpmk:"

int init_module(void)
{
	printk(KERN_INFO MODULE_NAME "In init_module\n");
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO MODULE_NAME "In cleanup module\n");
}

