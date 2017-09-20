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
#APPLICATIONS 	+=	libup2p/test

# Build vars
APP_SRCS	+=	$(APPLICATIONS)
MODULE_SRCS 	+= 	$(addsuffix /src,$(MODULES))
MODULE_INCS 	+= 	$(addsuffix /include,$(MODULES))
MODULE_DIRS 	+=	$(MODULE_INCS) $(MODULE_SRCS)
DIRS 		+=	$(addprefix $(TARGET)/obj/,$(MODULE_SRCS))
DIRS 		+=	$(addprefix $(TARGET)/obj/,$(APP_SRCS))
DIRS 		+=	$(TARGET)/bin
LIBS 		+= 	$(addprefix $(TARGET)/lib/,$(addsuffix .a,$(foreach mod, $(MODULES),$(subst /,-,$(mod)))))
APPS 		+= 	$(addprefix $(TARGET)/bin/,$(foreach bin, $(APPLICATIONS),$(subst /,-,$(bin))))
SRCS 		+=	$(shell find $(MODULE_SRCS) -maxdepth '1' -name '*.c')
SRCS 		+=	$(shell find $(APP_SRCS) -maxdepth '1' -name '*.c')
HDRS 		+= 	$(shell find $(MODULE_INCS) -maxdepth '1' -name '*.h')
OBJS		+=	$(addprefix $(TARGET)/obj/,$(SRCS:.c=.o))
INCS		+=	$(addprefix -I./,$(MODULE_DIRS))
INCS	 	+=	$(addprefix -I./,$(TARGET)/include)
DEFS 		+= 	$(addprefix -D,$(CONFIGS_D))
CFLAGS 		+= 	$(DEFS)
LDFLAGS 	+=	$(LIBS) $(addprefix $(TARGET)/lib/, libmbedcrypto.a libsecp256k1.a)
INSTALL 	+= 	$(LIBS)
INSTALL 	+= 	$(APPS)
INSTALL 	+= 	$(addprefix $(TARGET)/include/,$(notdir $(HDRS)))

all: $(DIRS) $(OBJS) $(INSTALL)

$(APPS): $(DIRS) $(LIBS)
	@echo "LINK $(notdir $@)"
	@$(CC) $(shell find $(subst $(TARGET)/bin,$(TARGET)/obj,$(subst -,/,$@)) -name '*.o') \
		$(LDFLAGS) $(INCS) -o $@

# The name convention allows collecting lib objects with find,
# IE: $(TARGET)/lib/libucrypto-mbedtls-uaes.a:=$(TARGET)/obj/libucrypto/mbedtls/uaes/**/*.o
$(TARGET)/lib/%.a:
	@echo "LINK $(notdir $@)"
	@ar rcs $@ $(shell find \
		$(subst $(TARGET)/lib,$(TARGET)/obj,$(subst .a,,$(subst -,/,$@))) -name '*.o')

# .c->.o
$(TARGET)/obj/%.o: %.c
	@echo "  CC $(notdir $<)"
	@${CC} -c ${CFLAGS} $(INCS) $< -o $@ 

# create some output directories
$(DIRS):
	@echo "MKDR $@"
	@${MKDIR_P} ${DIRS}

# All libraries must have an 'include' directory if they want headers installed
$(TARGET)/include/%.h:
	@echo "COPY $(notdir $@)"
	@cp $(shell find $(MODULE_INCS) -maxdepth '1' -name $(notdir $@)) $@

.PHONY: clean test print

# clean our mess
clean:
	@echo "CLEAN"
	@rm -rf $(INSTALL)
	@rm -rf $(TARGET)/obj

test:
	@$(foreach bin,$(APPS),     \
		echo TEST $(bin);   \
		valgrind            \
		--track-origins=yes \
		--tool=memcheck     \
		--leak-check=full   \
		--quiet             \
		./$(bin);           \
		)

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
		APPS: $(APPS) \
		INST: $(INSTALL) \
		OBJ_DIR: $(OBJ_DIR) \
		SRC_DIR: $(SRC_DIR) \
		| sed -e 's/\s\+/\n/g' 

#
#
#
