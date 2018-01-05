/* TPMK - A minimal tpm driver
 * HanishKVC, 2018
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ioport.h>

#define MODULE_NAME ":tpmk:"

#define ADDR_BASE 0xFED40000
#define ADDR_LEN 0x5000

struct resource *gpMyAdda;

int init_module(void)
{
	printk(KERN_INFO MODULE_NAME "In init_module\n");
	gpMyAdda = request_mem_region(ADDR_BASE, ADDR_LEN, MODULE_NAME);
	if (gpMyAdda == NULL) {
		printk(KERN_ALERT MODULE_NAME "Oops, I didnt get my adda");
		return 1;
	}
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO MODULE_NAME "In cleanup module\n");
	release_mem_region(ADDR_BASE, ADDR_LEN);
}

