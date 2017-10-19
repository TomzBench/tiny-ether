#!/bin/bash

source ./scripts/test_config.sh

for bin in ${BIN[$@]};do
	eval valgrind  \
		--track-origins=yes \
		--tool=memcheck \
		--leak-check=full \
		--quiet \
		$bin
done
