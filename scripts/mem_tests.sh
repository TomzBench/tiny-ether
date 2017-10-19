#!/bin/bash

source ./scripts/test_config.sh

for(( i = 0; i < ${#BIN[@]} ; i++));do
	VAL[$i]="valgrind --tool=memcheck
			  --leak-check=full
			  --track-origins=yes
			  --quiet ${BIN[$i]}"
done


for(( i = 0; i < ${#VAL[@]} ; i++));do
	echo TEST: ${BIN[$i]}
	eval ${VAL[$i]}
done
