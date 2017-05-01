LIST=src/multiboot.lst
CSRCS= boot.c ebda.c ebs.c elf.c main.c payload.c util.c framebuffer.c pci.c serial.c 
HSRCS= project.h

CPROTO=cproto

BUILD_DIR=build
DEPDIR=${BUILD_DIR}/.d

$(shell mkdir -p ${BUILD_DIR})
$(shell mkdir -p ${DEPDIR})

DEPFLAGS        = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

ifeq ("${OBJCOPY}","")
OBJCOPY=objcopy
endif

ifeq ("${STRIP}","")
STRIP=strip
endif

ifeq ("${EFI_PREFIX}","")
EFI_PREFIX=/usr
endif
EFI_ARCH=x86_64
EFI_INC          = ${EFI_PREFIX}/include/efi
EFI_CRT_OBJS    = $(EFI_LIB)/crt0-efi-$(EFI_ARCH).o
EFI_LDS         = $(EFI_LIB)/elf_$(EFI_ARCH)_efi.lds
GCC_ARCH         = -m64
ifeq ("${EFI_LIB}","")
EFI_LIB          = ${EFI_PREFIX}/lib64/gnuefi
endif


CPPFLAGS        = -I${BUILD_DIR} -I$(EFI_INC) -I$(EFI_INC)/$(EFI_ARCH) -I$(EFI_INC)/protocol \
		 -DEFI_FUNCTION_WRAPPER

CFLAGS          =  ${DEPFLAGS} ${GCC_ARCH} ${OPT} -fno-stack-protector -fpic \
                  -fshort-wchar -mno-red-zone -Wall -Wall -fno-strict-aliasing \
                  -fno-merge-constants -Wall -Wno-unused -Werror -g \
                  -fno-omit-frame-pointer

GCCLIB          =  $(shell $(CC) ${CFLAGS} ${CPPFLAGS} -print-libgcc-file-name)

LDFLAGS         = -nostdlib -znocombreloc -T $(EFI_LDS) -shared \
                  -Bsymbolic -L $(EFI_LIB) -L $(LIB)

PAYLOAD_LIB=${BUILD_DIR}/payload.a

LIBS            = -lefi -lgnuefi $(GCCLIB) ${PAYLOAD_LIB}


OBJS            = ${CSRCS:%.c=${BUILD_DIR}/%.o} 

PROG		= openxt
EFIBIN=		${BUILD_DIR}/${PROG}.efi

default:${EFIBIN} 

${PAYLOAD_LIB} ${BUILD_DIR}/payload.h: ${LIST} ${wildcard src/*} process_list
	./process_list ${LIST} ${BUILD_DIR}/payload.a  ${BUILD_DIR}/payload.h

${BUILD_DIR}/payload.o: ${BUILD_DIR}/payload.h

${BUILD_DIR}/${PROG}.so: $(OBJS) ${PAYLOAD_LIB}
	ld $(LDFLAGS) ${EFI_CRT_OBJS} $(OBJS) -o $@ ${LIBS}  
	nm $@ | grep -i " U " && /bin/rm -f $@ || true

%.efi: %.so
	${OBJCOPY} -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc \
		--target=efi-app-$(EFI_ARCH) $^ $@
	${STRIP} $@

${BUILD_DIR}/%.o:%.c
	${CC} ${CFLAGS} ${CPPFLAGS} -c -o $@ $<
	mv -f ${DEPDIR}/$*.Td ${DEPDIR}/$*.d

tidy:
	#indent -i2 -ts0 ${CSRCS}
	astyle -A3 -s2 --attach-extern-c -L -c -w -Y -m0 -f -p  -H -U -k3 -xj -xd ${CSRCS} ${HSRCS}

protos:  ${BUILD_DIR}/payload.h
	echo -n > prototypes.h
	${CPROTO} ${CPPFLAGS} -v -e ${CSRCS} > prototypes.h.tmp
	mv prototypes.h.tmp prototypes.h


-include $(patsubst %,$(DEPDIR)/%.d,$(basename $(CSRCS)))

clean:
	/bin/rm -rf build


${BUILD_DIR}/efi.img: ${EFIBIN}
	ASZ=`stat -c %s $<` ; ASZ=$$[ $$ASZ + 1048576 ]; SZ=1048576; while [ $$SZ -lt $$ASZ ]; do SZ=$$[ $$SZ + $$SZ ]; done; touch $@ ; truncate -s  $$SZ $@ || /bin/rm -f $@
	mkdosfs $@ || /bin/rm -f $@
	mmd -i $@ ::/EFI || /bin/rm -f $@
	mmd -i $@ ::/EFI/BOOT || /bin/rm -f $@
	mcopy -i $@ $< ::/EFI/BOOT/BOOTX64.EFI || /bin/rm -f $@

${BUILD_DIR}/bios.img: 
	touch $@
	truncate -s 8192  $@

${BUILD_DIR}/iso.stamp: ${BUILD_DIR}/efi.img ${BUILD_DIR}/bios.img
	rm -rf ${BUILD_DIR}/iso
	mkdir -p ${BUILD_DIR}/iso
	cp ${BUILD_DIR}/efi.img ${BUILD_DIR}/iso/efi.img
	cp ${BUILD_DIR}/bios.img ${BUILD_DIR}/iso/bios.img
	touch $@

${BUILD_DIR}/boot.iso: ${BUILD_DIR}/iso.stamp
	xorriso -as mkisofs -iso-level 3 -full-iso9660-filenames -volid ATTOXEN \
		-eltorito-boot bios.img  -no-emul-boot  -boot-load-size 8 \
		-boot-info-table -eltorito-alt-boot -e efi.img -no-emul-boot -isohybrid-gpt-basdat \
		-isohybrid-mbr ${BUILD_DIR}/iso/bios.img  --output $@ ${BUILD_DIR}/iso







${BUILD_DIR}/boot.iso: ${BUILD_DIR}/efi.img

