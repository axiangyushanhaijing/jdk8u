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

CC=/opt/riscv64/bin/riscv64-unknown-linux-gnu-gcc \
CXX=/opt/riscv64/bin/riscv64-unknown-linux-gnu-g++ \
bash configure \
--with-sysroot=/opt/riscv64/sysroot \
--openjdk-target=riscv64-unknown-linux-gnu \
--with-boot-jdk=/home/zhangxiang/jdk-7/java-se-7u75-ri \
--with-native-debug-symbols=internal \
--with-jvm-variants=server \
--with-debug-level=slowdebug \
#--with-jvm-features=cds,cmsgc,compiler2,epsilongc,g1gc,jfr,jni-check,jvmti,management,nmt,parallelgc,serialgc,services,vm-structs


CC=/opt/riscv64/bin/riscv64-unknown-linux-gnu-gcc \
CXX=/opt/riscv64/bin/riscv64-unknown-linux-gnu-g++ \
bash configure \
--with-sysroot=/opt/riscv64/sysroot \
--openjdk-target=riscv64-unknown-linux-gnu \
--with-boot-jdk=/home/zhangxiang/jdk-7/java-se-7u75-ri \
--with-native-debug-symbols=internal \
--with-jvm-variants=client \
--with-debug-level=slowdebug \

CC=/opt/riscv64/bin/riscv64-unknown-linux-gnu-gcc \
CXX=/opt/riscv64/bin/riscv64-unknown-linux-gnu-g++ \
bash configure \
--with-sysroot=/opt/riscv64/sysroot \
--openjdk-target=riscv64-unknown-linux-gnu \
--with-boot-jdk=/home/zhangxiang/jdk-7/java-se-7u75-ri \
--with-jvm-variants=core \
--with-debug-level=slowdebug \
--with-native-debug-symbols=internal \
--x-includes=/opt/riscv64/sysroot/usr/include/X11/extension \
--x-libraries=/opt/riscv64/sysroot/usr/lib \
--with-cups=/opt/riscv64/sysroot/usr/include/cups \
--with-freetype-include=/opt/riscv64/sysroot/usr/include/freetype2 \
--with-freetype-lib=/opt/riscv64/sysroot/usr/lib \
--with-extra-cflags="-Wno-error" 

CC=/opt/riscv64/bin/riscv64-unknown-linux-gnu-gcc \
CXX=/opt/riscv64/bin/riscv64-unknown-linux-gnu-g++ \
bash configure \
--with-sysroot=/opt/riscv64/sysroot \
--openjdk-target=riscv64-unknown-linux-gnu \
--with-boot-jdk=/home/zhangxiang/jdk-7/java-se-7u75-ri \
--with-jvm-variants=core \
--with-debug-level=release \
--with-native-debug-symbols=none \
--x-includes=/opt/riscv64/sysroot/usr/include/X11/extension \
--x-libraries=/opt/riscv64/sysroot/usr/lib \
--with-cups=/opt/riscv64/sysroot/usr/include/cups \
--with-freetype-include=/opt/riscv64/sysroot/usr/include/freetype2 \
--with-freetype-lib=/opt/riscv64/sysroot/usr/lib \
--with-extra-cflags="-Wno-error" 
make -d JOBS=$(nproc)

#cp /home/zifeihan/hsdis/hsdis-riscv32.so /home/zifeihan/jdk11u/build/linux-riscv32-normal-core-slowdebug/jdk/lib/server

# /home/zifeihan/qemu32/bin/qemu-riscv32 -L /opt/riscv32/sysroot -g 28080 ./java -XX:+TraceBytecodes -XX:+PrintInterpreter -version

