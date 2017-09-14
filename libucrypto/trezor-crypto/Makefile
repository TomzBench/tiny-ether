#crypto makefile

PREFIX:=../../target
LIBDIR:=${PREFIX}/lib
INCDIR:=${PREFIX}/include
BINDIR:=${PREFIX}/bin
SRCDIR:=./

incs+=-I.
incs+=-I${SRCDIR}
incs+=-I../../external
incs+=-I../../external/trezor-crypto
incs+=-I../../external/trezor-crypto/aes
incs+=-I../../external/trezor-crypto/ed25519-donna

#create dirs
dirs+=${INCDIR}/mtm
dirs+=${INCDIR}/mtm/trezor-crypto
dirs+=${INCDIR}/mtm/trezor-crypto/aes
dirs+=${LIBDIR}
dirs+=${BINDIR}
dirs+=./obj


#build objects
obj-y+=./obj/uaes.o
obj-y+=./obj/uecc.o
obj-y+=./obj/umpi.o
obj-y+=./obj/uecies.o
obj-y+=./obj/uhmac.o
obj-y+=./obj/urand.o

#external dependencies
ld-obj+=../../external/trezor-crypto/address.o
ld-obj+=../../external/trezor-crypto/base32.o
ld-obj+=../../external/trezor-crypto/base58.o
ld-obj+=../../external/trezor-crypto/bignum.o
ld-obj+=../../external/trezor-crypto/bip32.o
ld-obj+=../../external/trezor-crypto/bip39.o
ld-obj+=../../external/trezor-crypto/blake2b.o
ld-obj+=../../external/trezor-crypto/blake2s.o
ld-obj+=../../external/trezor-crypto/curves.o
ld-obj+=../../external/trezor-crypto/ecdsa.o
ld-obj+=../../external/trezor-crypto/hmac.o
ld-obj+=../../external/trezor-crypto/nem.o
ld-obj+=../../external/trezor-crypto/nist256p1.o
ld-obj+=../../external/trezor-crypto/pbkdf2.o
ld-obj+=../../external/trezor-crypto/rand.o
ld-obj+=../../external/trezor-crypto/rc4.o
#ld-obj+=../../external/trezor-crypto/rfc6979.o
ld-obj+=../../external/trezor-crypto/ripemd160.o
ld-obj+=../../external/trezor-crypto/script.o
ld-obj+=../../external/trezor-crypto/secp256k1.o
ld-obj+=../../external/trezor-crypto/sha2.o
ld-obj+=../../external/trezor-crypto/sha3.o
ld-obj+=../../external/trezor-crypto/ed25519-donna/curve25519-donna-32bit.o
ld-obj+=../../external/trezor-crypto/ed25519-donna/curve25519-donna-helpers.o
ld-obj+=../../external/trezor-crypto/ed25519-donna/curve25519-donna-scalarmult-base.o
ld-obj+=../../external/trezor-crypto/ed25519-donna/ed25519-donna-32bit-tables.o
ld-obj+=../../external/trezor-crypto/ed25519-donna/ed25519-donna-basepoint-table.o
ld-obj+=../../external/trezor-crypto/ed25519-donna/ed25519-donna-impl-base.o
ld-obj+=../../external/trezor-crypto/ed25519-donna/ed25519-keccak.o
ld-obj+=../../external/trezor-crypto/ed25519-donna/ed25519-sha3.o
ld-obj+=../../external/trezor-crypto/ed25519-donna/ed25519.o
ld-obj+=../../external/trezor-crypto/ed25519-donna/modm-donna-32bit.o
ld-obj+=../../external/trezor-crypto/aes/aes_modes.o
ld-obj+=../../external/trezor-crypto/aes/aescrypt.o
ld-obj+=../../external/trezor-crypto/aes/aeskey.o
ld-obj+=../../external/trezor-crypto/aes/aestab.o
ld-obj+=../../external/trezor-crypto/aes/aestst.o

hdrs+=${INCDIR}/mtm/uaes.h
hdrs+=${INCDIR}/mtm/uecc.h
hdrs+=${INCDIR}/mtm/umpi.h
hdrs+=${INCDIR}/mtm/uecies.h
hdrs+=${INCDIR}/mtm/uhmac.h

relobj-y:=${obj-y:.o=.lo}
ld-relobj-y:=${ld-obj:.o=.lo}

#targets
all: ${LIBDIR}/libucrypto-trezor.a ${LIBDIR}/libucrypto-trezor.so completed

${LIBDIR}/libucrypto-trezor.a: ${hdrs} ${ld-obj} ${obj-y}

${LIBDIR}/libucrypto-trezor.so: ${hdrs} ${ld-relobj-y} ${relobj-y}

completed:
	@echo "COPY install"
	@cp ../../external/trezor-crypto/*.h ${INCDIR}/mtm/trezor-crypto
	@cp ../../external/trezor-crypto/aes/*.h ${INCDIR}/mtm/trezor-crypto/aes
	@cp ../../external/trezor-crypto/ed25519-donna/*.h ${INCDIR}/mtm/trezor-crypto


include ../../common.mk

clean:
	@echo "CLEAN"
	@rm -rf ./obj/ 
	@rm -f ${ld-obj}
	@rm -f ${ld-relobj-y}
	@rm -rf ${INCDIR}/mtm/trezor-crypto
	@rm -f ${LIBDIR}/libucrypto-trezor.a
	@rm -f ${LIBDIR}/libucrypto-trezor.so
	@rm -f ${hdrs} #careful when headers is an empty /
