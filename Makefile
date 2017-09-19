#ulib makefile

# Intended to build on Linux Only.
# directory structure:
# module
#     src
#         srcs.(c|h)
#     include
#         public.h

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
MODULE_SRCS 	+= 	$(addsuffix /src,$(MODULES))
MODULE_INCS 	+= 	$(addsuffix /include,$(MODULES))
MODULE_DIRS 	+=	$(MODULE_INCS) $(MODULE_SRCS)
DIRS 		+=	$(addprefix $(TARGET)/obj/,$(MODULE_SRCS))
DIRS 		+=	$(addprefix $(TARGET)/obj/,$(APPLICATIONS))
LIBS 		+= 	$(addprefix $(TARGET)/lib/, \
			$(addsuffix .a,$(foreach mod, $(MODULES),$(subst /,-,$(mod)))))
SRCS 		+=	$(shell find $(MODULE_SRCS) -maxdepth '1' -name '*.c')
HDRS 		+= 	$(shell find $(MODULE_INCS) -maxdepth '1' -name '*.h')
OBJS		+=	$(addprefix $(TARGET)/obj/,$(SRCS:.c=.o))
INCS		+=	$(addprefix -I./,$(MODULE_DIRS))
INCS	 	+=	$(addprefix -I./,$(TARGET)/include)
DEFS 		+= 	$(addprefix -D,$(CONFIGS_D))
CFLAGS 		+= 	$(DEFS)
INSTALL 	+= 	$(LIBS)
INSTALL 	+= 	$(addprefix $(TARGET)/include/,$(notdir $(HDRS)))

all: $(DIRS) $(OBJS) $(INSTALL)

# The name convention allows collecting lib objects with find,
# IE: $(TARGET)/lib/libucrypto-mbedtls-uaes.a:=$(TARGET)/obj/libucrypto/mbedtls/uaes/**/*/.o
$(TARGET)/lib/%.a:
	@echo "LINK $@ $(shell find \
		$(subst $(TARGET)/lib,$(TARGET)/obj,$(subst .a,,$(subst -,/,$@))) -name '*.o')"
	@ar rcs $@ $(shell find \
		$(subst $(TARGET)/lib,$(TARGET)/obj,$(subst .a,,$(subst -,/,$@))) -name '*.o')

# .c->.o
$(TARGET)/obj/%.o: %.c
	@echo "  CC $@"
	@${CC} -c ${CFLAGS} ${LDFLAGS} $(INCS) $< -o $@ 

# create some output directories
$(DIRS):
	@echo "MKDR $@"
	@${MKDIR_P} ${DIRS}

# All libraries must have an 'include' directory if they want headers installed
$(TARGET)/include/%.h:
	@echo "COPY $@"
	@cp $(shell find $(MODULE_INCS) -maxdepth '1' -name $(notdir $@)) $@

.PHONY: clean test print

# clean our mess
clean:
	@echo "CLEAN"
	@rm -rf $(INSTALL)
	@rm -rf $(TARGET)/obj

# Makefile debug print
print:
	@echo \
		DIRS: $(DIRS) \
		LIBS: $(LIBS) \
		SRCS: $(SRCS) \
		HDRS: $(HDRS) \
		INCS: $(INCS) \
		OBJS: $(OBJS) \
		DEFS: $(DEFS) \
		INST: $(INSTALL) \
		OBJ_DIR: $(OBJ_DIR) \
		SRC_DIR: $(SRC_DIR) \
		test: $(test) \
		| sed -e 's/\s\+/\n/g' 

#
#
#
