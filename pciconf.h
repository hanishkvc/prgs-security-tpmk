/*
 * Read PCI Config space v20141121
 * Repurposed for kernel driver
 * v20180106
 * HanishKVC
 */
#ifndef _PCICONF_H_
#define _PCICONF_H_
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <asm/io.h>

#define PRG_VERSION "v20141121"
#define HELPERMODULE_NAME "pciconfk"

#define ENABLE_PCI_CONFIGSPACE_MAPPING 0x80000000
// PCI CONFIGACCESSMETHOD (CAM) IO Indexed Addresses
#define PCIIOCAM_CONFIGADDR 0xCF8
#define PCIIOCAM_CONFIGDATA 0xCFC

int pciconf_init(void);

uint32_t pciconf_read32(uint32_t bus, uint32_t dev, uint32_t func, uint32_t offset);

void pciconf_release(void);

#endif
