/* TPMK - A minimal tpm driver
 * HanishKVC, 2018
 */
#include "tpmk_driver.h"
#include "tpmk_lib.h"

struct resource *gpMyAdda;
void *gpBase;
void *gpGPIOMemBase, *gpILBBase;

int gCurLocality = 0;
int gbMITpmDoClear = 0;
int gbMITpmDoInitAuths = 0;
int gbMITpmDoVerifyAuths = 0;
int gbMIDebugTpmWriteDumpFullCommand = 0;
int gbMIDebugTpmReadDumpFullResponse = 0;

module_param(gbMITpmDoClear, int, 0);
MODULE_PARM_DESC(gbMITpmDoClear, "\n 1 to clear a TPM \n 0 to disable TPM clearing \n default: 0");
module_param(gbMITpmDoInitAuths, int, 0);
MODULE_PARM_DESC(gbMITpmDoInitAuths, "\n 0 to bypass Auth Initing \n 1 to init PlatformAuth only \n 4 or More to init All Auths, reqd for a new TPM or after TPMClear \n default: 0");
module_param(gbMITpmDoVerifyAuths, int, 0);
MODULE_PARM_DESC(gbMITpmDoVerifyAuths, "\n 0 to bypass Auth Verifying \n 1 to verify All Auths \n default: 0");
module_param(gbMIDebugTpmWriteDumpFullCommand, int, 0);
MODULE_PARM_DESC(gbMIDebugTpmWriteDumpFullCommand, "\n 0 disabled \n 1 to Dump the TPM Command buffer in dev_write \n default: 0");
module_param(gbMIDebugTpmReadDumpFullResponse, int, 0);
MODULE_PARM_DESC(gbMIDebugTpmReadDumpFullResponse, "\n 0 disabled \n 1 to Dump the TPM Response buffer in dev_read \n default: 0");

int gbAlreadyOpen = 0;
uint8_t gcaDrvTpmCmd[4096];
uint8_t gcaDrvTpmResponse[4096];

void tpm_dump_info(void)
{
	uint32_t temp;
	uint32_t vid;

	printk(KERN_INFO MODULE_NAME "Checking for VendorId at %p", gpBase+L0_VID);
	temp = ioread8(gpBase+L0_VID);
	vid = ioread32(gpBase+L0_VID);
	printk(KERN_INFO MODULE_NAME "VendorId at %p = %x, %x\n", gpBase+L0_VID, vid, temp);
	tpm_lib_dump_info();
}

int tpm_wait_for(void *addr, int mask, int trueValue, int maxWaitCnt, char *msgPrefix)
{
	volatile int waitCnt = 0;

	do {
		if ((ioread8(addr) & mask) == trueValue) {
			printk(KERN_INFO MODULE_NAME ":%s: waitCnt = %d to get %p to have 0x%x for mask 0x%x\n", msgPrefix, waitCnt, addr, trueValue, mask); 
			return 0;
		}
		waitCnt += 1;
		msleep(MSLEEP_TPMWAITFOR);
	} while (waitCnt < maxWaitCnt);
	return -1;
}

int tpm_request_locality(int locality)
{
	void *tempAddr = gpBase+Lx_ACCESS(locality);

	// relinquish the current locality
	iowrite8(ACCESS_RELINQUISH, gpBase+Lx_ACCESS(gCurLocality));
	// get access to new locality
	printk(KERN_INFO MODULE_NAME "Using Access register at %p to request access", tempAddr);
	iowrite8(ACCESS_REQUESTUSE, gpBase+Lx_ACCESS(locality));
	if (tpm_wait_for(gpBase+Lx_ACCESS(locality), ACCESS_ACTIVE, ACCESS_ACTIVE, MAXWAITCNT_REQUESTLOCALITY, "RequestLocality") == 0) {
		gCurLocality = locality;
		printk(KERN_INFO MODULE_NAME "got access to locality %d", locality);
		return 0;
	} else {
		printk(KERN_INFO MODULE_NAME "Failed to get access to locality %d", locality);
	}
	return -1;
}

int sys_init(void)
{
	uint32_t gpioIOBase, gpioMemBase, ilbBase;
	uint32_t lpcc;
	// AD3, AD2, AD1, FrameB, AD0, ClkOut0, ClkRunB
	uint32_t lpcPins[10] = { 0x420, 0x430, 0x440, 0x450, 0x460, 0x470, 0x480, 0x0};
	uint32_t tempLpcPadConf;
	int i;

	ilbBase = pciconf_read32(0, 31, 0, 0x50);
	gpioIOBase = pciconf_read32(0, 31, 0, 0x48);
	gpioMemBase = pciconf_read32(0, 31, 0, 0x4c);

	printk(KERN_INFO MODULE_NAME "gpioIOBase=0x%x", gpioIOBase);
	printk(KERN_INFO MODULE_NAME "gpioMemBase=0x%x", gpioMemBase);
	printk(KERN_INFO MODULE_NAME "ilbBase=0x%x", ilbBase);
	gpioMemBase &= 0xfffff000;
	ilbBase &= 0xfffff000;

	gpILBBase = ioremap_nocache(ilbBase, 256);
	if (IS_ERR(gpILBBase)) {
		printk(KERN_ALERT MODULE_NAME "Oops, I couldnt remap ILBBase %x\n", ilbBase);
		return 2;
	} else {
		printk(KERN_INFO MODULE_NAME "Yo, I remapped %x to %p\n", ilbBase, gpILBBase);
	}
	gpGPIOMemBase = ioremap_nocache(gpioMemBase, 0x1000);
	if (IS_ERR(gpGPIOMemBase)) {
		printk(KERN_ALERT MODULE_NAME "Oops, I couldnt remap GPIOMemBase %x\n", gpioMemBase);
		return 2;
	} else {
		printk(KERN_INFO MODULE_NAME "Yo, I remapped %x to %p\n", gpioMemBase, gpGPIOMemBase);
	}

	lpcc = ioread32(gpILBBase+0x84);
	printk(KERN_INFO MODULE_NAME "LPCC=0x%x", lpcc);
	for (i = 0; lpcPins[i] != 0x0; i++) {
		tempLpcPadConf = ioread32(gpGPIOMemBase+lpcPins[i]);
		printk(KERN_INFO MODULE_NAME "CurPadConf %d:0x%x = 0x%x", i, lpcPins[i], tempLpcPadConf);
		iowrite32((tempLpcPadConf & 0xfffffff8)|0x1, gpGPIOMemBase+lpcPins[i]);
	}

	return 0;
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

int tpm_send(int locality, uint8_t *buf, int len)
{
	int cnt = 0;
	int burstCnt = 0;
	int gotStatus = 0;
	int remaining, toWrite;

	if (tpm_request_locality(locality) != 0)
		return -1;
	// In case the previous command didnt finish properly
	iowrite8(STATUS_CMDREADY, gpBase+Lx_STATUS(locality));
	// Verify the TPM is ready
	if (tpm_wait_for(gpBase+Lx_STATUS(locality), STATUS_CMDREADY, STATUS_CMDREADY, MAXWAITCNT_FORCMDREADY, "CmdReady4Send") != 0) {
		printk(KERN_ALERT MODULE_NAME "tpm_send:TPM seems to be busy");
		return -2;
	}

	while(cnt < len) {
		burstCnt = ioread8(gpBase+Lx_BURSTLEN_LSB(locality)) | (ioread8(gpBase+Lx_BURSTLEN_MSB(locality))<<8);
		if (burstCnt <= 0) {
			msleep(MSLEEP_TPMSEND_BURST);
		} else {
			remaining = len - cnt;
			if (burstCnt > remaining)
				toWrite = remaining;
			else
				toWrite = burstCnt;
			iowrite8_rep(gpBase+Lx_DFIFO(locality), &buf[cnt], toWrite);
			cnt += toWrite;
			for(gotStatus = 0; (gotStatus & STATUS_VALID) == 0;)
				gotStatus = ioread8(gpBase+Lx_STATUS(locality));
			if ((gotStatus & STATUS_DATAEXPECT) == 0) {
				if (cnt < len) {
					printk(KERN_ALERT MODULE_NAME "tpm_send: TPM outOfSync??? doesnt want remaining data %d", len-cnt);
					return -3;
				}
			} else {
				if (cnt >= len) {
					printk(KERN_ALERT MODULE_NAME "tpm_send:DEBUG CmdBuf: TPM wants more data, but I dont have any");
				}
			}
		}
	}
	iowrite8(STATUS_START, gpBase+Lx_STATUS(locality));

	return len;
}

int tpm_recv_helper(int locality, uint8_t *buf, int len)
{
	int cnt = 0;
	int toRead = 0;
	int remaining = 0;
	int burstCnt = 0;

	while(((ioread8(gpBase+Lx_STATUS(locality)) & (STATUS_VALID|STATUS_DATAAVAIL)) == (STATUS_VALID|STATUS_DATAAVAIL)) &&
			(cnt < len)) {
		burstCnt = ioread8(gpBase+Lx_BURSTLEN_LSB(locality)) | (ioread8(gpBase+Lx_BURSTLEN_MSB(locality))<<8);
		if (burstCnt <= 0) {
			msleep(MSLEEP_TPMRECVHLPR_BURST);
		} else {
			remaining = len-cnt;
			if (remaining > burstCnt)
				toRead = burstCnt;
			else
				toRead = remaining;
			ioread8_rep(gpBase+Lx_DFIFO(locality), &buf[cnt], toRead);
			cnt += toRead;
		}
	}
	return cnt;
}

int tpm_recv(int locality, uint8_t *buf, int len)
{
	int cnt = 0;
	int totalSize;

	if (len < RECV_INITIAL) {
		printk(KERN_ALERT MODULE_NAME "tpm_recv: Too short a buffer provided, cant even fit header");
		return -1;
	}

	// Verify the TPM is ready with response
	if (tpm_wait_for(gpBase+Lx_STATUS(locality), STATUS_VALID | STATUS_DATAAVAIL, STATUS_VALID | STATUS_DATAAVAIL, MAXWAITCNT_DATAAVAIL, "DataAvail4Recv") != 0) {
		printk(KERN_ALERT MODULE_NAME "tpm_recv:TPM seems to be empty/NotReady or out of sync");
		return -2;
	}

	cnt = tpm_recv_helper(locality, buf, RECV_INITIAL);
	if (cnt < RECV_INITIAL) {
		printk(KERN_ALERT MODULE_NAME "tpm_recv: Couldnt retrieve even initial header of response");
		return -3;
	}

	totalSize = be32_to_cpup((const __be32*)&buf[2]);
	if (totalSize > len) {
		printk(KERN_ALERT MODULE_NAME "tpm_recv: Too short a buffer provided, Need %d provided %d", totalSize, len);
		return -4;
	} else {
		printk(KERN_INFO MODULE_NAME "tpm_recv: ResponseSize %d, available buffer %d", totalSize, len);
	}

	cnt += tpm_recv_helper(locality, &buf[RECV_INITIAL], totalSize-RECV_INITIAL);
	if (cnt < totalSize) {
		printk(KERN_ALERT MODULE_NAME "tpm_recv: Couldnt retrieve remaining part of response");
		return -5;
	}

	// Verify the TPM has no more response
	if ((ioread8(gpBase+Lx_STATUS(locality)) & (STATUS_VALID|STATUS_DATAAVAIL)) == (STATUS_VALID|STATUS_DATAAVAIL)) {
		printk(KERN_ALERT MODULE_NAME "tpm_recv:TPM seems to have still more response (than required?)???");
		return -6;
	}

	iowrite8(STATUS_CMDREADY, gpBase+Lx_STATUS(locality));
	if (cnt != totalSize) {
		printk(KERN_ALERT MODULE_NAME "tpm_recv:NEED TO DEBUG: cnt != totalSize");
	}
	return totalSize;
}


static int dev_open(struct inode *inode, struct file *file)
{
	if (gbAlreadyOpen) {
		printk(KERN_ALERT MODULE_NAME ":Already open\n");
		return -EBUSY;
	} else {
		printk(KERN_INFO MODULE_NAME ":opened\n");
	}

	gbAlreadyOpen = 1;
	try_module_get(THIS_MODULE);
	return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
	gbAlreadyOpen = 0;
	module_put(THIS_MODULE);
	printk(KERN_INFO MODULE_NAME ":closed\n");
	return 0;
}

static ssize_t dev_read(struct file *filp, char *buf, size_t len, loff_t *offset)
{
	char *bufResp = gcaDrvTpmResponse;
	int bufRespLen = 4096;
	int iGot;

	iGot = tpm_recv(0, bufResp, bufRespLen);
	tpm_print_response_generic(bufResp, iGot, gbMIDebugTpmReadDumpFullResponse, "TPMDrvRead");
	if (copy_to_user(buf, bufResp, iGot) != 0) {
		return -EFAULT;
	}
	return iGot;
}

static ssize_t dev_write(struct file *filp, const char *buf, size_t len, loff_t *offset)
{
	char *bufCmd = gcaDrvTpmCmd;
	int bufCmdLen = 4096;
	int locality = 0;

	if (len > bufCmdLen) {
		return -EFAULT;
	}
	if (copy_from_user(bufCmd, buf, len) != 0) {
		return -EFAULT;
	}
	tpm_print_command_generic(bufCmd, len, gbMIDebugTpmWriteDumpFullCommand, "TPMDrvWrite");
	//locality = *offset;
	tpm_send(locality, bufCmd, len);
	return len;
}

struct file_operations fops = {
	.read = dev_read,
	.write = dev_write,
	.open = dev_open,
	.release = dev_release
};

int init_module(void)
{
	printk(KERN_INFO MODULE_NAME MODULE_MY_VERSION " In init_module\n");
	if (register_chrdev(DEV_MAJOR, MODULE_NAME, &fops) != 0) {
		printk(KERN_ALERT MODULE_NAME "ERROR: Couldnt get chr major %d\n", DEV_MAJOR);
		return -EIO;
	}
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
	pciconf_init();
	sys_init();
	tpm_init();
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO MODULE_NAME "In cleanup module\n");
	unregister_chrdev(DEV_MAJOR, MODULE_NAME);
	iounmap(gpBase);
	release_mem_region(ADDR_BASE, ADDR_LEN);
}

MODULE_ALIAS_CHARDEV_MAJOR(DEV_MAJOR);
