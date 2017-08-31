# micro-urlpx makefile

PREFIX:=../build
LIBDIR:=${PREFIX}/lib
INCDIR:=${PREFIX}/include
BINDIR:=${PREFIX}/bin
SRCDIR:=./src

incs+= \
       -I../micro-board/linux-emu/

CFLAGS?=-O0 -g -Wall -DNDEBUG -DURLP_CONFIG_LINUX_EMU 
CC:=cc -std=gnu11  -I${INCDIR} -I${SRCDIR} -I./external/klib

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
