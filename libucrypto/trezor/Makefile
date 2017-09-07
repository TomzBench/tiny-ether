#crypto makefile

PREFIX:=../target
LIBDIR:=${PREFIX}/lib
INCDIR:=${PREFIX}/include
BINDIR:=${PREFIX}/bin
SRCDIR:=./

incs+=-I.
incs+=-I${SRCDIR}
incs+=-I../../external/trezor-crypto
incs+=-I../../board/linux-emu

#create dirs
dirs+=./obj

#build objects
obj-y+=../../external/trezor-crypto/sha3.o
obj-y+=./obj/sha.o

relobj-y:=${obj-y:.o=.lo}

#targets
all: ${dirs} ${obj-y}

include ../../common.mk

clean:
	@echo "CLEAN"
	@rm -rf ./obj/ 
	@rm -rf ../../external/trezor-crypto/*.o
	@rm -f ${hdrs} #careful when headers is an empty /
