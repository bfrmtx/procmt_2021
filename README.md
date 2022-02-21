# ProcMT 2021

ProcMT stands for **Proc**ess **M**agneto**T**elluric data.

Presently (Jan 2022) this repository contains a flawly pre-release of the  [Qt6](https://www.qt.io/) version.

Other libraries are marked individually.

# Prerequisites

* Linux: clang++ / gcc compiler with C++ 17 support 
* Apple clang version 12 or above
* MSVC on Windows ... to be checked
* tested with MinGW64 compiler & Ninja make, shipped with Qt 6.2
* CMake 3.20 or above (Verion is upgrading fast)
* C++ Boost library installed 1.6 or later (*find_package(Boost REQUIRED)*)
* the Boost libraries do not necessarily to be compiled
* on Windows Khronos Vulkan is needed in order to avoid error messages [Vulkan](https://vulkan.lunarg.com/sdk/home)
* C++ package Qt 6.2 or later
* OpenGL and Sql libraries from Qt

# License

GPL v3<br>
(this is due to some underlying libraries, not decided by me).

