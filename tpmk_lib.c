/*
 * tpmk_lib.c
 * HanishKVC, 2018
 */
#include "tpmk_driver.h"

uint8_t gcaTpm2Startup [0x0c] = {
	0x80, 0x01,			/* TAG: TPM_ST_NO_SESSIONS */
	0x00, 0x00, 0x00, 0x0c,		/* Size */
	0x00, 0x00, 0x01, 0x44,		/* CommandCode: TPM_CC_Startup */
	0x00, 0x00			/* TPM_SU_CLEAR */
};

uint8_t gcaGetCap_TPM_PT_FIXED [0x16] = {
	0x80, 0x01,			/* TPM_ST_NO_SESSIONS */
	0x00, 0x00, 0x00, 0x16,		/* Size */
	0x00, 0x00, 0x01, 0x7a,		/* CommandCode: TPM_CC_GetCapability */
	0x00, 0x00, 0x00, 0x06,		/* Capability: TPM_CAP_TPM_PROPERTIES */
	0x00, 0x00, 0x01, 0x00,		/* Property: TPM_PT_FAMILY_INDICATOR */
	0x00, 0x00, 0x00, 0x45		/* Property Count */
};

uint8_t gcaGetCap_TPM_PT_MANUFACTURER [0x16] = {
	0x80, 0x01,			/* TPM_ST_NO_SESSIONS */
	0x00, 0x00, 0x00, 0x16,		/* Size */
	0x00, 0x00, 0x01, 0x7a,		/* CommandCode: TPM_CC_GetCapability */
	0x00, 0x00, 0x00, 0x06,		/* Capability: TPM_CAP_TPM_PROPERTIES */
	0x00, 0x00, 0x01, 0x05,		/* Property: TPM_PT_MANUFACTURER */
	0x00, 0x00, 0x00, 0x01		/* Property Count */
};

uint8_t gcaTpmResponse[4096];
int gbDebug_TpmCommandDumpFullResponse = 0;

int tpm_command(int locality, uint8_t *inBuf, int inSize, uint8_t *outBuf, int outSize, char *msg)
{
	int i, iGot;

	printk(KERN_INFO MODULE_NAME "command:%s: Input buffer size %d, Output buffer size %d\n", msg, inSize, outSize);
	tpm_send(locality, inBuf, inSize);
	iGot = tpm_recv(0, outBuf, outSize);
	printk("ResponseTag : 0x%.4x\n", be16_to_cpup((__be16*)outBuf));
	printk("ResponseSize: 0x%.8x\n", be32_to_cpup((__be32*)&outBuf[2]));
	printk("ResponseCode: 0x%.8x\n", be32_to_cpup((__be32*)&outBuf[6]));
	if (gbDebug_TpmCommandDumpFullResponse == 1) {
		for(i = 10; i < iGot; i++) {
			printk("%.4d=0x%.2x [%c], ", i, outBuf[i], outBuf[i]);
		}
	}
	printk(KERN_INFO MODULE_NAME "command: done\n");
	return iGot;
}

void tpm_startup(void)
{
	tpm_command(0, gcaTpm2Startup, sizeof(gcaTpm2Startup),
			gcaTpmResponse, sizeof(gcaTpmResponse), "Tpm2Startup");
}

void tpm_getcap_ptfixed(void)
{
	int i, iGot;

	iGot = tpm_command(0, gcaGetCap_TPM_PT_FIXED, sizeof(gcaGetCap_TPM_PT_FIXED),
			gcaTpmResponse, sizeof(gcaTpmResponse), "GetCap_PTFIXED All");
	printk("moreData:0x%x\n", gcaTpmResponse[10]);
	for(i = 11; i < iGot; i+=8) {
		printk("0x%.8x : 0x%.8x\n", be32_to_cpup((__be32*)&gcaTpmResponse[i]), be32_to_cpup((__be32*)&gcaTpmResponse[i+4]));
	}
}

void tpm_get_capabilities(void)
{
	int i, iGot;

	printk(KERN_INFO MODULE_NAME "GetCap:TPM_PT_MANUFACTURER");
	tpm_send(0, gcaGetCap_TPM_PT_MANUFACTURER, sizeof(gcaGetCap_TPM_PT_MANUFACTURER));
	iGot = tpm_recv(0, gcaTpmResponse, 4096);
	for(i = 0; i < iGot; i++) {
		printk("%d=%x, ", i, gcaTpmResponse[i]);
	}
	printk(KERN_INFO MODULE_NAME "GetCap:Done\n");
}

