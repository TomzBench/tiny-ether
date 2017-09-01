#board_net makefile

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
obj-y+=./obj/board_net.o

relobj-y:=${obj-y:.o=.lo}

#targets
all: ${dirs} ${obj-y} ${relobj-y}

include ../../common.mk

clean:
	@echo "CLEAN"
	@rm -rf ./obj/ 
	@rm -f ${hdrs} #careful when headers is an empty /

