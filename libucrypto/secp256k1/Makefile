PREFIX:=../../target
LIBDIR:=${PREFIX}/lib
INCDIR:=${PREFIX}/include
BINDIR:=${PREFIX}/bin
SRCDIR:=./

incs+=-I.
incs+=-I${SRCDIR}
incs+=-I../../external/secp256k1/include
incs+=-I../../external/secp256k1/src

dirs+=${INCDIR}/mtm
dirs+=./obj
obj-y+=./obj/uecc.o
obj-y+=./obj/uhash.o

alibs+=../../target/lib/libsecp256k1.la

hdrs+=${INCDIR}/mtm/uecc.h
hdrs+=${INCDIR}/mtm/uhash.h

relobj-y:=${obj-y:.o=.lo}
ld-relobj-y:=${ld-obj:.o=.lo}

#targets
all: ${LIBDIR}/libucrypto-secp256k1.a ${LIBDIR}/libucrypto-secp256k1.so

${LIBDIR}/libucrypto-secp256k1.a: ${hdrs} ${ld-obj} ${obj-y}

${LIBDIR}/libucrypto-secp256k1.so: ${hdrs} ${ld-relobj-y} ${relobj-y}

include ../../common.mk

clean:
	@echo "CLEAN"
	@rm -rf ./obj/ 
	@rm -f ${ld-obj}
	@rm -f ${ld-relobj-y}
	@rm -f ${LIBDIR}/libucrypto-secp256k1.a
	@rm -f ${LIBDIR}/libucrypto-secp256k1.la
	@rm -f ${LIBDIR}/libucrypto-secp256k1.so
	@rm -f ${hdrs} #careful when headers is an empty /
