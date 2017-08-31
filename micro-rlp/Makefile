# libatx makefile

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
relobj-y:=${obj-y:.o=.lo}

# headers
hdrs+=${INCDIR}/${BRAND}/urlp.h
hdrs+=${INCDIR}/${BRAND}/urlp_config.h
hdrs+=${INCDIR}/${BRAND}/urlp_config_linux_emu.h

#targets

all: ${LIBDIR}/liburlp.so ${LIBDIR}/liburlp.a

include ../common.mk

${LIBDIR}/liburlp.so: ${dirs} ${relobj-y} ${hdrs}
	@echo "LINK $@"
	@${CC} -shared ${relobj-y} ${LDFLAGS} -o ${LIBDIR}/liburlp.so

${LIBDIR}/liburlp.a: ${dirs} ${obj-y} ${hdrs}
	@echo "LINK $@"
	@ar rcs ${LIBDIR}/liburlp.a ${obj-y}

${INCDIR}/${BRAND}/%.h: ${SRCDIR}/%.h
	@echo "COPY $@"
	@cp $< $@

./obj/%.lo: ${SRCDIR}/%.c
	@echo "  CC $@"
	@${CC} -c -fPIC ${CFLAGS} ${LDFLAGS} $< -o $@ 

./obj/%.o: ${SRCDIR}/%.c
	@echo "  CC $@"
	@${CC} -c ${CFLAGS} ${LDFLAGS} $< -o $@ 

.PHONY: clean

clean:
	@echo "CLEAN"
	@rm -rf ./obj/ ${LIBDIR}/liburlp.so ${LIBDIR}/liburlp.a
	@rm -f ${hdrs} #careful when headers is an empty /
