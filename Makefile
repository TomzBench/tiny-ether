#ulib makefile

# setup install directory
TARGET		?=	target
MKDIR_P		:= 	mkdir -p
CC		:=	gcc

# Configurations switches -D
CONFIGS_D 	+= 	URLP_CONFIG_LINUX_EMU
CONFIGS_D 	+= 	URLPX_CONFIG_LINUX_EMU

# Collect lib objects *.o
MODULES 	+=	liburlp
MODULES 	+=	libucrypto/mbedtls/uaes
MODULES 	+=	libucrypto/mbedtls/uhash
MODULES 	+=	libucrypto/secp256k1/uecc
MODULES 	+=	libucrypto/secp256k1/uhash

# Build test applications
APPLICATIONS 	+=	liburlp/test
APPLICATIONS 	+=	libucrypto/test
APPLICATIONS 	+=	libup2p/test

# Build vars
DIRS 		+=	$(addprefix $(TARGET)/obj/,$(MODULES))
DIRS 		+=	$(addprefix $(TARGET)/obj/,$(APPLICATIONS))
LIBS 		+= 	$(addprefix $(TARGET)/lib/, \
			$(addsuffix .a,$(foreach mod, $(MODULES),$(subst /,-,$(mod)))))
SRCS 		+=	$(shell find $(MODULES) -maxdepth '1' -name '*.c')
OBJS		+=	$(addprefix $(TARGET)/obj/,$(SRCS:.c=.o))
INCS		+=	$(addprefix -I./, $(MODULES))
INCS		+=	$(addsuffix /include,$(addprefix -I./, $(MODULES)))
INCS	 	+=	$(addprefix -I./,$(TARGET)/include)
DEFS 		+= 	$(addprefix -D,$(CONFIGS_D))
CFLAGS 		+= 	$(DEFS)

all: $(DIRS) $(OBJS) $(LIBS)

# The name convention allows collecting lib objects with find,
# IE: $(TARGET)/lib/libucrypto-mbedtls-uaes.a:=$(TARGET)/obj/libucrypto/mbedtls/uaes/**/*/.o
$(TARGET)/lib/%.a:
	@echo "LINK $@ $(shell find \
		$(subst $(TARGET)/lib,$(TARGET)/obj,$(subst .a,,$(subst -,/,$@))) -name '*.o')"
	@ar rcs $@ $(shell find \
		$(subst $(TARGET)/lib,$(TARGET)/obj,$(subst .a,,$(subst -,/,$@))) -name '*.o')

$(TARGET)/obj/%.o: %.c
	@echo "  CC $@"
	@${CC} -c ${CFLAGS} ${LDFLAGS} $(INCS) $< -o $@ 

$(DIRS):
	@echo "MKDR $@"
	@${MKDIR_P} ${DIRS}

#${TARGET}/include/%.h: ${SRCDIR}/%.h
#	@echo "COPY $@"
#	@cp $< $@

.PHONY: dirs libs clean test print

clean:
	@echo "CLEAN"
	@rm -rf $(TARGET)/lib/libucrypto
	@rm -rf $(TARGET)/lib/liburlp
	@rm -rf $(TARGET)/lib/liburlp.a
	@rm -rf $(TARGET)/lib/libucrypto-mbedtls-uaes.a
	@rm -rf $(TARGET)/lib/libucrypto-mbedtls-uhash.a
	@rm -rf $(TARGET)/lib/libucrypto-secp256k1-uecc.a
	@rm -rf $(TARGET)/lib/libucrypto-secp256k1-uhash.a
	@rm -rf $(TARGET)/obj

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
