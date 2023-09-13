rem global vars
call "base6.bat"
rem set vars here
set PROJ=tsmp
rem finished setting vars
call "base_dirs.bat" %PROJ%

windeployqt.exe --libdir %libdir% --plugindir %bindir% --no-translations --compiler-runtime %bindir%
