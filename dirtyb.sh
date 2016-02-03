#!/bin/bash

export ARCH=arm
export SUBARCH=arm
export KBUILD_BUILD_USER="anomalchik"
export KBUILD_BUILD_HOST="sweetmachine"
export CROSS_COMPILE=/home/anomalchik/tch/arm-cortex_a7-linux-gnueabihf-linaro_4.9/bin/arm-cortex_a7-linux-gnueabihf-
#make clean
make mrproper
make alps_hongyu82_defconfig
make -j4 zImage
