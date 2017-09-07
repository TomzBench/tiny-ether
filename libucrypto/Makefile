#crypto makefile

PREFIX:=../target
LIBDIR:=${PREFIX}/lib
INCDIR:=${PREFIX}/include
BINDIR:=${PREFIX}/bin
SRCDIR:=./

incs+=-I.
incs+=-I${SRCDIR}
incs+=-I../external/mbedtls/include
incs+=-I../board/linux-emu

#create dirs
dirs+=./obj

#build objects
obj-y+=./obj/dh.o
obj-y+=./obj/mpi.o

relobj-y:=${obj-y:.o=.lo}

#targets
all: ${dirs} ${obj-y} ${relobj-y}

include ../common.mk

clean:
	@echo "CLEAN"
	@rm -rf ./obj/ 
	@rm -f ${hdrs} #careful when headers is an empty /
