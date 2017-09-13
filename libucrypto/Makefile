#crypto makefile

PREFIX:=../target
LIBDIR:=${PREFIX}/lib
INCDIR:=${PREFIX}/include
BINDIR:=${PREFIX}/bin
SRCDIR:=./

incs+=-I.
incs+=-I${SRCDIR}
incs+=-I../external/mbedtls/include
#incs+=-I../external/trezor-crypto/

#create dirs
dirs+=./obj

#build objects
obj-y+=./obj/aes.o
obj-y+=./obj/ecc.o
obj-y+=./obj/mpi.o
obj-y+=./obj/ecies.o
obj-y+=./obj/hmac.o

#external dependencies
#ld-obj+=../external/trezor-crypto/sha3.o
ld-obj+=../external/mbedtls/library/aes.o
ld-obj+=../external/mbedtls/library/aesni.o
ld-obj+=../external/mbedtls/library/asn1write.o
ld-obj+=../external/mbedtls/library/asn1parse.o
ld-obj+=../external/mbedtls/library/bignum.o
ld-obj+=../external/mbedtls/library/ctr_drbg.o
ld-obj+=../external/mbedtls/library/ecdh.o
ld-obj+=../external/mbedtls/library/ecp.o
ld-obj+=../external/mbedtls/library/ecp_curves.o
ld-obj+=../external/mbedtls/library/ecdsa.o
ld-obj+=../external/mbedtls/library/ecdh.o
ld-obj+=../external/mbedtls/library/entropy.o
ld-obj+=../external/mbedtls/library/entropy_poll.o
ld-obj+=../external/mbedtls/library/hmac_drbg.o
ld-obj+=../external/mbedtls/library/md.o
ld-obj+=../external/mbedtls/library/md2.o
ld-obj+=../external/mbedtls/library/md4.o
ld-obj+=../external/mbedtls/library/md5.o
ld-obj+=../external/mbedtls/library/md_wrap.o
ld-obj+=../external/mbedtls/library/ripemd160.o
ld-obj+=../external/mbedtls/library/sha1.o
ld-obj+=../external/mbedtls/library/sha256.o
ld-obj+=../external/mbedtls/library/sha512.o
ld-obj+=../external/mbedtls/library/timing.o

hdrs+=${INCDIR}/mtm/aes.h
hdrs+=${INCDIR}/mtm/bits.h
hdrs+=${INCDIR}/mtm/ecc.h
hdrs+=${INCDIR}/mtm/mpi.h
hdrs+=${INCDIR}/mtm/ecies.h
hdrs+=${INCDIR}/mtm/hmac.h


relobj-y:=${obj-y:.o=.lo}
ld-relobj-y:=${ld-obj:.o=.lo}

#targets
all: ${LIBDIR}/libucrypto.a ${LIBDIR}/libucrypto.so

${LIBDIR}/libucrypto.a: ${hdrs} ${ld-obj} ${obj-y}
${LIBDIR}/libucrypto.so: ${hdrs} ${ld-relobj-y} ${relobj-y}

include ../common.mk

clean:
	@echo "CLEAN"
	@rm -rf ./obj/ 
	@rm -f ${ld-obj}
	@rm -f ${ld-relobj-y}
	@rm -f ${LIBDIR}/libucrypto.a
	@rm -f ${LIBDIR}/libucrypto.so
	@rm -f ${hdrs} #careful when headers is an empty /
