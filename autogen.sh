#!/bin/sh -x
cd ./scripts
perl createAMMakefile.pl
cd ..

aclocal
automake --foreign
autoconf
./configure $*
