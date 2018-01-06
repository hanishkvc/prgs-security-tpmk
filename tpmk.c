/* TPMK - A minimal tpm driver
 * HanishKVC, 2018
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/delay.h>

#define MODULE_NAME ":tpmk:"
#define MODULE_MY_VERSION "v20180105_2054UTC"

#define ADDR_BASE 0xFED40000L
#define ADDR_LEN 0x5000

#define Lx_ACCESS(l)		(0x0000 | (l << 12))
#define Lx_STATUS(l)		(0x0018 | (l << 12))
#define Lx_BURSTLEN_LSB(l)	(0x0019 | (l << 12))
#define Lx_BURSTLEN_MSB(l)	(0x001A | (l << 12))
#define Lx_DFIFO(l)		(0x0024 | (l << 12))
#define L0_VID			0x0F00

#define ACCESS_REQUESTUSE	0x02
#define ACCESS_RELINQUISH	0x20
#define ACCESS_ACTIVE		0x20

#define STATUS_VALID		0X80
#define STATUS_CMDREADY		0X40
#define STATUS_DATAAVAIL	0X10
#define STATUS_DATAEXPECT	0x08
#define STATUS_START		0x20

struct resource *gpMyAdda;
void *gpBase;

int gCurLocality = 0;

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
	volatile int waitCnt = 0;

	// relinquish the current locality
	iowrite8(ACCESS_RELINQUISH, gpBase+Lx_ACCESS(gCurLocality));
	// get access to new locality
	printk(KERN_INFO MODULE_NAME "Using Access register at %p to request access", tempAddr);
	iowrite8(ACCESS_REQUESTUSE, gpBase+Lx_ACCESS(locality));
	do {
		if (ioread8(gpBase+Lx_ACCESS(locality)) & ACCESS_ACTIVE) {
			gCurLocality = locality;
			printk(KERN_INFO MODULE_NAME "waitCnt = %d to get access to locality %d", waitCnt, locality);
			return 0;
		}
		waitCnt += 1;
		msleep(5);
	}while (waitCnt < MAXWAITCNT_REQUESTLOCALITY);
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

#define MAXWAITCNT_FORCMDREADY 20

int tpm_send(int locality, uint8_t *buf, int len)
{
	int cnt = 0;
	int burstCnt = 0;
	int gotStatus = 0;
	int waitCnt;

	if (tpm_request_locality(locality) != 0)
		return -1;
	// In case the previous command didnt finish properly
	iowrite8(STATUS_CMDREADY, gpBase+Lx_STATUS(locality));
	// Verify the TPM is ready
	waitCnt = 0;
	do {
		if ((ioread8(gpBase+Lx_STATUS(locality)) & STATUS_CMDREADY) == STATUS_CMDREADY)
			break;
		waitCnt += 1;
		msleep(5);
	} while (waitCnt < MAXWAITCNT_FORCMDREADY);
	if (waitCnt == MAXWAITCNT_FORCMDREADY)
		return -2;

	while(cnt < len) {
		burstCnt = ioread8(gpBase+Lx_BURSTLEN_LSB(locality)) | (ioread8(gpBase+Lx_BURSTLEN_MSB(locality))<<8);
		if (burstCnt <= 0) {
			msleep(1);
		} else {
			iowrite8_rep(gpBase+Lx_DFIFO(locality), &buf[cnt], burstCnt);
			cnt += burstCnt;
			for(gotStatus = 0; (gotStatus & STATUS_VALID) == 0;)
				gotStatus = ioread8(gpBase+Lx_STATUS(locality));
			if ((gotStatus & STATUS_DATAEXPECT) == 0)
				return -3;
		}
	}
	iowrite8(STATUS_START, gpBase+Lx_STATUS(locality));
	return len;
}

int init_module(void)
{
	printk(KERN_INFO MODULE_NAME MODULE_MY_VERSION " In init_module\n");
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

