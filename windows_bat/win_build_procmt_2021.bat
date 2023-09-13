rem global vars
call "base6.bat"
rem set vars here
set PROJ=procmt_2021
rem finished setting vars
call "base_dirs.bat" %PROJ%

set srcdir="Z:\github_procmt_2021\procmt_2021"


rem -DCMAKE_CXX_STANDARD=17
cmake -S %srcdir% -B %builddir% -GNinja -DCMAKE_CXX_COMPILER=c++ -DCMAKE_INSTALL_PREFIX=%installdir% -DCMAKE_VERBOSE_MAKEFILE=OFF -DCMAKE_BUILD_TYPE=RELEASE -DQCUSTOMPLOT_USE_OPENGL=TRUE -DUSE_SPECTRAL_PLOTTER_PRO=TRUE
cmake --build %builddir%
cmake --install %builddir%
