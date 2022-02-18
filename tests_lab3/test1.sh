#!/bin/sh
# Rättningsskript
cd ../src/userprog
make
cd build
pintos-mkdisk fs.dsk 2
pintos -v --qemu -- -f -q
pintos -v --qemu -p ../../examples/lab3test1 -a lab3test1 -- -q 
pintos -v --qemu -p ../../examples/printf -a printf -- -q
pintos -v --qemu -- run lab3test1
# pintos --qemu --gdb -- run lab3test2

