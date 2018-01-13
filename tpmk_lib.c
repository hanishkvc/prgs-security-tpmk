/*
 * tpmk_lib.c
 * HanishKVC, 2018
 */
#include "tpmk_driver.h"
#include "tpmk_lib.h"

uint8_t gcaTpm2Startup [0x0c] = {
	0x80, 0x01,			/* TAG: TPM_ST_NO_SESSIONS */
	0x00, 0x00, 0x00, 0x0c,		/* Size */
	0x00, 0x00, 0x01, 0x44,		/* CommandCode: TPM_CC_Startup */
	0x00, 0x00			/* TPM_SU_CLEAR */
};

uint8_t gcaTpm2ReadClock [0x0a] = {
	0x80, 0x01,			/* TAG: TPM_ST_NO_SESSIONS */
	0x00, 0x00, 0x00, 0x0a,		/* Size */
	0x00, 0x00, 0x01, 0x81		/* CommandCode: TPM_CC_ReadClock */
};

uint8_t gcaGetCap_TPM_PT_FIXED [0x16] = {
	0x80, 0x01,			/* TPM_ST_NO_SESSIONS */
	0x00, 0x00, 0x00, 0x16,		/* Size */
	0x00, 0x00, 0x01, 0x7a,		/* CommandCode: TPM_CC_GetCapability */
	0x00, 0x00, 0x00, 0x06,		/* Capability: TPM_CAP_TPM_PROPERTIES */
	0x00, 0x00, 0x01, 0x00,		/* Property: TPM_PT_FAMILY_INDICATOR */
	0x00, 0x00, 0x00, 0x45		/* Property Count */
};

uint8_t gcaGetCap_TPM_PT_LOCKOUTPLUS [0x16] = {
	0x80, 0x01,			/* TPM_ST_NO_SESSIONS */
	0x00, 0x00, 0x00, 0x16,		/* Size */
	0x00, 0x00, 0x01, 0x7a,		/* CommandCode: TPM_CC_GetCapability */
	0x00, 0x00, 0x00, 0x06,		/* Capability: TPM_CAP_TPM_PROPERTIES */
	0x00, 0x00, 0x02, 0x14,		/* Property: TPM_PT_LOCKOUT_COUNTER */
	0x00, 0x00, 0x00, 0x04		/* Property Count */
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
	/* pcr07-pcr00, pcr15-pcr08, pcr23-pcr16 */
	0xff, 0xff, 0xff		/* TPML_PCR_SELECTION.TPMS_PCR_SELECTION[0].pcrSelect = All 24 PCRs */
	/* if only interested in pcr0, then instead of 0x03, 0xff, 0xff, 0xff use 0x01, 0x01 and reduce size accordingly */
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

uint8_t gcaTpmPCRExtend_SHA1 [0x35] = {
	0x80, 0x02,			/* TPM_ST_SESSIONS */
	0x00, 0x00, 0x00, 0x35,		/* Size */
	0x00, 0x00, 0x01, 0x82,		/* CommandCode: TPM_CC_PCR_Extend */
	/* TPM_HANDLE */
	0x00, 0x00, 0x00, 0x00,		/* TPMI_DH_PCR=TPM_HANDLE(8bits:TPM_HT_PCR,24Bits) */
	0x00, 0x00, 0x00, 0x09,		/* TPMS_AUTH_COMMAND Structure size */
	/* TPMS_AUTH_COMMAND */
	0x40, 0x00, 0x00, 0x09,		/* TPMI_SH_AUTH_SESSION=>TPM_RS_PW */
	0x00, 0x00,			/* A empty TPM2B_NONCE */
	0x01,				/* continue the TPMA_SESSION, attribute */
	0x00, 0x00,			/* TPM2B_AUTH: A Null Authorisation */
	/* TPML_DIGEST_VALUES */
	0x00, 0x00, 0x00, 0x01,		/* count */
	0x00, 0x04,			/* TPMT_HA.TPMI_ALG_HASH=TPM_ALG_SHA1 */
	0x01, 0x23, 0x45, 0x67, 0x89,	/* PTMT_HA.SHA1.Digest... */
	0x01, 0x23, 0x45, 0x67, 0x89,
	0x01, 0x23, 0x45, 0x67, 0x89,
	0x01, 0x23, 0x45, 0x67, 0x89
};

#define HIERARCHYCHANGEAUTH_RHHIERARCHY 10

uint8_t gcaTpm2HierarchyChangeAuth_INITIAL_DEFAULTOWNERPASS[0x31] = {
	0x80, 0x02,			/* TPM_ST_SESSIONS */
	0x00, 0x00, 0x00, 0x31,		/* Size */
	0x00, 0x00, 0x01, 0x29,		/* CommandCode: TPM_CC_HierarchyChangeAuth */
	/* TPMI_RH_HIERARCHY_AUTH */
	0x40, 0x00, 0x00, 0x01,		/* TPM_RH_OWNER */
	// Size of Auth Structure
	0x00, 0x00, 0x00, 0x09,		/* TPMS_AUTH_COMMAND Structure size */
	/* TPMS_AUTH_COMMAND */
	0x40, 0x00, 0x00, 0x09,		/* TPMI_SH_AUTH_SESSION=>TPM_RS_PW */
	0x00, 0x00,			/* A empty TPM2B_NONCE */
	0x01,				/* continue the TPMA_SESSION, attribute */
	0x00, 0x00,			/* TPM2B_AUTH: A Null Authorisation */
	/* TPM2B_AUTH New */
	0x00, 0x14,			/* Size */
	0x01, 0x23, 0x45, 0x67, 0x89,	/* Password */
	0x00, 0x00, 0x45, 0x67, 0x89,
	0x01, 0x23, 0x45, 0x00, 0x00,
	0x01, 0x23, 0x45, 0x67, 0x89
};

uint8_t gcaTpm2HierarchyChangeAuth_ANYTIME [0x45] = {
	0x80, 0x02,			/* TPM_ST_SESSIONS */
	0x00, 0x00, 0x00, 0x45,		/* Size */
	0x00, 0x00, 0x01, 0x29,		/* CommandCode: TPM_CC_HierarchyChangeAuth */
	/* TPMI_RH_HIERARCHY_AUTH */
	0x40, 0x00, 0x00, 0x01,		/* TPM_RH_OWNER */
	// Size of Auth Structure
	0x00, 0x00, 0x00, 0x1d,		/* TPMS_AUTH_COMMAND Structure size */
	/* TPMS_AUTH_COMMAND */
	0x40, 0x00, 0x00, 0x09,		/* TPMI_SH_AUTH_SESSION=>TPM_RS_PW */
	0x00, 0x00,			/* A empty TPM2B_NONCE */
	0x01,				/* continue the TPMA_SESSION, attribute */
	0x00, 0x14,			/* TPM2B_AUTH: The old Password */
	0x01, 0x23, 0x45, 0x67, 0x89,	/* Password */
	0x00, 0x00, 0x45, 0x67, 0x89,
	0x01, 0x23, 0x45, 0x00, 0x00,
	0x01, 0x23, 0x45, 0x67, 0x89,
	/* TPM2B_AUTH New */
	0x00, 0x14,			/* Size */
	0x01, 0x23, 0x45, 0x67, 0x89,	/* Password */
	0x00, 0x00, 0x45, 0x67, 0x89,
	0x01, 0x23, 0x45, 0x00, 0x00,
	0x01, 0x23, 0x45, 0x67, 0x89
};

uint8_t gcaTpm2Clear [0x2f] = {
	0x80, 0x02,			/* TPM_ST_SESSIONS */
	0x00, 0x00, 0x00, 0x2f,		/* Size */
	0x00, 0x00, 0x01, 0x26,		/* CommandCode: TPM_CC_Clear */
	/* TPMI_RH_CLEAR */
	0x40, 0x00, 0x00, 0x0C,		/* TPM_RH_PLATFORM */
	// Size of Auth Structure
	0x00, 0x00, 0x00, 0x1d,		/* TPMS_AUTH_COMMAND Structure size */
	/* TPMS_AUTH_COMMAND */
	0x40, 0x00, 0x00, 0x09,		/* TPMI_SH_AUTH_SESSION=>TPM_RS_PW */
	0x00, 0x00,			/* A empty TPM2B_NONCE */
	0x01,				/* continue the TPMA_SESSION, attribute */
	0x00, 0x14,			/* TPM2B_AUTH: The old Password */
	0x01, 0x23, 0x45, 0x67, 0x89,	/* Password */
	0x00, 0x00, 0x45, 0x67, 0x89,
	0x01, 0x23, 0x45, 0x00, 0x00,
	0x01, 0x23, 0x45, 0x67, 0x89,
};


uint8_t gcaTpmResponse[4096];
int gbDebug_TpmCommandDumpFullResponse = 0;

uint64_t gCurRunTime, gTotalRunTime;

struct domain domainsALL[4] = { {TPM_RH_PLATFORM, "Platform" }, {TPM_RH_OWNER, "Owner" },
				{TPM_RH_ENDORSEMENT, "Endorsement"}, {TPM_RH_LOCKOUT, "LockOut"} };
struct domain domainsALWAYS[1] = { {TPM_RH_PLATFORM, "Platform" } };
uint32_t domainsHandle[4] = { TPM_RH_OWNER, TPM_RH_LOCKOUT, TPM_RH_ENDORSEMENT, TPM_RH_PLATFORM };
char* domainsName[4] = { "Owner", "Lockout", "Endorsement", "Platform" };

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
		printk(KERN_ALERT "ERR:Mugambo Kush allallo Nai hai hoo haa");
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

void tpm_readclock(void)
{
	int i, iGot;

	iGot = tpm_command(0, gcaTpm2ReadClock, sizeof(gcaTpm2ReadClock),
			gcaTpmResponse, sizeof(gcaTpmResponse), "Tpm2ReadClock");
	gCurRunTime = be64_to_cpup((__be64*)&gcaTpmResponse[10]);
	printk("\tCurRuntime(msec):0x%.16llx, (mins) %lld\n", gCurRunTime, gCurRunTime/(1000*60));
	gTotalRunTime = be64_to_cpup((__be64*)&gcaTpmResponse[18]);
	printk("\tTotalRunTime(msec):0x%.16llx, (mins) %lld\n", gTotalRunTime, gTotalRunTime/(1000*60));
	printk("\tResetCount:0x%x\n", be32_to_cpup((__be32*)&gcaTpmResponse[26]));
	printk("\tRestartCount:0x%x\n", be32_to_cpup((__be32*)&gcaTpmResponse[30]));
	printk("\tSafe:0x%x\n", gcaTpmResponse[34]);

	for(i = 10; i < iGot; i+=4) {
		printk("0x%.8x\n", be32_to_cpup((__be32*)&gcaTpmResponse[i]));
	}

	if (iGot != 35) {
		printk(KERN_ALERT "Seems odd, readclock returned more data ???");
	}
}

int tpm_getcap(uint8_t *inBuf, int inLen, uint8_t *outBuf, int outLen, char *msg)
{
	int i, iGot;

	iGot = tpm_command(0, inBuf, inLen, outBuf, outLen, msg);
	printk("moreData:0x%x\n", gcaTpmResponse[10]);
	for(i = 11; i < iGot; i+=8) {
		printk("0x%.8x : 0x%.8x\n", be32_to_cpup((__be32*)&gcaTpmResponse[i]), be32_to_cpup((__be32*)&gcaTpmResponse[i+4]));
	}
	return iGot;
}

void tpm_getcap_ptfixed(void)
{
	int iGot;
	iGot = tpm_getcap(gcaGetCap_TPM_PT_FIXED, sizeof(gcaGetCap_TPM_PT_FIXED),
			gcaTpmResponse, sizeof(gcaTpmResponse), "GetCap_PTFIXED All");
}

void tpm_getcap_lockoutplus(void)
{
	int iGot;
	iGot = tpm_getcap(gcaGetCap_TPM_PT_LOCKOUTPLUS, sizeof(gcaGetCap_TPM_PT_LOCKOUTPLUS),
			gcaTpmResponse, sizeof(gcaTpmResponse), "GetCap_PTLockOutPlus");
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

int tpm_print_tpml_pcr_selection(uint8_t *buf, int iPos)
{
	int i, j;
	int iNumOfPcrSelections, iSizeOfSelect;

	iNumOfPcrSelections = be32_to_cpup((__be32*)&buf[iPos]);
	iPos += 4;
	printk("NumOfPcrSelections in Response [0x%.8x]\n", iNumOfPcrSelections);
	for(i = 0; i < iNumOfPcrSelections; i++) {
		printk("PCRSelection[%d] HashAlgo is [0x%.4x]", i, be16_to_cpup((__be16*)&buf[iPos]));
		iPos += 2;
		iSizeOfSelect = buf[iPos];
		printk("PCRSelection[%d] sizeOfSelect is [0x%.2x]\n", i, iSizeOfSelect);
		iPos += 1;
		for(j = 0; j < iSizeOfSelect; j++) {
			printk(KERN_CONT "0x%.2x ", buf[iPos]);
			iPos += 1;
		}
		printk(KERN_CONT "\n");
	}
	return iPos;
}

int tpm_print_tpml_digest(uint8_t *buf, int iPos)
{
	int i,j;
	int iNumOfDigests, iSizeOfDigest;

	iNumOfDigests = be32_to_cpup((__be32*)&buf[iPos]);
	iPos += 4;
	printk("NumOfDigests in Response [0x%.8x]\n", iNumOfDigests);
	for(i = 0; i < iNumOfDigests; i++) {
		iSizeOfDigest = be16_to_cpup((__be16*)&buf[iPos]);
		iPos += 2;
		printk("Digest[%d] of size [0x%.2x]\n", i, iSizeOfDigest);
		for(j = 0; j < iSizeOfDigest; j++) {
			printk(KERN_CONT "0x%.2x ", buf[iPos]);
			iPos += 1;
		}
		printk(KERN_CONT "\n");
	}
	return iPos;
}

void tpm_pcr_read(void)
{
	int i, iGot;
	int iPos;

	iGot = tpm_command(0, gcaTpmPCRRead_SHA1, sizeof(gcaTpmPCRRead_SHA1),
			gcaTpmResponse, sizeof(gcaTpmResponse), "TpmPCRRead SHA1");
	printk("pcrUpdateCounter:0x%.8x\n", be32_to_cpup((__be32*)&gcaTpmResponse[10]));
	// TPML_PCR_SELECTION
	iPos = tpm_print_tpml_pcr_selection(gcaTpmResponse, 14);
	// TPML_DIGEST
	iPos = tpm_print_tpml_digest(gcaTpmResponse, iPos);
	// print anything else still left in the response buffer: There shouldnt be but just in case...
	for(i = iPos; i < iGot; i+=1) {
		printk("DEBUG:OverFlow: 0x%.8x : 0x%.2x\n", i, gcaTpmResponse[i]);
	}
}

void tpm_pcr_read_all(void)
{
	int i;
	int iSize = sizeof(gcaTpmPCRRead_SHA1);
	for(i = 0; i < 3; i++) {
		gcaTpmPCRRead_SHA1[iSize-1] = 0x0;
		gcaTpmPCRRead_SHA1[iSize-2] = 0x0;
		gcaTpmPCRRead_SHA1[iSize-3] = 0x0;
		gcaTpmPCRRead_SHA1[iSize-1-i] = 0xff;
		tpm_pcr_read();
	}
}

int tpm_print_tpm2b(uint8_t *buf, int iPos, char *msg)
{
	int iSize, i;

	iSize = be16_to_cpup((__be16*)&buf[iPos]);
	iPos += 2;
	printk("%s: Size [%d]\n", msg, iSize);
	printk("%s: Data [ ", msg);
	for(i = 0; i < iSize; i++) {
		printk(KERN_CONT "0x%.2x, ", buf[iPos]);
		iPos += 1;
	}
	printk(KERN_CONT "]\n");
	return iPos;
}

int tpm_print_tpms_auth_response(uint8_t *buf, int iPos)
{
	printk("TPMS_AUTH_RESPONSE Decode...");
	// TPM2B_NONCE
	iPos = tpm_print_tpm2b(buf, iPos, "\tTPM2B_NONCE");
	// TPMA_SESSION
	printk("\tTPMA_SESSION [0x%.2x]\n", buf[iPos]);
	iPos += 1;
	// TPM2B_AUTH
	iPos = tpm_print_tpm2b(buf, iPos, "\tTPM2B_AUTH");
	return iPos;
}

void tpm_pcr_extend(void)
{
	int i, iGot, iPos;
	uint32_t iParamSize;

	iGot = tpm_command(0, gcaTpmPCRExtend_SHA1, sizeof(gcaTpmPCRExtend_SHA1),
			gcaTpmResponse, sizeof(gcaTpmResponse), "TpmPCRExtend SHA1");
	// print anything in the response beyond the header, which should include the response Authorisation structure
	iParamSize = be32_to_cpup((__be32*)&gcaTpmResponse[10]);
	printk("ParamSize: %d\n", iParamSize);
	iPos = tpm_print_tpms_auth_response(gcaTpmResponse, 14);
	for(i = iPos; i < iGot; i+=1) {
		printk("DEBUG:OverFlow: 0x%.8x : 0x%.2x\n", i, gcaTpmResponse[i]);
	}
}

void tpm_hierarchy_changeauth(uint8_t *cmdBuf, int len, char *msgCmdType, struct domain *domains, int numOfDomains)
{
	int i, j, iGot, iPos;
	uint32_t iParamSize;
	char theMsg[64];

	for (j = 0; j < numOfDomains; j++) {
		strncpy(theMsg, "Tpm2HierarchyChangeAuth", 64);
		strncat(theMsg, msgCmdType, 64);
		strncat(theMsg, domains[j].name, 64);
		*((uint32_t*)&cmdBuf[HIERARCHYCHANGEAUTH_RHHIERARCHY]) = cpu_to_be32(domains[j].handle);
		iGot = tpm_command(0, cmdBuf, len,
				gcaTpmResponse, sizeof(gcaTpmResponse), theMsg);
		if (iGot == 10) {
			printk(KERN_ALERT "HierarchyChangeAuth seems to have failed\n");
			continue;
		}
		// print anything in the response beyond the header, which should include the response Authorisation structure
		iParamSize = be32_to_cpup((__be32*)&gcaTpmResponse[10]);
		printk("ParamSize: %d\n", iParamSize);
		iPos = tpm_print_tpms_auth_response(gcaTpmResponse, 14);
		for(i = iPos; i < iGot; i+=1) {
			printk("DEBUG:OverFlow: 0x%.8x : 0x%.2x\n", i, gcaTpmResponse[i]);
		}
	}
}

#undef FIRSTTIME_RUN

void tpm_lib_dump_info(void)
{
	struct domain *domains;
	int numOfDomains;

	tpm_startup();
	tpm_getcap_lockoutplus();
	tpm_get_capabilities();
	tpm_getcap_ptfixed();
	tpm_readclock();
	tpm_pcr_read_all();
	tpm_pcr_extend();
	tpm_pcr_read_all();
	if (gbMITpmDoClear) {
		tpm_command(0, gcaTpm2Clear, sizeof(gcaTpm2Clear), gcaTpmResponse, sizeof(gcaTpmResponse), "Tpm2Clear_Platform");
	}
	if (gbMITpmDoInitAllAuths) {
		domains = domainsALL;
		numOfDomains = 4;
	} else {
		domains = domainsALWAYS;
		numOfDomains = 1;
	}
	tpm_hierarchy_changeauth(gcaTpm2HierarchyChangeAuth_INITIAL_DEFAULTOWNERPASS,
					sizeof(gcaTpm2HierarchyChangeAuth_INITIAL_DEFAULTOWNERPASS),
					"_INITIAL_", domains, numOfDomains);
	tpm_hierarchy_changeauth(gcaTpm2HierarchyChangeAuth_ANYTIME,
					sizeof(gcaTpm2HierarchyChangeAuth_ANYTIME),
					"_ANYTIME_", domainsALL, 4);
	tpm_getcap_lockoutplus();
}

