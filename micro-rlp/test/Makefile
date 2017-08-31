#test makefile

# tell common.mk flags and file locations
INCDIR:=../../build/include/
SRCDIR:=./
CFLAGS?=-O0 -g -Wall -Wno-pointer-sign -DNDEBUG -DURLP_CONFIG_LINUX_EMU 
LDFLAGS=-L../../build/lib -l:liburlp.a
CC:=cc -std=gnu11  -I${INCDIR} -I.

# create some dirs
dirs+=./obj

# compiled objects
obj-y+=./obj/test.o

# note - common.mk build requires app_*
app: ${dirs} ${obj-y} app_test

include ../../common.mk
