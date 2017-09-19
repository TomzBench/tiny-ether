#ulib makefile

# setup install directory
PREFIX		?=	target
BASE 		:= 	./
MKDIR_P		:= 	mkdir -p
CC		:=	gcc

# Configurations switches -D
CONFIGS_D 	+= 	URLP_CONFIG_LINUX_EMU
CONFIGS_D 	+= 	URLPX_CONFIG_LINUX_EMU

# Collect lib objects *.o
MODULES 	+=	liburlp
MODULES 	+=	libucrypto/mbedtls/uaes
MODULES 	+=	libucrypto/mbedtls/uecc
MODULES 	+=	libucrypto/mbedtls/uhash
MODULES 	+=	libucrypto/secp256k1/uecc
MODULES 	+=	libucrypto/secp256k1/uhash

# Build test applications
APPLICATIONS 	+=	liburlp/test
APPLICATIONS 	+=	libucrypto/test
APPLICATIONS 	+=	libup2p/test

# Build vars
DIRS 		+=	$(addprefix $(PREFIX)/lib/,$(MODULES))
DIRS 		+=	$(addprefix $(PREFIX)/obj/,$(MODULES))
DIRS 		+=	$(addprefix $(PREFIX)/obj/,$(APPLICATIONS))
LIBS 		+= 	$(addsuffix .a,$(foreach mod, $(MODULES),$(subst /,-,$(mod))))
SRCS 		+=	$(shell find $(MODULES) -maxdepth '1' -name '*.c')
OBJS		+=	$(addprefix $(PREFIX)/obj/,$(SRCS:.c=.o))
INCS		+=	$(addprefix -I./, $(MODULES))
INCS		+=	$(addsuffix /include,$(addprefix -I./, $(MODULES)))
INCS	 	+=	$(addprefix -I./,$(PREFIX)/include)
INCS 		+= 	$(addprefix -I./,$(BASE)/external/secp256k1/include)
DEFS 		+= 	$(addprefix -D,$(CONFIGS_D))
CFLAGS 		+= 	$(DEFS)

all: $(DIRS) $(OBJS)

.PHONY: dirs libs clean test print

$(PREFIX)/obj/%.o: %.c
	@echo "  CC $@"
	@${CC} -c ${CFLAGS} ${LDFLAGS} $(INCS) $< -o $@ 

$(DIRS):
	@echo "MKDR $@"
	@${MKDIR_P} ${DIRS}

clean:
	@echo "CLEAN"
	@rm -rf target

# Makefile debug print
print:
	@echo \
		DIRS: $(DIRS) \
		LIBS: $(LIBS) \
		SRCS: $(SRCS) \
		INCS: $(INCS) \
		OBJS: $(OBJS) \
		DEFS: $(DEFS) \
		OBJ_DIR: $(OBJ_DIR) \
		SRC_DIR: $(SRC_DIR) \
		test: $(test) \
		| sed -e 's/\s\+/\n/g' 

#
#
#
