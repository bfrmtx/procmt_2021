#!/bin/bash
clear
PROJ='procmt_2021'
INTSTALLDIR='/usr/local/procmt'
# debug here
# INTSTALLDIR=$HOME'/build/'$PROJ/'install'
SRC_DIR=$HOME'/devel/github_procmt_2021/'$PROJ
BUILD_DIR=$HOME'/build/'$PROJ/'build'
#
mkdir -p $BUILD_DIR
#cmake -S $SRC_DIR -B $BUILD_DIR -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_COMPILER=g++ -DCMAKE_INSTALL_PREFIX=$INTSTALLDIR -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_BUILD_TYPE=RELEASE
cmake -S $SRC_DIR -B $BUILD_DIR -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_INSTALL_PREFIX=$INTSTALLDIR -DCMAKE_BUILD_TYPE=RELEASE
cmake --build $BUILD_DIR --parallel 4
# in case make a single thread
# cmake --build $BUILD_DIR
cmake --install $BUILD_DIR
cp $HOME'/bin/proc_mt_s/ediplotter' $INTSTALLDIR/bin

