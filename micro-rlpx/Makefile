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
dirs+=${INCDIR}/${BRAND}
dirs+=${LIBDIR}
dirs+=${BINDIR}

# get objects
obj-y+=./obj/urlpx.o
obj-y+=../micro-board/linux-emu/obj/mtm_net.o

# headers
hdrs+=${INCDIR}/${BRAND}/urlpx.h
hdrs+=${INCDIR}/${BRAND}/urlpx_config.h
hdrs+=${INCDIR}/${BRAND}/urlpx_config_linux_emu.h

#targets

all: ${LIBDIR}/liburlpx.so ${LIBDIR}/liburlpx.a

include ../common.mk

#${LIBDIR}/liburlpx.a: ${hdrs}
#${LIBDIR}/liburlpx.so: ${hdrs}

.PHONY: clean

clean:
	@echo "CLEAN"
	@rm -rf ./obj/ ${LIBDIR}/liburlpx.so ${LIBDIR}/liburlpx.a
	@rm -f ${hdrs} #careful when headers is an empty /

#
#
#
