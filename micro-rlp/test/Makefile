#test makefile

INCDIR:=../../build/include/
CFLAGS?=-O0 -g -Wall -Wno-pointer-sign -DNDEBUG -DURLP_CONFIG_LINUX_EMU 
LDFLAGS=-L../../build/lib -l:liburlp.a
CC:=cc -std=gnu11  -I${INCDIR} -I. -I../../build/include

dirs+=./obj

obj-y+=./obj/test.o

test:	${dirs} ${obj-y}
	@echo "LINK $@"
	@${CC} ${obj-y} ${LDFLAGS} -o test

./obj/%.o: %.c
	@echo "  CC $@"
	@${CC} -c ${CFLAGS} ${LDFLAGS} $< -o $@

include ../../common.mk

.PHONY: cscope clean print
cscope:
	find . \
		\( -name '*.c' -o -name '*.h' -o -name '*.hpp' -o -name '*.cpp' \) -print > cscope.files
		cscope -b 

valgrind:
	valgrind \
		--track-origins=yes \
		--tool=memcheck \
		--leak-check=full \
		./test

clean:
	@echo "CLEAN"
	@rm -rf ./obj/ test

print:
	@echo \
		OBJECTS: ${obj-y} \
		LIB_OBJ: ${relobj-y} \
		| sed -e 's/\s\+/\n/g' 
