rem global vars
call "base6.bat"
rem set vars here
set PROJ=procmt
rem finished setting vars
call "base_dirs.bat" %PROJ%
IF EXIST "%base%\install\tsmp\bin\tsmp.exe" copy "%base%\install\tsmp\bin\tsmp.exe"  %bindir%

windeployqt6.exe --libdir %libdir% --plugindir %bindir% --no-translations --compiler-runtime %bindir%

copy %msys64%\*.dll %libdir%

