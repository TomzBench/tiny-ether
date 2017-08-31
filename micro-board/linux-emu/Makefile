#mtm_net makefile

PREFIX:=../build
LIBDIR:=${PREFIX}/lib
INCDIR:=${PREFIX}/include
BINDIR:=${PREFIX}/bin
SRCDIR:=./

incs+=-I.
incs+=-I${SRCDIR}

#create dirs
dirs+=./obj

#build objects
obj-y+=./obj/mtm_net.o

#copy headers
hdrs+=${INCDIR}/mtm/mtm_net.h

#targets
all: ${dirs} ${obj-y}

include ../../common.mk

clean:
	@echo "CLEAN"
	@rm -rf ./obj/ 
	@rm -f ${hdrs} #careful when headers is an empty /

