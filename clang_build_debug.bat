set base=%HOMEDRIVE%%HOMEPATH%\mtxsw
if not exist %base% ( 
mkdir %base% 
)
set builddir=%base%\build\tsmp
set installdir=%base%\install\procmt
set bindir=%installdir%\bin
set libdir=%installdir%\lib
set msys64=C:\msys64\ucrt64\bin
rem set srcdir="Z:\tsmp"
set srcdir=.
rem cmake -S %srcdir% -B %builddir% -GNinja -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_STANDARD=20 -DCMAKE_INSTALL_PREFIX=%installdir% -DCMAKE_VERBOSE_MAKEFILE=OFF -DCMAKE_BUILD_TYPE=RELEASE
rem cmake --build %builddir%
rem cmake --install %builddir%
rem set srcdir="Z:\github_procmt_2021\procmt_2021"
set builddir=%base%\build\procmt
cmake -S %srcdir% -B %builddir% -GNinja -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_STANDARD=20 -DCMAKE_INSTALL_PREFIX=%installdir% -DCMAKE_VERBOSE_MAKEFILE=OFF -DCMAKE_BUILD_TYPE=DEBUG
cmake --build %builddir%
cmake --install %builddir%
windeployqt6.exe --libdir %libdir% --plugindir %bindir% --no-translations --compiler-runtime %bindir%
rem copy %msys64%\*.dll %libdir%