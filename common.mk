# common.mk

BRAND:=mtm
MKDIR_P:= mkdir -p

CFLAGS?=-O0 -g -Wall  -DNDEBUG -std=gnu11
CFLAGS+=${defines}
CFLAGS+=${incs}
CFLAGS+=${cflags}
#LDFLAGS+=${libs}

CC:=cc

relobj-y:=${obj-y:.o=.lo}

${dirs}:
	${MKDIR_P} ${dirs}

${LIBDIR}/%.a: ${dirs} ${obj-y} ${hdrs} ${ld-obj}
	@echo "LINK $@"
	@ar rcs $@ ${obj-y} ${ld-obj} 

${INCDIR}/${BRAND}/%.h: ${SRCDIR}/%.h
	@echo "COPY $@"
	@cp $< $@

./obj/%.o: ${SRCDIR}/%.c
	@echo "  CC $@"
	@${CC} -c ${CFLAGS} ${LDFLAGS} $< -o $@ 

%.o: %.c
	@echo "  CC $@"
	@${CC} -c ${CFLAGS} ${LDFLAGS} $< -o $@ 

%.lo: %.c
	@echo "  CC $@"
	@${CC} -c -fPIC ${CFLAGS} ${LDFLAGS} $< -o $@ 

app_%: ${dirs} ${obj-y}
	@echo "LINK $@"
	@${CC} ${ld-obj} ${obj-y} ${LDFLAGS} ${alibs} -o $@



valgrind_%: %
	valgrind \
		--track-origins=yes \
		--tool=memcheck \
		--leak-check=full \
		--quiet \
		./$<

clean_%: 
	@echo "CLEAN"
	@rm -rf ./${obj-y} ./${relobj-y} $(patsubst clean_%,%,$@)

#
#
#
