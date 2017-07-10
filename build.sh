#!/bin/sh

export NDK_TOOLCHAIN_VERSION=4.9
export NDK_PATH=/mnt/data/ndk/android-ndk-r10d
export PATH=$NDK_PATH:$PATH
make -f $2.mak $1 NDK=1 APP_API_LEVEL=21 CFG=debug NDK_VERBOSE=1 COPY_DLL_TO_SRV=0 GEN_SYM=true -j4
