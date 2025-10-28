@echo off
set base=%HOMEDRIVE%%HOMEPATH%\mtxsw
if not exist %base% ( 
mkdir %base% 
)
set installdir=%base%\install\procmt
set bindir=%installdir%\bin
set libdir=%installdir%\lib
set msys64=C:\msys64\ucrt64\bin

rem Automatically copy required runtime DLLs to %bindir%
rem List of common runtime DLLs for clang++/ucrt64
set dlls=libstdc++-6.dll libgcc_s_seh-1.dll libwinpthread-1.dll libclang-cpp.dll 
for %%d in (%dlls%) do (
    if exist "%msys64%\%%d" (
        copy /Y "%msys64%\%%d" "%libdir%"
    )
)
%msys64%\windeployqt6.exe --libdir %libdir% --plugindir %bindir% --no-translations --compiler-runtime %bindir%
rem add some missing DLLs
set extra_dlls=Qt6OpenGL.dll Qt6OpenGL.dll Qt6OpenGLWidgets.dll Qt6PrintSupport.dll Qt6Xml.dll Qt6SvgWidgets.dll Qt6Svg.dll
for %%d in (%extra_dlls%) do (
    if exist "%msys64%\%%d" (
        copy /Y "%msys64%\%%d" "%libdir%"
    )
)
copy %msys64%\*.dll %libdir%
rem 

