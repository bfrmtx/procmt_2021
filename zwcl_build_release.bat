rem I use the powershell with option programming
rem add path for cl.exe
setlocal
set "VSINSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools"
set "PATH=%VSINSTALLDIR%\Common7\IDE;%VSINSTALLDIR%\Common7\Tools;%VSINSTALLDIR%\Common7\Tools\bin\Hostx64\x64;%PATH%"
set "INCLUDE=%VSINSTALLDIR%\VC\Tools\MSVC\14.34.31933\include;%VSINSTALLDIR%\VC\Tools\MSVC\14.34.31933\atlmfc\include;%INCLUDE%"
set "LIB=%VSINSTALLDIR%\VC\Tools\MSVC\14.34.31933\lib\x64;%VSINSTALLDIR%\VC\Tools\MSVC\14.34.31933\atlmfc\lib\x64;%LIB%"

rem call "%VSINSTALLDIR%VC\Auxiliary\Build\vcvars64.bat"
call  'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat'
set "base=%HOMEDRIVE%%HOMEPATH%\mtxsw"
if not exist %base% ( 
mkdir %base% 
)
set builddir=%base%\build\tsmp
set installdir=%base%\install\procmt
set bindir=%installdir%\bin
set libdir=%installdir%\lib
rem
set srcdir="Z:\tsmp"
cmake -S %srcdir% -B %builddir% -GNinja  -DCMAKE_CXX_COMPILER=cl -DCMAKE_CXX_STANDARD=20 -DCMAKE_INSTALL_PREFIX=%installdir% -DCMAKE_VERBOSE_MAKEFILE=OFF -DCMAKE_BUILD_TYPE=RELEASE
cmake --build %builddir%
cmake --install %builddir%
set srcdir="Z:\github_procmt_2021\procmt_2021"
set builddir=%base%\build\procmt
cmake -S %srcdir% -B %builddir% -GNinja  -DCMAKE_CXX_COMPILER=cl -DCMAKE_CXX_STANDARD=20 -DCMAKE_INSTALL_PREFIX=%installdir% -DCMAKE_VERBOSE_MAKEFILE=OFF -DCMAKE_BUILD_TYPE=RELEASE
cmake --build %builddir%
cmake --install %builddir%

