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
incs+=-I../board/linux-emu/
incs+=-I.
incs+=-I./src

# compile objects
obj-y+=./obj/rlpx_session.o

# linker (external) dependencies
ld-obj+=../board/linux-emu/obj/board_net.o
ld-relobj+=../board/linux-emu/obj/board_net.lo

relobj-y:=${obj-y:.o=.lo}

# headers
hdrs+=${INCDIR}/mtm/rlpx.h
hdrs+=${INCDIR}/mtm/rlpx_types.h
hdrs+=${INCDIR}/mtm/rlpx_config.h
hdrs+=${INCDIR}/mtm/rlpx_config_linux_emu.h

# targets
all: \
	board \
	${dirs} \
	${obj-y} \
	${relobj-y} \
	${LIBDIR}/libup2p.so \
	${LIBDIR}/libup2p.a \
	${hdrs} 

board: ${ld-obj} ${ld-relobj}
	$(MAKE) -s -C ../board/linux-emu
	
include ../common.mk

.PHONY: clean

clean:
	@echo "CLEAN"
	@rm -rf ./obj/ ${LIBDIR}/libup2p.so ${LIBDIR}/libup2p.a
	@rm -f ${hdrs} #careful when headers is an empty /

#
#
#
