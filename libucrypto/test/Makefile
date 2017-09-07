#crypto test makefile

#INCDIR:=../
SRCDIR:=./

incs+=-I../mbedtls
incs+=-I./

libs+=-L../../external/mbedtls/library -l:libmbedcrypto.a

# create some dirs
dirs+=./obj

# compiled objects
obj-y+=./obj/test.o

# linker (external) dependencies
ld-obj+=../mbedtls/obj/dh.o
ld-obj+=../mbedtls/obj/mpi.o

# note - common.mk target requires app_*
app: ${dirs} ${obj-y} board app_test

board: ${ld-obj} 
	$(MAKE) -s -C ../mbedtls

include ../../common.mk

clean:
	@echo "CLEAN"
	@rm -rf ./obj/ 
	@rm -f ${hdrs} #careful when headers is an empty /
