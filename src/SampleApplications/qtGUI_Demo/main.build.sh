#!/bin/bash

# build the binary, hidden step: generate ui header files
make -j4


# make clean wipes the temporary ui headers
if [ ! -d `pwd` ]; then  
  make clean
fi

touch bogus.o
rm *.o


