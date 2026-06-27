@echo off
setlocal

set "VSBT=%ProgramFiles(x86)%\Microsoft Visual Studio\2022\BuildTools"
call "%VSBT%\VC\Auxiliary\Build\vcvars64.bat"
if errorlevel 1 exit /b %errorlevel%

set "QTDIR=C:\Qt\6.11.1\msvc2022_64"
if exist "%QTDIR%\bin" set "PATH=%QTDIR%\bin;%PATH%"

set "base=%HOMEDRIVE%%HOMEPATH%\mtxsw"
if not exist "%base%" (
mkdir "%base%"
)

set "installdir=%base%\install\procmt"

set "srcdir=Z:\github_procmt_2021\procmt_2021"
set "builddir=%base%\build\procmt"
if exist "%builddir%\CMakeCache.txt" rmdir /s /q "%builddir%"
cmake -S "%srcdir%" -B "%builddir%" -GNinja -DCMAKE_CXX_COMPILER=cl -DCMAKE_CXX_STANDARD=20 -DCMAKE_INSTALL_PREFIX="%installdir%" -DCMAKE_VERBOSE_MAKEFILE=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="%QTDIR%"
if errorlevel 1 exit /b %errorlevel%
cmake --build "%builddir%"
if errorlevel 1 exit /b %errorlevel%
cmake --install "%builddir%"
if errorlevel 1 exit /b %errorlevel%

