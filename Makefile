OBJECT_FILES_NON_STANDARD := y

KDIR ?= /lib/modules/`uname -r`/build
obj-m += blake2s.o blake2b.o
obj-m += blake2b-sse2.o blake2b-sse41.o blake2b-avx2.o

blake2b-sse2-y := blake2b-nocompress.o blake2b-compress-sse2.o
blake2b-sse41-y := blake2b-nocompress.o blake2b-compress-sse41.o
blake2b-avx2-y := blake2b-nocompress.o blake2b-compress-avx2.o

default:
	$(MAKE) -C $(KDIR) M=$$PWD

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean

modules_install:
	$(MAKE) -C $(KDIR) M=$$PWD modules_install

gen:
	$(MAKE) -C genmod default alls
	cp genmod/*.S .
