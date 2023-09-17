#!/bin/zsh
clear
PROJ='procmt_2021'
INTSTALLDIR='/usr/local/procmt'
# debug here
# put your github download here:
SRC_DIR=$HOME'/devel/github_procmt_2021/'$PROJ
# in my case /home/bfr/github_procmt_2021/procmt_2021 is where the CmakeLists.txt is
# this directory will be created by cmake respectivly mkdir -p
BUILD_DIR=$HOME'/build/'$PROJ/'build'
# in my case /home/bfr/build/procmt_2021/build
#
#
mkdir -p $BUILD_DIR
cmake -S $SRC_DIR -B $BUILD_DIR -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_INSTALL_PREFIX=$INTSTALLDIR -DCMAKE_BUILD_TYPE=DEBUG -DQCUSTOMPLOT_USE_OPENGL=TRUE -DUSE_SPECTRAL_PLOTTER_PRO=TRUE
# make a single thread in case
cmake --build $BUILD_DIR --parallel 8
# install needs write access to /usr/local/procmt - im my case I simply "sudo mkdir /usr/local/procmt"
# libraries and headers will go below; so you can alway "uninstall" by removing the /usr/local/procmt directory
cmake --install $BUILD_DIR
# COMMENT this line out - the "old" ediplotter can't be build with Qt6; this version will call "ediplotter_new" in case "ediplotter" is missing
cp $HOME'/bin/proc_mt_s/ediplotter' $INTSTALLDIR/bin
