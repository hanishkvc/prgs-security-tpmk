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
	0x00, 0x00, 0x00, 0x02		/* Property Count */
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

void tpm_command(int locality, uint8_t *buf, int size, char *msg)
{
	int i, iGot;

	printk(KERN_INFO MODULE_NAME "command:%s: buffer size %d\n", msg, size);
	tpm_send(locality, buf, size);
	iGot = tpm_recv(0, gcaTpmResponse, 4096);
	for(i = 0; i < iGot; i++) {
		printk("%d=%x, ", i, gcaTpmResponse[i]);
	}
	printk(KERN_INFO MODULE_NAME "command: done\n");

}

void tpm_startup(void)
{
	tpm_command(0, gcaTpm2Startup, sizeof(gcaTpm2Startup), "Tpm2Startup");
}

void tpm_getcap_ptfixed(void)
{
	tpm_command(0, gcaGetCap_TPM_PT_FIXED, sizeof(gcaGetCap_TPM_PT_FIXED), "GetCap_PTFIXED All");
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

