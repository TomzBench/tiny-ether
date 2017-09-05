# devp2p makefile

PREFIX:=../target
LIBDIR:=${PREFIX}/lib
INCDIR:=${PREFIX}/include
BINDIR:=${PREFIX}/bin
SRCDIR:=./src

# create dirs
dirs+=./obj
dirs+=${INCDIR}/mtm
dirs+=${LIBDIR}
dirs+=${BINDIR}

# preprocessor
defines+=-DURLPX_CONFIG_LINUX_EMU
defines+=-DURLP_CONFIG_LINUX_EMU

# source directories
incs+=-I../target/include/
incs+=-I.
incs+=-I./src
incs+=-I../micro-board/linux-emu/

# compile objects
obj-y+=./obj/rlpx.o

# linker (external) dependencies
ld-obj+=../micro-board/linux-emu/obj/board_net.o
ld-relobj+=../micro-board/linux-emu/obj/board_net.lo

relobj-y:=${obj-y:.o=.lo}

# headers
hdrs+=${INCDIR}/mtm/rlpx.h
hdrs+=${INCDIR}/mtm/rlpx_config.h
hdrs+=${INCDIR}/mtm/rlpx_config_linux_emu.h

# targets
all: \
	board \
	${dirs} \
	${obj-y} \
	${relobj-y} \
	${LIBDIR}/librlpx.so \
	${LIBDIR}/librlpx.a \
	${hdrs} 

board: ${ld-obj} ${ld-relobj}
	$(MAKE) -s -C ../micro-board/linux-emu
	
include ../common.mk

.PHONY: clean

clean:
	@echo "CLEAN"
	@rm -rf ./obj/ ${LIBDIR}/librlpx.so ${LIBDIR}/librlpx.a
	@rm -f ${hdrs} #careful when headers is an empty /

#
#
#
