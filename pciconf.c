/*
 * Read PCI Config space v20141121
 * Repurposed for kernel driver
 * v20180106
 * HanishKVC
 */

#include "pciconf.h"

struct resource *pResPCIConf;

int pciconf_init(void)
{
	pResPCIConf = request_region(PCIIOCAM_CONFIGADDR, 0x8, HELPERMODULE_NAME);
	if (pResPCIConf == NULL) {
		printk(KERN_ALERT HELPERMODULE_NAME "pciconf iocam: Failed to get access");
		return -1;
	}
	printk(KERN_INFO HELPERMODULE_NAME "pciconf iocam: got access");
	return 0;
}

uint32_t pciconf_read32(uint32_t bus, uint32_t dev, uint32_t func, uint32_t offset)
{
	uint32_t baseAddr, addr;
	baseAddr = ((bus & 0xFF) << 16) | ((dev & 0x1F) << 11) | ((func & 0x06) << 8);
	baseAddr |= ENABLE_PCI_CONFIGSPACE_MAPPING;
	addr = baseAddr + offset;
	outl_p(addr, PCIIOCAM_CONFIGADDR);
	return inl_p(PCIIOCAM_CONFIGDATA);
}

void pciconf_release(void)
{
	release_region(PCIIOCAM_CONFIGADDR, 0x8);
}

