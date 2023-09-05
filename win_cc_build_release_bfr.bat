cls
set PROJ=procmt_2021
set out=%HOMEDRIVE%%HOMEPATH%\build\%PROJ%
set targ=%HOMEDRIVE%%HOMEPATH%\install\%PROJ%
rem set in="Y:\github_procmt_2021\procmt_2021"
set in="Z:\github_procmt_2021\procmt_2021"
rem set in="Z:/procmt_2021"
rem setx BOOST_ROOT C:/boost/boost_1_78_0
rem setx /u bfr BOOST_INCLUDEDIR c:/boost/boost_1_78_0/boost
rem setx Boost_INCLUDEDIR C:/boost/boost_1_78_0/boost
rem
md %out%
md %targ%
rem -DCMAKE_CXX_STANDARD=17
cmake -S %in% -B %out% -GNinja -DCMAKE_CXX_COMPILER=c++ -DCMAKE_INSTALL_PREFIX=%targ% -DCMAKE_VERBOSE_MAKEFILE=OFF -DCMAKE_BUILD_TYPE=RELEASE -DQCUSTOMPLOT_USE_OPENGL=TRUE -DUSE_SPECTRAL_PLOTTER_PRO=TRUE
cmake --build %out%
cmake --install %out%
