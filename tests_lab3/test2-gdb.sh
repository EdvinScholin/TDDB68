#!/bin/sh
# Rättningsskript
cd ../src/userprog
make
cd build
pintos-mkdisk fs.dsk 2
pintos -v --qemu -- -f -q
pintos -v --qemu -p ../../examples/lab3test2 -a lab3test2 -- -q 
pintos -v --qemu -p ../../examples/printf -a printf -- -q
# pintos --qemu -- run lab3test2
pintos -v --qemu --gdb -- run lab3test2

