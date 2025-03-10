rem global vars
call "base6.bat"
rem set vars here
set PROJ=procmt
rem finished setting vars
call "base_dirs.bat" %PROJ%
IF EXIST "%base%\install\tsmp\bin\tsmp.exe" copy "%base%\install\tsmp\bin\tsmp.exe"  %bindir%

windeployqt6.exe --libdir %libdir% --plugindir %bindir% --no-translations --compiler-runtime %bindir%

rem cd C:\Qt\%qtversion%\mingw_64\bin
rem qt version seems not to mentioned here
cd C:\msys64\ucrt64\bin
copy Qt6OpenGL.dll %libdir%
copy Qt6OpenGLWidgets.dll %libdir%
copy Qt6PrintSupport.dll %libdir%
copy Qt6Xml.dll %libdir%
copy Qt6SvgWidgets.dll %libdir%
copy Qt6Svg.dll %libdir%
