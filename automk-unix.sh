# !/bin/sh

./configure --disable-menu \
            --disable-caca \
            --disable-vidix \
            --disable-vidix-pcidb 

make

make install
