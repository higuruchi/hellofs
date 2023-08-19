ifndef CONFIG_HELLOFS_FS
	EXTERNAL_BUILD=y
	CONFIG_HELLOFS_FS=m
endif

obj-$(CONFIG_HELLOFS_FS) += hellofs.o
hellofs-y := inode.o super.o

list-$(CONFIG_HELLOFS_FS) += hellofs

EXTRA_CFLAGS += -I ../../include

KVER := $(shell uname -r)
KSRC := /lib/modules/$(KVER)/build
MDIR := /lib/modules/$(KVER)/kernel/fs/hellofs/
PWD := $(shell pwd)

PREINC := -I$(PWD)/../../include
EXTRA_CFLAGS += -Wall

default:
	$(MAKE) -C $(KSRC) SUBDIRS=$(PWD) BUILD_DIR=$(PWD) \
	NOSTDINC_FLAGS=$(PREINC) modules

install: unload
	install -d $(MDIR)
	install -m 644 -c $(addsuffix .ko,$(list-m)) $(MDIR)
	/sbin/depmod $(KVER) -A

uninstall: unload
	rm -rf $(addprefix $(MDIR),$(addsuffix .ko,$(list-m)))
	/sbin/depmod $(KVER) -A

load:
	insmod $(MDIR)/hellofs.ko

unload:
	@test "`uname -r`" != "$(KVER)" || \
	     if [ -n "`mount -t $(list-m)`" ]; then \
	         echo "Unmount $(list-m) first."; false; \
	     elif (/sbin/lsmod | grep -e ^$(list-m)' ') > /dev/null; then /sbin/rmmod $(list-m); \
	     	 if [ $$? != 0 ]; then echo "failed to unload $(list-m)"; false; \
	         else echo "unloaded $(list-m)"; fi \
	     fi \

clean:
	rm -f *.mod.c *.mod *.o *.ko .*.cmd .*.flags *.lst *~ .#*
	rm -f modules.order Module.symvers
	rm -rf .tmp_versions .$(obj-m:.o=)*

