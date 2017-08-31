#test makefile

# tell common.mk flags and file locations
INCDIR:=../../build/include/
SRCDIR:=./

defines+=-DURLP_CONFIG_LINUX_EMU
cflags+=-Wno-pointer-sign

incs+=-I../../build/include
incs+=-I./

libs+=-L../../build/lib -l:liburlp.a

# create some dirs
dirs+=./obj

# compiled objects
obj-y+=./obj/test.o

# note - common.mk build requires app_*
app: ${dirs} ${obj-y} app_test

include ../../common.mk
