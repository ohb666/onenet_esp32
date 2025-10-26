# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/esp32-idf-ahy/5.1.2/frameworks/esp-idf-v5.1.2/components/bootloader/subproject"
  "D:/esp-code/sample_project/build/bootloader"
  "D:/esp-code/sample_project/build/bootloader-prefix"
  "D:/esp-code/sample_project/build/bootloader-prefix/tmp"
  "D:/esp-code/sample_project/build/bootloader-prefix/src/bootloader-stamp"
  "D:/esp-code/sample_project/build/bootloader-prefix/src"
  "D:/esp-code/sample_project/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/esp-code/sample_project/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/esp-code/sample_project/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
