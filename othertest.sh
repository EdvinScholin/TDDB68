#!/bin/sh
# Rättningsskript
cd src/userprog
make
cd build
pintos-mkdisk fs.dsk 2
pintos -v --qemu -- -f -q
pintos -v --qemu -p ../../examples/lab1test2 -a lab1test2 -- -q 
pintos -v --qemu -p ../../examples/printf -a printf -- -q
pintos -v --qemu -- run lab1test2
# pintos --qemu --gdb -- run lab3test2

