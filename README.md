# ProcMT 2021

ProcMT stands for **Proc**ess **M**agneto**T**elluric data.

I was not able to make a complete port of the "older" ProcMT version.
But most - and all important - parts are ported to Qt 6.5.

May 2023: changes for Qt 6.5
August 2023: edi files with ZROT and TROT
December 2023: Mac OS port.


# Prerequisites

* Linux: clang++ / gcc compiler with C++ 17 support 
* Apple clang version 12 or above
* tested with MinGW64 compiler & Ninja make, shipped with Qt 6.5
* CMake 3.20 or above (Version is upgrading fast - Cmake is tied to compilers and their features)
* C++ Boost library installed 1.6 or later (*find_package(Boost REQUIRED)*)
* the Boost libraries do not necessarily to be compiled
* Qt 6.5 or later be installed (on Mac OS: brew install qt)

## Optional
* on Windows Khronos Vulkan is needed in order to avoid error messages [Vulkan](https://vulkan.lunarg.com/sdk/home)
* C++ package Qt 6.5 or later
* OpenGL and Sql libraries from Qt

# Build

## Linux

Install cmake, boost, sqlite, qt, db-browser-for-sqlite, Qt6

## MAC OS

```bash
cd $HOME
xcode-select --install
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"~

(echo; echo 'eval "$(/opt/homebrew/bin/brew shellenv)"') >> /Users/bfr/.zprofile
eval "$(/opt/homebrew/bin/brew shellenv)"

brew update
brew install cmake boost sqlite
brew install qt 
# brew install --cask qt-creator
brew install --cask db-browser-for-sqlite
```

## Windows

... to be done ...

## START

If you are smart - you simply use my settings and do not need to edit anything. <br>
Especially if you run updates from github, you do not need edit your files again and again.

You **always** need a install directory, where the executable is copied to. <br>
**YOU** must have read/write access to this directory.

```bash
sudo mkdir -p /usr/local/procmt
# Linux:
sudo chown -R $USER:$USER /usr/local/procmt
# Mac OS:
sudo chown -R $USER:staff /usr/local/procmt
```

When later rebuilding the project, you can use the same directory again. <br>
Simply go **INTO** the directory and delete all files and subdirectories. <br>
**DO NOT** delete the directory itself.
For the professionals:

```bash
cd /usr/local/procmt
# check with pwd that you are there!
pwd
# and then delete all files and subdirectories
rm -rf *
```
otherwise use your file manager!

In case you want to save time, use this:

```bash
cd $HOME
mkdir -p $HOME/build
mkdpir -p $HOME/devel/github_procmt_2021
cd $HOME/devel/github_procmt_2021
git clone https://github.com/bfrmtx/procmt_2021.git
cd procmt_2021
zsh clang_build_release.sh
```

# License

GPL v3<br>
(this is due to some underlying libraries, not decided by me). <br>
So use it as you like, but do not sell it.
