
rem global vars
call "base6.bat"
rem set vars here
set PROJ=tsmp
rem finished setting vars
call "base_dirs.bat" %PROJ%

set srcdir="Z:\tsmp"

rem -DCMAKE_CXX_STANDARD=17
cmake -S %srcdir% -B %builddir% -GNinja -DCMAKE_CXX_COMPILER=c++ -DCMAKE_INSTALL_PREFIX=%installdir% -DCMAKE_VERBOSE_MAKEFILE=OFF -DCMAKE_BUILD_TYPE=RELEASE
cmake --build %builddir%
cmake --install %builddir%