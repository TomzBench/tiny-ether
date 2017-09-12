# common.mk

BRAND:=mtm
MKDIR_P:= mkdir -p

CFLAGS?=-O0 -g -Wall -DNDEBUG
CFLAGS+= ${defines}
CFLAGS+=${cflags}
LDFLAGS+=${libs}

CC:=cc -std=gnu11 
CC+= ${incs}

relobj-y:=${obj-y:.o=.lo}

${dirs}:
	${MKDIR_P} ${dirs}

${LIBDIR}/%.so: ${dirs} ${relobj-y} ${obj-y} ${hdrs}
	@echo "LINK $@"
	@${CC} -shared ${relobj-y} ${ld-relobj} ${LDFLAGS} -o $@

${LIBDIR}/%.a: ${dirs} ${obj-y} ${hdrs} ${ld-obj}
	@echo "LINK $@"
	@ar rcs $@ ${obj-y} ${ld-obj}

${INCDIR}/${BRAND}/%.h: ${SRCDIR}/%.h
	@echo "COPY $@"
	@cp $< $@

./obj/%.lo: ${SRCDIR}/%.c
	@echo "  CC $@"
	@${CC} -c -fPIC ${CFLAGS} ${LDFLAGS} $< -o $@ 

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
	@${CC} ${ld-obj} ${obj-y} ${LDFLAGS} -o $@



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
