cls 
rem base for QT6 based projects
rem set base vars here
set qtversion=6.8.2
rem set the base of your c/c++ projects here
set base=%HOMEDRIVE%%HOMEPATH%\mtxsw
if not exist %base% ( 
mkdir %base% 
)
