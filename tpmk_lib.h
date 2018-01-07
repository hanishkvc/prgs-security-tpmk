/*
 * tpmk_lib.c
 * HanishKVC, 2018
 */
#ifndef _TPMK_LIB_H_
#define _TPMK_LIB_H_

void tpm_command(int locality, uint8_t *buf, int size, char *msg);
void tpm_startup(void);
void tpm_get_capabilities(void);

#endif

