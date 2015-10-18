#!/bin/bash

export ARCH=arm CROSS_COMPILE=./toolchain/linaro-4.9/bin/arm-linux-androideabi-
#make clean
#make mrproper
make hongyu82_defconfig
make -j4 zImage
