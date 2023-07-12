# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/learn/ESPIDF/Espressif/frameworks/esp-idf-v4.4.5/components/bootloader/subproject"
  "D:/learn/ESPIDF/Espressif/frameworks/KTVXL/build/bootloader"
  "D:/learn/ESPIDF/Espressif/frameworks/KTVXL/build/bootloader-prefix"
  "D:/learn/ESPIDF/Espressif/frameworks/KTVXL/build/bootloader-prefix/tmp"
  "D:/learn/ESPIDF/Espressif/frameworks/KTVXL/build/bootloader-prefix/src/bootloader-stamp"
  "D:/learn/ESPIDF/Espressif/frameworks/KTVXL/build/bootloader-prefix/src"
  "D:/learn/ESPIDF/Espressif/frameworks/KTVXL/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/learn/ESPIDF/Espressif/frameworks/KTVXL/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
