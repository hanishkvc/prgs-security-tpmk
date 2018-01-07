/* TPMK - A minimal tpm driver
 * HanishKVC, 2018
 */
#ifndef _TPMK_DRIVER_H_
#define _TPMK_DRIVER_H_
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <asm/byteorder.h>
#include "pciconf.h"

#define MODULE_NAME ":tpmk:"
#define MODULE_MY_VERSION "v20180107_0203"

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

extern int gCurLocality;

#define MAXWAITCNT_REQUESTLOCALITY 20
#define MAXWAITCNT_FORCMDREADY 20
#define MAXWAITCNT_DATAAVAIL 20
#define RECV_INITIAL 10

void tpm_dump_info(void);

int tpm_wait_for(void *addr, int mask, int trueValue, int maxWaitCnt, char *msgPrefix);

int tpm_request_locality(int locality);

int sys_init(void);

int tpm_init(void);

int tpm_send(int locality, uint8_t *buf, int len);

int tpm_recv_helper(int locality, uint8_t *buf, int len);

int tpm_recv(int locality, uint8_t *buf, int len);

#endif

