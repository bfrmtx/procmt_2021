#!/bin/bash
clear
PROJ='tsmp'
INTSTALLDIR='/usr/local/procmt'
# debug here
SRC_DIR=$HOME'/devel/tsmp'
# build outside please!
BUILD_DIR=$HOME'/build/'$PROJ'/build'
#
# uncomment when final
#INTSTALLDIR=$HOME'/build/'$PROJ'_cpp/install'


#
mkdir -p $BUILD_DIR
# GCC
#cmake -S $SRC_DIR -B $BUILD_DIR -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_COMPILER=g++ -DCMAKE_INSTALL_PREFIX=$INTSTALLDIR -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_BUILD_TYPE=RELEASE
#
cmake -S $SRC_DIR -B $BUILD_DIR -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_INSTALL_PREFIX=$INTSTALLDIR -DCMAKE_BUILD_TYPE=RELEASE
#
# in case make a single thread (maybe facingin dependency errors) remove --parallel 8
#
cmake --build $BUILD_DIR --parallel 8
cmake --install $BUILD_DIR
