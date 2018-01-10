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

uint8_t gcaTpmPCRRead_SHA1 [0x14] = {
	0x80, 0x01,			/* TPM_ST_NO_SESSIONS */
	0x00, 0x00, 0x00, 0x14,		/* Size */
	0x00, 0x00, 0x01, 0x7e,		/* CommandCode: TPM_CC_PCR_Read */
	0x00, 0x00, 0x00, 0x01,		/* TPML_PCR_SELECTION.count */
	0x00, 0x04,			/* TPML_PCR_SELECTION.TPMS_PCR_SELECTION[0].hash_TPMI_ALG_HASH=TPM_ALG_SHA1 */
	0x03,				/* TPML_PCR_SELECTION.TPMS_PCR_SELECTION[0].sizeofSelect=8x3=24 PCRs */
	0xff, 0xff, 0xff		/* TPML_PCR_SELECTION.TPMS_PCR_SELECTION[0].pcrSelect = All 24 PCRs */
};

uint8_t gcaTpmPCRRead_SHA256 [0x14] = {
	0x80, 0x01,			/* TPM_ST_NO_SESSIONS */
	0x00, 0x00, 0x00, 0x14,		/* Size */
	0x00, 0x00, 0x01, 0x7e,		/* CommandCode: TPM_CC_PCR_Read */
	0x00, 0x00, 0x00, 0x01,		/* TPML_PCR_SELECTION.count */
	0x00, 0x0b,			/* TPML_PCR_SELECTION.TPMS_PCR_SELECTION[0].hash_TPMI_ALG_HASH=TPM_ALG_SHA256 */
	0x03,				/* TPML_PCR_SELECTION.TPMS_PCR_SELECTION[0].sizeofSelect=8x3=24 PCRs */
	0xff, 0xff, 0xff		/* TPML_PCR_SELECTION.TPMS_PCR_SELECTION[0].pcrSelect = All 24 PCRs */
};

uint8_t gcaTpmResponse[4096];
int gbDebug_TpmCommandDumpFullResponse = 0;

int tpm_command(int locality, uint8_t *inBuf, int inSize, uint8_t *outBuf, int outSize, char *msg)
{
	int i, iGot;
	uint32_t respCode;
	uint32_t respCode_11_08;

	printk(KERN_INFO MODULE_NAME "command:%s: Input buffer size %d, Output buffer size %d\n", msg, inSize, outSize);
	tpm_send(locality, inBuf, inSize);
	iGot = tpm_recv(0, outBuf, outSize);
	printk("ResponseTag : 0x%.4x\n", be16_to_cpup((__be16*)outBuf));
	printk("ResponseSize: 0x%.8x\n", be32_to_cpup((__be32*)&outBuf[2]));
	respCode = be32_to_cpup((__be32*)&outBuf[6]);
	printk("ResponseCode: 0x%.8x\n", respCode);
	if (respCode != 0) {
		printk("ERR:Mugambo Kush Nai hai hoo haa");
		if (respCode & 0x80) {
			respCode_11_08 = (respCode & 0xf00) >> 8;
			printk("INFO: Format-One Response code");
			if (respCode & 0x40) {
				printk("\tINFO: Parameter Number [%d] has Error", respCode_11_08);
			} else {
				if (respCode_11_08 == 0) {
					printk("\tINFO: 0x%x : HandleError Or Not able to pin it to handle, session, parameter", respCode_11_08);
				} else {
					if ((respCode_11_08 > 0) && (respCode_11_08 <= 7) ){
						printk("\tINFO: 0x%x : HandleError ?Parameter Number?", respCode_11_08);
					} else {
						printk("\tINFO: 0x%x : SessionError ?Session Number?", respCode_11_08);
					}
				}
			}
			printk("INFO: Error [0x%x]\n", respCode & 0x3f);
		} else {
			printk("INFO: Format-Zero Response code");
			if (respCode & 0x100) {
				printk("\tINFO: Error defined in Version: 2.0, ResponseTag should be TPM_ST_NO_SESSIONS");
			} else {
				printk("\tINFO: Error defined in Version: ?1.2?, ResponseTag should be TPM_TAG_RSP_COMMAND");
			}
			if (respCode & 0x400) {
				printk("\tINFO: Error defined by TPM Vendor");
			} else {
				printk("\tINFO: Error defined by TCG");
			}
			if (respCode & 0x800) {
				printk("\tINFO: Severity is Warning, ? What TPM What ?");
			} else {
				printk("\tINFO: Severity is Command has Error");
			}
			printk("INFO: Error [0x%x]\n", respCode & 0x7f);
		}
	}
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

void tpm_pcr_read(void)
{
	int i, iGot;
	int iPos;
	int j;
	int iNumOfPcrSelections, iSizeOfSelect;

	iGot = tpm_command(0, gcaTpmPCRRead_SHA1, sizeof(gcaTpmPCRRead_SHA1),
			gcaTpmResponse, sizeof(gcaTpmResponse), "TpmPCRRead SHA1");
	printk("pcrUpdateCounter:0x%.8x\n", be32_to_cpup((__be32*)&gcaTpmResponse[10]));
	iNumOfPcrSelections = be32_to_cpup((__be32*)&gcaTpmResponse[14]);
	printk("NumOfPcrSelections in Response [0x%.8x]\n", iNumOfPcrSelections);
	iPos = 14+4;
	for(i = 0; i < iNumOfPcrSelections; i++) {
		printk("Group[%d] HashAlgo is [0x%.4x]", i, be16_to_cpup((__be16*)&gcaTpmResponse[iPos]));
		iPos += 2;
		iSizeOfSelect = gcaTpmResponse[iPos];
		printk("Group[%d] sizeOfSelect is [0x%.2x]", i, iSizeOfSelect);
		iPos += 1;
		for(j = 0; j < iSizeOfSelect; j++) {
			printk("Group[%d] PCRSelect[%d] = 0x%.2x", i, j, gcaTpmResponse[iPos]);
			iPos += 1;
		}
	}
	for(i = iPos; i < iGot; i+=8) {
		printk("0x%.8x : 0x%.8x\n", be32_to_cpup((__be32*)&gcaTpmResponse[i]), be32_to_cpup((__be32*)&gcaTpmResponse[i+4]));
	}
}

void tpm_lib_dump_info(void)
{
	tpm_startup();
	tpm_get_capabilities();
	tpm_getcap_ptfixed();
	tpm_pcr_read();
}

