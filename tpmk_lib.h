/*
 * tpmk_lib.c
 * HanishKVC, 2018
 */
#ifndef _TPMK_LIB_H_
#define _TPMK_LIB_H_

void tpm_command(int locality, uint8_t *buf, int size, char *msg);
void tpm_startup(void);
void tpm_getcap_ptfixed(void);
void tpm_get_capabilities(void);
void tpm_pcr_read(void);
void tpm_lib_dump_info(void);

#endif
