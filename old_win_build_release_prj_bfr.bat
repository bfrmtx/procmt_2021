cls
set PROJ=procmt2021
set out="C:\Users\bfr\Documents\build"
set targ="C:\Users\bfr\Documents\procmt2021"

set in="Y:\bbcloud\devel\procmt2021"
md %out%
md %targ%
cmake -S %in% -B %out% -DCMAKE_INSTALL_PREFIX=%targ% -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_BUILD_TYPE=RELEASE
cmake --build %out% --parallel 8
cmake --install %targ%
