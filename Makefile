ifneq ($(KERNELRELEASE),)

obj-m := raspi_led.o

else


KERNDIR := /usr/src/linux-headers-$(shell uname -r)
PWD := $(shell pwd)


all:
	make -C $(KERNDIR) M=$(PWD) modules

clean:
	rm -rf *.o *.ko .*.cmd *.mod.c *.symvers *.order *.mk .*.mk .tmp_versions/ 

endif