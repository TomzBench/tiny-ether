#!/bin/bash

source ./scripts/test_config.sh

for bin in ${BIN[@]} ;do
	eval $bin
done
