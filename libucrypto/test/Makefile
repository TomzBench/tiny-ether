#crypto test makefile

SRCDIR:=./

incs+=-I./
incs+=-I../
incs+=-I../../external/secp256k1/include
incs+=-I../../target/include/
incs+=-I../../target/include/mtm

alibs+=-L../../target/lib -l:libucrypto.a -l:libsecp256k1.la
#alibs+=-L../../target/lib -l:libucrypto-trezor.a

# create some dirs
dirs+=./obj

# compiled objects
obj-y+=./obj/test.o

# note - common.mk target requires app_*
app: ${dirs} ${obj-y} app_test

include ../../common.mk

clean:
	@echo "CLEAN"
	@rm -rf ./obj/ 
	@rm -f app_test
