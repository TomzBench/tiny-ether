#crypto test makefile

APPLICATION:=libucrypto-test
BOARD:=linux-emu
BASE:=../..

USEMODULE+=libucrypto/mbedtls/uaes
USEMODULE+=libucrypto/secp256k1/uecc
USEMODULE+=libucrypto/secp256k1/uhash

include ${BASE}/Makefile.include

print:
	@echo \
		DIRS: $(DIRS) \
		SRCS: $(SRCS) \
		OBJS: $(OBJS) \
		| sed -e 's/\s\+/\n/g' 
