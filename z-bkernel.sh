#!/bin/bash

export ARCH=arm CROSS_COMPILE=./toolchain/linaro-4.9/bin/arm-linux-androideabi-
make clean
make mrproper
make a_hongyu82_defconfig
make -j4 zImage
