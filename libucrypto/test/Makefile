#crypto test makefile

APPLICATION:=libucrypto-test
BOARD:=linux-emu
BASE:=../..

USEMODULE+=mbedtls-uaes
USEMODULE+=secp256k1-uecc
USEMODULE+=secp256k1-uhash

include ${BASE}/Makefile.include

print:
	@echo \
		DEPS: ${DEPS} \
		DIRS: $(DIRS) \
		OBJS: $(OBJS) \
		INCS: $(INCS) \
		SRCS: $(SRCS) \
		| sed -e 's/\s\+/\n/g' 
