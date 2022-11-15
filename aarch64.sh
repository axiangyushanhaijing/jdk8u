#!/bin/bash

echo "************************************************************************"
echo "************************************************************************"
echo "************************************************************************"
echo ""
echo ""
echo "start make"
echo ""
echo ""
echo "************************************************************************"
echo "************************************************************************"
echo "************************************************************************"

rm -rf build/

make clean

bash configure \
--with-boot-jdk=/home/dingli/zulu7.52.0.11-ca-jdk7.0.332-linux_x64 \
--with-jvm-variants=core \
--with-debug-level=slowdebug \
--with-extra-cflags="-Wno-error"
#make LOG=info > x86_make.log

#cp /home/zifeihan/hsdis/hsdis-riscv32.so /home/zifeihan/jdk11u/build/linux-riscv32-normal-core-slowdebug/jdk/lib/server

# /home/zifeihan/qemu32/bin/qemu-riscv32 -L /opt/riscv32/sysroot -g 28080 ./java -XX:+TraceBytecodes -XX:+PrintInterpreter -version

