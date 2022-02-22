#!/bin/sh
# Rättningsskript
cd ../src/userprog
make
cd build
pintos-mkdisk fs.dsk 2
pintos -v --qemu -- -f -q
pintos -v --qemu -p ../../examples/lab4test1 -a lab4test1 -- -q 
pintos -v --qemu -p ../../examples/printf -a printf -- -q
pintos --qemu -- run 'lab4test1 arg1 arg2 arg3'
# pintos --qemu --gdb -- run lab3test2

