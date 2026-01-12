rem global vars
call "base6.bat"
rem set vars here
set PROJ=procmt
rem finished setting vars
call "base_dirs.bat" %PROJ%
IF EXIST "C:\Users\ea2abc\mtxsw\install\tsmp\bin\tsmp.exe" copy "C:\Users\ea2abc\mtxsw\install\tsmp\bin\tsmp.exe"  %bindir%

windeployqt.exe --libdir %libdir% --plugindir %bindir% --no-translations --compiler-runtime %bindir%

cd C:\Qt\%qtversion%\mingw_64\bin
copy Qt6OpenGL.dll %libdir%
copy Qt6OpenGLWidgets.dll %libdir%
copy Qt6PrintSupport.dll %libdir%
copy Qt6Xml.dll %libdir%
copy Qt6SvgWidgets.dll %libdir%
copy Qt6Svg.dll %libdir%