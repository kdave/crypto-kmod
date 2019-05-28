KDIR ?= /lib/modules/`uname -r`/build
obj-m := blake2s.o

default:
	$(MAKE) -C $(KDIR) M=$$PWD

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean

modules_install:
	$(MAKE) -C $(KDIR) M=$$PWD modules_install
