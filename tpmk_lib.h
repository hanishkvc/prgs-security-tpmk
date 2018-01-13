/*
 * tpmk_lib.h
 * HanishKVC, 2018
 */
#ifndef _TPMK_LIB_H_
#define _TPMK_LIB_H_

#define TPM_RH_OWNER		0x40000001
#define TPM_RH_LOCKOUT		0x4000000A
#define TPM_RH_ENDORSEMENT	0x4000000B
#define TPM_RH_PLATFORM		0x4000000C

extern uint64_t gCurRunTime, gTotalRunTime;
extern int gbDebug_TpmCommandDumpFullCommand;
extern int gbDebug_TpmCommandDumpFullResponse;

struct domain {
	uint32_t handle;
	char *name;
};

extern struct domain domainsALL[4];
extern struct domain domainsALWAYS[1];
extern uint32_t domainsHandle[4];
extern char* domainsName[4];

int tpm_command(int locality, uint8_t *inBuf, int inSize, uint8_t *outBuf, int outSize, char *msg);
void tpm_print_command_generic(uint8_t *buf, int len, int bDumpFullCommand, char *msg);
void tpm_print_response_generic(uint8_t *outBuf, int iGot, int bDumpFullResponse, char *msg);
void tpm_startup(void);
void tpm_shutdown(void);
void tpm_readclock(void);
int tpm_getcap(uint8_t *inBuf, int inLen, uint8_t *outBuf, int outLen, char *msg);
void tpm_getcap_ptfixed(void);
void tpm_getcap_lockoutplus(void);
void tpm_get_capabilities(void);
int tpm_print_tpml_pcr_selection(uint8_t *buf, int iPos);
int tpm_print_tpml_digest(uint8_t *buf, int iPos);
void tpm_pcr_read(void);
void tpm_pcr_read_all(void);
int tpm_print_tpm2b(uint8_t *buf, int iPos, char *msg);
int tpm_print_tpms_auth_response(uint8_t *buf, int iPos);
void tpm_pcr_extend(void);
void tpm_hierarchy_changeauth(uint8_t *cmdBuf, int len, char *msgCmdType, struct domain *domains, int numOfDomains);
void tpm_lib_dump_info(void);

#endif

