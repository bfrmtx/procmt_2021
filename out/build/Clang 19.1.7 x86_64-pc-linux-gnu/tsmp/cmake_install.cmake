# Install script for directory: /home/bfr/devel/github_procmt_2021/procmt_2021/tsmp

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/bin/tsmp" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/bin/tsmp")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/bin/tsmp"
         RPATH "\$ORIGIN/../lib:/usr/local/procmt/lib:\$ORIGIN")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/bin/tsmp")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/bin" TYPE EXECUTABLE FILES "/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/tsmp/tsmp")
  if(EXISTS "$ENV{DESTDIR}/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/bin/tsmp" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/bin/tsmp")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/bin/tsmp"
         OLD_RPATH "::::::::::::::::::::::::::::::::::::::::::::"
         NEW_RPATH "\$ORIGIN/../lib:/usr/local/procmt/lib:\$ORIGIN")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/llvm-strip" "$ENV{DESTDIR}/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/bin/tsmp")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  include("/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/tsmp/CMakeFiles/tsmp.dir/install-cxx-module-bmi-Debug.cmake" OPTIONAL)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/doc/tsmp.pdf")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/doc" TYPE FILE FILES "/home/bfr/devel/github_procmt_2021/procmt_2021/tsmp/tsmp.pdf")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/tsmp/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
