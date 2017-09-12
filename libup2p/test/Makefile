#test makefile

# tell common.mk flags and file locations
INCDIR:=../../target/include/
SRCDIR:=./

defines+=-DURLP_CONFIG_LINUX_EMU
cflags+=-Wno-pointer-sign

incs+=-I../../target/include
incs+=-I./

libs+=-L../../target/lib \
			-l:libup2p.a \
			-l:libucrypto.a\
			-l:liburlp.a 

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
