#!/bin/bash

find . \( -name '*.c' -o -name '*.h' -o -name '*.hpp' -o -name '*.cpp' -o -name '*.cxx' \) -print > cscope.files
cscope -b
