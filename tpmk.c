/* TPMK - A minimal tpm driver
 * HanishKVC, 2018
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ioport.h>

#define MODULE_NAME ":tpmk:"

#define ADDR_BASE 0xFED40000L
#define ADDR_LEN 0x5000

#define L0_VID 0x0F00

struct resource *gpMyAdda;
void *gpBase;

int init_module(void)
{
	printk(KERN_INFO MODULE_NAME "In init_module\n");
	gpMyAdda = request_mem_region(ADDR_BASE, ADDR_LEN, MODULE_NAME);
	if (gpMyAdda == NULL) {
		printk(KERN_ALERT MODULE_NAME "Oops, I didnt get my adda\n");
		return 1;
	}
	gpBase = ioremap_nocache(ADDR_BASE, ADDR_LEN);
	if (IS_ERR(gpBase)) {
		printk(KERN_ALERT MODULE_NAME "Oops, I couldnt remap %lx\n", PTR_ERR(gpBase));
		return 2;
	} else {
		printk(KERN_INFO MODULE_NAME "Yo, I remapped %lx to %p\n", ADDR_BASE, gpBase);
	}
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO MODULE_NAME "In cleanup module\n");
	iounmap(gpBase);
	release_mem_region(ADDR_BASE, ADDR_LEN);
}

