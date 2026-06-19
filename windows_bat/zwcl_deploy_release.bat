@echo off
rem This script deploys the Qt application built with MSVC 2022
rem I use the powershell with option programming
setlocal EnableExtensions EnableDelayedExpansion
set "VSBT=%ProgramFiles(x86)%\Microsoft Visual Studio\2022\BuildTools"
call "%VSBT%\VC\Auxiliary\Build\vcvars64.bat"
if errorlevel 1 exit /b %errorlevel%
set "base=%HOMEDRIVE%%HOMEPATH%\mtxsw"
set "installdir=%base%\install\procmt"
set "bindir=%installdir%\bin"
set "libdir=%installdir%\lib"
rem exit if bindir does not exist
if not exist "%bindir%" (
    echo The directory "%bindir%" does not exist.
    exit /b 1
)
rem now we crete the directory if not existing procmt_zip_dir in base
set "procmt_zip_root=%base%\procmt_zip_dir"
set "procmt_zip_dir=%procmt_zip_root%\procmt"
if not exist "%procmt_zip_root%" (
    mkdir "%procmt_zip_root%"
)
if not exist "%procmt_zip_dir%" (
    mkdir "%procmt_zip_dir%"
)
rem copy all files from bindir to procmt_zip_dir recursively
xcopy /E /Y /I "%bindir%\*" "%procmt_zip_dir%\"
rem set the bindir to the procmt_zip_dir
set "bindir=%procmt_zip_dir%"
rem call windeployqt
set "qtdir=C:\Qt\6.11.1\msvc2022_64\bin"
set "windeployqt=%qtdir%\windeployqt.exe"
if not exist "%windeployqt%" set "windeployqt=%qtdir%\windeployqt6.exe"
if not exist "%windeployqt%" (
    echo windeployqt not found in "%qtdir%".
    exit /b 1
)
for %%f in ("%bindir%\*.exe") do (
    if /I not "%%~nxf"=="vc_redist.x64.exe" (
        "%windeployqt%" --compiler-runtime --no-translations --dir "%bindir%" "%%~ff"
        if errorlevel 1 exit /b %errorlevel%
    )
)
set "extra_dlls=Qt6OpenGL.dll Qt6OpenGLWidgets.dll Qt6PrintSupport.dll Qt6Xml.dll Qt6SvgWidgets.dll Qt6Svg.dll"
rem copy the dll if not existing in bindir without confirmation
for %%d in (%extra_dlls%) do (
    if not exist "%bindir%\%%d" (
        copy /Y "%qtdir%\%%d" "%bindir%"
    )
)
rem if bindir contains vc_redist.x64.exe, then make a subdirectory named vc_redist_x64 under procmt_zip_dir and move vc_redist.x64.exe to that directory
if exist "%bindir%\vc_redist.x64.exe" (
    if not exist "%procmt_zip_dir%\vc_redist_x64" (
        mkdir "%procmt_zip_dir%\vc_redist_x64"
    )
    move /Y "%bindir%\vc_redist.x64.exe" "%procmt_zip_dir%\vc_redist_x64\"
)
rem now we zip the procmt_zip_dir
set "zipfile=%base%\procmt.zip"
if exist "%zipfile%" (
    del /Q "%zipfile%"
)
rem use powershell to zip the procmt_zip_dir
rem powershell -Command "Add-Type -A 'System.IO.Compression.FileSystem'; [IO.Compression.ZipFile]::CreateFromDirectory('%procmt_zip_dir%', '%zipfile%', [IO.Compression.CompressionLevel]::Optimal, $false)"
rem we use the famous 7z to zip the procmt_zip_dir
set zipper=C:\Program Files\7-Zip\7z.exe
if not exist "%zipper%" (
    echo 7-Zip not found at %zipper%. Please install 7-Zip and ensure it is in the specified path.
    exit /b 1
)
"%zipper%" a -tzip "%zipfile%" "%procmt_zip_root%\procmt" -mx=9 -mmt -aoa
if errorlevel 1 exit /b %errorlevel%
rem finished
echo Deployment completed successfully. The zip file is located at %zipfile%.