#!/bin/sh
# Rättningsskript
cd src/userprog
make
cd build
pintos-mkdisk fs.dsk 2
pintos --qemu -- -f -q
pintos --qemu -p ../../examples/lab3test2 -a lab3test2 -- -q 
# pintos --qemu --gdb -- run lab3test2
pintos --qemu --gdb -- run lab3test2

