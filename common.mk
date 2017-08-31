# common.mk

BRAND:=mtm
MKDIR_P:= mkdir -p

${dirs}:
	${MKDIR_P} ${dirs}

${LIBDIR}/%.so: ${dirs} ${relobj-y} ${hdrs}
	@echo "LINK $@"
	@${CC} -shared ${relobj-y} ${LDFLAGS} -o $@

${LIBDIR}/%.a: ${dirs} ${obj-y} ${hdrs}
	@echo "LINK $@"
	@ar rcs $@ ${obj-y}

${INCDIR}/${BRAND}/%.h: ${SRCDIR}/%.h
	@echo "COPY $@"
	@cp $< $@

./obj/%.lo: ${SRCDIR}/%.c
	@echo "  CC $@"
	@${CC} -c -fPIC ${CFLAGS} ${LDFLAGS} $< -o $@ 

./obj/%.o: ${SRCDIR}/%.c
	@echo "  CC $@"
	@${CC} -c ${CFLAGS} ${LDFLAGS} $< -o $@ 
