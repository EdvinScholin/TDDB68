#!/bin/sh
# Rättningsskript
cd ../src/userprog
make
cd build
pintos-mkdisk fs.dsk 2
pintos -v --qemu -- -f -q
#pintos -v --qemu -p ../../examples/lab4test1 -a lab4test1 -- -q 
pintos -v --qemu -p ../../examples/lab1test2 -a lab1test2 -- -q 
#pintos -v --qemu -p tests/userprog/create-normal -a create-normal -- -q
#pintos -v --qemu -p ../../examples/printf -a printf -- -q
#pintos -v --qemu --gdb -- run 'lab4test1 arg1 arg2 arg3'
#pintos -v --qemu -- run 'lab4test1 arg1 arg2 arg3'
pintos -v --qemu -- run 'lab1test2'
#pintos -v --qemu --gdb -- run 'create-normal'
#pintos -v -k -T 60 --qemu  --fs-disk=2 -p tests/userprog/create-normal -a create-normal -- -q   -f run create-normal
#pintos -v -k -T 60 --qemu -- run 'create-normal'
