/* TPMK - A minimal tpm driver
 * HanishKVC, 2018
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ioport.h>

#define MODULE_NAME ":tpmk:"

#define ADDR_BASE 0xFED40000L
#define ADDR_LEN 0x5000

#define Lx_ACCESS(l) (0x0000 | (l << 12))
#define Lx_STATUS(l) (0x0018 | (l << 12))
#define Lx_DFIFO(l)  (0x0024 | (l << 12))
#define L0_VID 0x0F00

#define ACCESS_REQUESTUSE 0x02
#define ACCESS_RELINQUISH 0x20
#define ACCESS_ACTIVE     0x20

struct resource *gpMyAdda;
void *gpBase;

void tpm_dump_info(void)
{
	uint32_t temp;
	uint32_t vid;

	printk(KERN_INFO MODULE_NAME "Checking for VendorId at %p", gpBase+L0_VID);
	temp = ioread8(gpBase+L0_VID);
	vid = ioread32(gpBase+L0_VID);
	printk(KERN_INFO MODULE_NAME "VendorId at %p = %x, %x\n", gpBase+L0_VID, vid, temp);
}

#define MAXWAITCNT_REQUESTLOCALITY 20

int tpm_request_locality(int locality)
{
	void *tempAddr = gpBase+Lx_ACCESS(locality);
	int cnt = 0;

	printk(KERN_INFO MODULE_NAME "Using Access register at %p to request access", tempAddr);
	iowrite8(ACCESS_REQUESTUSE, gpBase+Lx_ACCESS(locality));
	do {
		if (ioread8(gpBase+Lx_ACCESS(locality)) & ACCESS_ACTIVE) {
			printk(KERN_INFO MODULE_NAME "Waited cnt = %d to get access to locality %d", cnt, locality);
			return 0;
		}
		cnt += 1;
	}while (cnt < MAXWAITCNT_REQUESTLOCALITY);
	return -1;
}

int tpm_init(void)
{
	int i;
	for (i = 0; i < 5; i++)
		iowrite8(ACCESS_RELINQUISH, gpBase+Lx_ACCESS(i));
	if (tpm_request_locality(0) != 0)
		return -1;
	tpm_dump_info();
	return 0;
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
	return tpm_init();
}

void cleanup_module(void)
{
	printk(KERN_INFO MODULE_NAME "In cleanup module\n");
	iounmap(gpBase);
	release_mem_region(ADDR_BASE, ADDR_LEN);
}

