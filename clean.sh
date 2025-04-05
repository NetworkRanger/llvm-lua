#!/bin/bash

# 删除build目录
rm -rf build/
rm -rf .cmake/
rm -rf cmake-build-default-event-trace/

# 删除CMake生成的文件
rm -f CMakeCache.txt
rm -rf CMakeFiles/
rm -f cmake_install.cmake
rm -f Makefile

# 删除编译产生的二进制文件
rm -rf bin/

# 删除其他可能的临时文件
rm -f *.o
rm -f *.a
rm -f *.so
rm -f *.dylib


echo "清理完成！" 