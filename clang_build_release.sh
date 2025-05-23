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
#
# Number of parallel jobs if you want to limit the number of threads detected by ninja
PARALLEL_JOBS=8
#
mkdir -p $BUILD_DIR
#
# the openGL version seems to be faulty! Don't use
#cmake -S $SRC_DIR -B $BUILD_DIR -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_INSTALL_PREFIX=$INTSTALLDIR -DCMAKE_BUILD_TYPE=RELEASE -DQCUSTOMPLOT_USE_OPENGL=TRUE -DUSE_SPECTRAL_PLOTTER_PRO=TRUE
#
# cmake options DO OVERRIDE the CMakeLists.txt, so -DCMAKE_CXX_STANDARD=20 will override the CMakeLists.txt
# this as well -DQCUSTOMPLOT_USE_OPENGL=TRUE -DUSE_SPECTRAL_PLOTTER_PRO=TRUE
#
if ! which ninja > /dev/null; then
  echo "using build without ninja"
  cmake -S $SRC_DIR -B $BUILD_DIR -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_INSTALL_PREFIX=$INTSTALLDIR -DCMAKE_BUILD_TYPE=RELEASE
  cmake --build $BUILD_DIR --parallel $PARALLEL_JOBS
else
  # pass --debug-find for debug find options
  echo "using build with ninja"
  cmake -GNinja -S $SRC_DIR -B $BUILD_DIR -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_INSTALL_PREFIX=$INTSTALLDIR -DCMAKE_BUILD_TYPE=RELEASE
  # if you want limit the number of threads detected by ninja
  # cmake --build $BUILD_DIR -- -j $PARALLEL_JOBS
  cmake --build $BUILD_DIR
fi
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
