OBJECT_FILES_NON_STANDARD := y

KDIR ?= /lib/modules/`uname -r`/build
obj-m += blake2b-sse2-gen.o blake2b-sse41-gen.o blake2b-avx2-gen.o
obj-m += blake2b-test-gen.o

ccflags-y := -save-temps=obj

blake2b-sse2-gen-y := blake2b-nocompress.o blake2b-compress-gen-sse2.o
blake2b-sse41-gen-y := blake2b-nocompress.o blake2b-compress-gen-sse41.o
blake2b-avx2-gen-y := blake2b-nocompress.o blake2b-compress-gen-avx2.o

blake2b-test-gen-y := blake2b-nocompress.o blake2b-compress-gen-test.o

CFLAGS_blake2b-compress-gen-sse2.o += -msse2
CFLAGS_blake2b-compress-gen-sse41.o += -msse4.1
CFLAGS_blake2b-compress-gen-avx2.o += -mavx2
CFLAGS_blake2b-compress-gen-test.o += -msse4.1 -O3

all: default alls

default:
	$(MAKE) -C $(KDIR) M=$$PWD

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean

modules_install:
	$(MAKE) -C $(KDIR) M=$$PWD modules_install

stargets = blake2b-compress-sse2.S blake2b-compress-sse41.S blake2b-compress-avx2.S
stargets += blake2b-compress-test.S
alls: $(stargets)
cleans:
	rm -f $(stargets)

# needs blake2b-sse2-gen.ko
blake2b-compress-sse2.S:
	cp blake2b-compress-gen-sse2.s blake2b-compress-sse2.S
	sed -i -e '/\.loc/d' blake2b-compress-sse2.S
	sed -i -e '/\.cfi_/d' blake2b-compress-sse2.S
	sed -i -e '/\.LVL/d' blake2b-compress-sse2.S
	sed -i -e '/\.LF[BE]/d' blake2b-compress-sse2.S
	sed -i -e '/\.LB[BEI]/d' blake2b-compress-sse2.S
	sed -i -e '/^\.Letext/Q' blake2b-compress-sse2.S

blake2b-compress-sse41.S:
	cp blake2b-compress-gen-sse41.s blake2b-compress-sse41.S
	sed -i -e '/\.loc/d' blake2b-compress-sse41.S
	sed -i -e '/\.cfi_/d' blake2b-compress-sse41.S
	sed -i -e '/\.LVL/d' blake2b-compress-sse41.S
	sed -i -e '/\.LF[BE]/d' blake2b-compress-sse41.S
	sed -i -e '/\.LB[BEI]/d' blake2b-compress-sse41.S
	sed -i -e '/^\.Letext/Q' blake2b-compress-sse41.S

blake2b-compress-avx2.S:
	cp blake2b-compress-gen-avx2.s blake2b-compress-avx2.S
	sed -i -e '/\.loc/d' blake2b-compress-avx2.S
	sed -i -e '/\.cfi_/d' blake2b-compress-avx2.S
	sed -i -e '/\.LVL/d' blake2b-compress-avx2.S
	sed -i -e '/\.LF[BE]/d' blake2b-compress-avx2.S
	sed -i -e '/\.LB[BEI]/d' blake2b-compress-avx2.S
	sed -i -e '/^\.Letext/Q' blake2b-compress-avx2.S

blake2b-compress-test.S:
	cp blake2b-compress-gen-test.s blake2b-compress-test.S
	sed -i -e '/\.loc/d' blake2b-compress-test.S
	sed -i -e '/\.cfi_/d' blake2b-compress-test.S
	sed -i -e '/\.LVL/d' blake2b-compress-test.S
	sed -i -e '/\.LF[BE]/d' blake2b-compress-test.S
	sed -i -e '/\.LB[BEI]/d' blake2b-compress-test.S
	sed -i -e '/^\.Letext/Q' blake2b-compress-test.S

