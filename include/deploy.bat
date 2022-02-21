set PROJ=procmt_2021
set out=%HOMEDRIVE%%HOMEPATH%\build\%PROJ%
set targ=%HOMEDRIVE%%HOMEPATH%\install\%PROJ%\bin

rem for %%f in (*.exe) do windeployqt --compiler-runtime --release --no-translations  %%f
for %%f in (%targ%\*.exe) do windeployqt --compiler-runtime --release --no-translations %%f