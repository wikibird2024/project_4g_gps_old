# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/xk/esp/esp-idf/components/bootloader/subproject"
  "/home/xk/esp/project_4g_old/build/bootloader"
  "/home/xk/esp/project_4g_old/build/bootloader-prefix"
  "/home/xk/esp/project_4g_old/build/bootloader-prefix/tmp"
  "/home/xk/esp/project_4g_old/build/bootloader-prefix/src/bootloader-stamp"
  "/home/xk/esp/project_4g_old/build/bootloader-prefix/src"
  "/home/xk/esp/project_4g_old/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/xk/esp/project_4g_old/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/xk/esp/project_4g_old/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
