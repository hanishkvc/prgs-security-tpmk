obj-m += tpmk.o

tpmk-objs := tpmk_driver.o pciconf.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

