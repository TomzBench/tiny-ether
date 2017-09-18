#crypto test makefile

APPLICATION:=libucrypto-test
BOARD:=linux-emu
BASE:=../..

USEMODULE+=mbedtls
USEMODULE+=secp256k1
USEMODULE+=mbedtls-uaes
USEMODULE+=secp256k1-uecc
USEMODULE+=secp256k1-uhash

include ${BASE}/Makefile.include

print:
	@echo \
		DEPS: ${DEPS} \
		INCS: ${USEMODULE_INCLUDES} \
		| sed -e 's/\s\+/\n/g' 
