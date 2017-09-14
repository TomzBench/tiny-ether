PREFIX:=../../target
LIBDIR:=${PREFIX}/lib
INCDIR:=${PREFIX}/include
BINDIR:=${PREFIX}/bin
SRCDIR:=./

incs+=-I.
incs+=-I${SRCDIR}
incs+=-I../../external/secp256k1/include

dirs+=${INCDIR}/mtm
dirs+=./obj
obj-y+=./obj/uecc.o

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
	@rm -f ${LIBDIR}/libucrypto-secp256k1.so
	@rm -f ${hdrs} #careful when headers is an empty /
