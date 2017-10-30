#!/bin/bash

SOURCES="$(find \
	./apps \
	./libucrypto/mbedtls \
	./libucrypto/secp256k1 \
	./libucrypto/test \
	./libucrypto/uecies_decrypt.c \
	./libucrypto/uecies_decrypt.h \
	./libucrypto/uecies_encrypt.c \
	./libucrypto/uecies_encrypt.h \
	./libucrypto/unonce.c \
	./libucrypto/unonce.h \
	./libueth \
	./libup2p \
	./liburlp \
	./libusys \
	-name '*.c' -o -name '*.h')"
SOURCES=($SOURCES)

for(( i = 0; i < ${#SOURCES[@]} ; i++));do
	if ! grep -q Copyright ${SOURCES[$i]}
	then
		echo ${SOURCES[$i]} want copyright
		cat ./scripts/copyright.txt ${SOURCES[$i]} > ${SOURCES[$i]}.new
		mv ${SOURCES[$i]}.new ${SOURCES[$i]}
	fi
done
