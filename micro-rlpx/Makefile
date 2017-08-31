# micro-urlpx makefile

PREFIX:=../build
LIBDIR:=${PREFIX}/lib
INCDIR:=${PREFIX}/include
BINDIR:=${PREFIX}/bin
SRCDIR:=./src

#defines+=-DURLPX_CONFIG_LINUX_EMU
#incs+=-I.
#incs+=./src
#incs+=../micro-board/linux-emu/

# create dirs
dirs+=./obj
dirs+=${INCDIR}/mtm
dirs+=${LIBDIR}
dirs+=${BINDIR}

# get objects
obj-y+=./obj/urlpx.o
obj-y+=../micro-board/linux-emu/obj/mtm_net.o

# headers
hdrs+=${INCDIR}/mtm/urlpx.h
hdrs+=${INCDIR}/mtm/urlpx_config.h
hdrs+=${INCDIR}/mtm/urlpx_config_linux_emu.h

#targets
all: ${LIBDIR}/liburlpx.so ${LIBDIR}/liburlpx.a ${hdrs}

include ../common.mk

.PHONY: clean

clean:
	@echo "CLEAN"
	@rm -rf ./obj/ ${LIBDIR}/liburlpx.so ${LIBDIR}/liburlpx.a
	@rm -f ${hdrs} #careful when headers is an empty /

#
#
#
