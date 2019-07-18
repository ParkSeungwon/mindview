#!/bin/bash

ls */*.{cc,h,cpp,hpp} > cscope.files
cscope -cb
ctags --fields=+i -n -R -L cscope.files
dirname=`pwd`
cqmakedb -s ${dirname##*/}.db -c cscope.out -t tags -p
