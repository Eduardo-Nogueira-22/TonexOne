# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/IDF/v5.3.1/esp-idf/components/bootloader/subproject"
  "C:/Users/Eduardo Nogueira/Tonex/build/bootloader"
  "C:/Users/Eduardo Nogueira/Tonex/build/bootloader-prefix"
  "C:/Users/Eduardo Nogueira/Tonex/build/bootloader-prefix/tmp"
  "C:/Users/Eduardo Nogueira/Tonex/build/bootloader-prefix/src/bootloader-stamp"
  "C:/Users/Eduardo Nogueira/Tonex/build/bootloader-prefix/src"
  "C:/Users/Eduardo Nogueira/Tonex/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/Eduardo Nogueira/Tonex/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/Eduardo Nogueira/Tonex/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
