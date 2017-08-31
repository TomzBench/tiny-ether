# micro-rlp makefile

PREFIX:=../build
LIBDIR:=${PREFIX}/lib
INCDIR:=${PREFIX}/include
BINDIR:=${PREFIX}/bin
SRCDIR:=./src

CFLAGS?=-O0 -g -Wall -DNDEBUG -DURLP_CONFIG_LINUX_EMU 
CC:=cc -std=gnu11  -I${INCDIR} -I${SRCDIR} -I./external/klib

# create dirs
dirs+=./obj
dirs+=${INCDIR}/${BRAND}
dirs+=${LIBDIR}
dirs+=${BINDIR}

# get objects
obj-y+=./obj/urlp.o

# headers
hdrs+=${INCDIR}/mtm/urlp.h
hdrs+=${INCDIR}/mtm/urlp_config.h
hdrs+=${INCDIR}/mtm/urlp_config_linux_emu.h

#targets

all: ${LIBDIR}/liburlp.so ${LIBDIR}/liburlp.a ${hdrs}

include ../common.mk

.PHONY: clean

clean:
	@echo "CLEAN"
	@rm -rf ./obj/ ${LIBDIR}/liburlp.so ${LIBDIR}/liburlp.a
	@rm -f ${hdrs} #careful when headers is an empty /
