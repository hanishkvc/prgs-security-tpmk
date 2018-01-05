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

unsigned char *gpHardBase = (unsigned char*)ADDR_BASE;

struct resource *gpMyAdda;
void *gpBase;

void dump_info(void)
{
	uint32_t temp;
	uint32_t vid;
	unsigned char *gpHardBaseVID = gpHardBase+L0_VID;

	printk(KERN_INFO MODULE_NAME "Checking for VendorId at %p", gpBase+L0_VID);
	temp = ioread8(gpBase+L0_VID);
	vid = ioread32(gpBase+L0_VID);
	printk(KERN_INFO MODULE_NAME "VendorId at %p = %x, %x\n", gpBase+L0_VID, vid, temp);
	//below crashes as expected
	//vid = *(uint32_t*)(gpHardBase+L0_VID);
	//printk(KERN_INFO MODULE_NAME "VendorId at %p = %x\n", gpHardBaseVID, vid);
}

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
	dump_info();
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO MODULE_NAME "In cleanup module\n");
	iounmap(gpBase);
	release_mem_region(ADDR_BASE, ADDR_LEN);
}

