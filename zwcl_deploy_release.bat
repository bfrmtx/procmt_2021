@echo off
rem This script deploys the Qt application built with MSVC 2022
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
set installdir=%base%\install\procmt
set bindir=%installdir%\bin
set libdir=%installdir%\lib
rem exit if bindir does not exist
if not exist %bindir% (
    echo The directory %bindir% does not exist.
    exit /b 1
)
rem now we crete the directory if not existing procmt_zip_dir in base
set procmt_zip_dir=%base%\procmt_zip_dir
if not exist %procmt_zip_dir% (
    mkdir %procmt_zip_dir%
    rem make a subdirectory named procmt under procmt_zip_dir
    mkdir %procmt_zip_dir%\procmt
    set procmt_zip_dir=%procmt_zip_dir%\procmt
)
rem copy all files from bindir to procmt_zip_dir recursively
xcopy /E /Y /I %bindir%\* %procmt_zip_dir%\
rem set the bindir to the procmt_zip_dir
set bindir=%procmt_zip_dir%
rem call windeployqt6
set qtdir=C:\Qt\6.9.0\msvc2022_64\bin
%qtdir%\windeployqt6 --compiler-runtime --plugindir %bindir% --libdir %bindir% --no-translations %bindir%
set extra_dlls=Qt6OpenGL.dll Qt6OpenGLWidgets.dll Qt6PrintSupport.dll Qt6Xml.dll Qt6SvgWidgets.dll Qt6Svg.dll
rem copy the dll if not existing in bindir without confirmation
for %%d in (%extra_dlls%) do (
    if not exist %bindir%\%%d (
        copy /Y %qtdir%\%%d %bindir%
    )
)
rem if bindir contains vc_redist.x64.exe, then make a subdirectory named vc_redist_x64 under procmt_zip_dir and move vc_redist.x64.exe to that directory
if exist %bindir%\vc_redist.x64.exe (
    if not exist %procmt_zip_dir%\vc_redist_x64 (
        mkdir %procmt_zip_dir%\vc_redist_x64
    )
    move /Y %bindir%\vc_redist.x64.exe %procmt_zip_dir%\vc_redist_x64\
)
rem now we zip the procmt_zip_dir
set zipfile=%base%\procmt.zip
if exist %zipfile% (
    del /Q %zipfile%
)
rem use powershell to zip the procmt_zip_dir
rem powershell -Command "Add-Type -A 'System.IO.Compression.FileSystem'; [IO.Compression.ZipFile]::CreateFromDirectory('%procmt_zip_dir%', '%zipfile%', [IO.Compression.CompressionLevel]::Optimal, $false)"
rem we use the famous 7z to zip the procmt_zip_dir
set zipper=C:\Program Files\7-Zip\7z.exe
if not exist "%zipper%" (
    echo 7-Zip not found at %zipper%. Please install 7-Zip and ensure it is in the specified path.
    exit /b 1
)
"%zipper%" a -tzip "%zipfile%" "%procmt_zip_dir%" -mx=9 -mmt -aoa
rem finished
echo Deployment completed successfully. The zip file is located at %zipfile%.