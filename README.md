# ProcMT 2021

ProcMT stands for **Proc**ess **M**agneto**T**elluric data.

Presently (Feb 2022) this repository contains a flawly pre-release of the  [Qt6](https://www.qt.io/) version.

May 2023: changes for Qt 6.5
August 2023: edi files with ZROT and TROT

Other libraries are marked individually.

# Prerequisites

* Linux: clang++ / gcc compiler with C++ 17 support 
* Apple clang version 12 or above
* MSVC on Windows ... to be checked
* tested with MinGW64 compiler & Ninja make, shipped with Qt 6.5
* CMake 3.20 or above (Version is upgrading fast - Cmake is tied to compilers and their features)
* C++ Boost library installed 1.6 or later (*find_package(Boost REQUIRED)*)
* the Boost libraries do not necessarily to be compiled
* on Windows Khronos Vulkan is needed in order to avoid error messages [Vulkan](https://vulkan.lunarg.com/sdk/home)
* C++ package Qt 6.5 or later
* OpenGL and Sql libraries from Qt

## cmake example

Look at `clang_build_release_bfr.sh` for an example of a cmake command line.

# License

GPL v3<br>
(this is due to some underlying libraries, not decided by me).

