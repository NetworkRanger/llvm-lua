cmake_minimum_required(VERSION 3.10)
project(lua-compiler)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找LLVM包
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# 获取所有需要的LLVM组件
llvm_map_components_to_libnames(llvm_libs
    core
    support
    irreader
    analysis
    bitwriter
    mcjit
    native
    option
    passes
    x86codegen
    x86asmparser
    x86info
)

# 添加LLVM的头文件路径和定义
include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# 添加项目的头文件路径
include_directories(include)

# 添加源文件
add_executable(luac
    src/main.cpp
    src/Lexer.cpp
    src/Parser.cpp
    src/CodeGen.cpp
    src/AST.cpp
)

# 链接LLVM库
target_link_libraries(luac PRIVATE ${llvm_libs})

# 设置输出目录
set_target_properties(luac PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
) 