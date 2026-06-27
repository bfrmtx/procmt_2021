# Install script for directory: /home/bfr/devel/github_procmt_2021/procmt_2021/include

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
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/include/doubles.h;/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/include/iterator_complex_templates.h;/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/include/iterator_templates.h;/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/include/qtx_templates.h;/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/include/statmaps.h;/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/include/threadbuffer.h;/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/include/vector_templates.h;/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/include/vector_utils.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/include" TYPE FILE FILES
    "/home/bfr/devel/github_procmt_2021/procmt_2021/include/doubles.h"
    "/home/bfr/devel/github_procmt_2021/procmt_2021/include/iterator_complex_templates.h"
    "/home/bfr/devel/github_procmt_2021/procmt_2021/include/iterator_templates.h"
    "/home/bfr/devel/github_procmt_2021/procmt_2021/include/qtx_templates.h"
    "/home/bfr/devel/github_procmt_2021/procmt_2021/include/statmaps.h"
    "/home/bfr/devel/github_procmt_2021/procmt_2021/include/threadbuffer.h"
    "/home/bfr/devel/github_procmt_2021/procmt_2021/include/vector_templates.h"
    "/home/bfr/devel/github_procmt_2021/procmt_2021/include/vector_utils.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/bin/info.sql3;/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/bin/master_calibration.sql3;/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/bin/procmt.sql3")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/bin" TYPE FILE FILES
    "/home/bfr/devel/github_procmt_2021/procmt_2021/include/info.sql3"
    "/home/bfr/devel/github_procmt_2021/procmt_2021/include/master_calibration.sql3"
    "/home/bfr/devel/github_procmt_2021/procmt_2021/include/procmt.sql3"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/bin/5chanEHnoise")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/bfr/devel/github_procmt_2021/procmt_2021/out/install/Clang 19.1.7 x86_64-pc-linux-gnu/bin" TYPE DIRECTORY FILES "/home/bfr/devel/github_procmt_2021/procmt_2021/include/5chanEHnoise")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/bfr/devel/github_procmt_2021/procmt_2021/out/build/Clang 19.1.7 x86_64-pc-linux-gnu/include/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
