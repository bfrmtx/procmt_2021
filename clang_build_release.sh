#!/bin/zsh
clear
PROJ='procmt_2021'
INTSTALLDIR='/usr/local/procmt'
# put your github download here:
SRC_DIR='./'
# in my case /home/bfr/github_procmt_2021/procmt_2021 is where the CmakeLists.txt is
# this directory will be created by cmake respectivly mkdir -p
BUILD_DIR=$HOME'/build/'$PROJ/'build'
# in my case /home/bfr/build/procmt_2021/build
#
mkdir -p $BUILD_DIR
#
# the openGL version seems to be faulty! Don't use
#cmake -S $SRC_DIR -B $BUILD_DIR -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_INSTALL_PREFIX=$INTSTALLDIR -DCMAKE_BUILD_TYPE=RELEASE -DQCUSTOMPLOT_USE_OPENGL=TRUE -DUSE_SPECTRAL_PLOTTER_PRO=TRUE
cmake -S $SRC_DIR -B $BUILD_DIR -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_INSTALL_PREFIX=$INTSTALLDIR -DCMAKE_BUILD_TYPE=RELEASE
# make a single thread in case --parallel 8
cmake --build $BUILD_DIR -v --parallel 16
# install needs write access to /usr/local/procmt - im my case I simply "sudo mkdir /usr/local/procmt"
# libraries and headers will go below; so you can alway "uninstall" by removing the /usr/local/procmt directory
cmake --install $BUILD_DIR
# COMMENT the "old" ediplotter can't be build with Qt6; this version will call "ediplotter_new" in case "ediplotter" is missing
#
# detect the OS
OS=$(uname -s)
# if darwin (macOS) then do not cp ediplotter
if [ $OS = "Darwin" ]; then
  echo "OS is macOS - no old ediplotter available"
  echo "done"
  exit 0
fi
# if not macOS then copy old ediplotter ediplotter
echo "try to copy ediplotter"
cp $HOME'/bin/proc_mt_s/ediplotter' $INTSTALLDIR/bin
echo "done"
