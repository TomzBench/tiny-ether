# common.mk

BRAND:=mtm
MKDIR_P:= mkdir -p

CFLAGS?=-O0 -g -Wall -DNDEBUG 
LDFLAGS:= -lwolfssl
CC:=cc -std=gnu11

${dirs}:
	${MKDIR_P} ${dirs}
