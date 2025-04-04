cmake_minimum_required(VERSION 3.10)
project(lua-compiler)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找LLVM包
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# 获取LLVM的CXXFLAGS
execute_process(
    COMMAND ${LLVM_TOOLS_BINARY_DIR}/llvm-config --cxxflags
    OUTPUT_VARIABLE LLVM_CXXFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# 获取LLVM的LDFLAGS
execute_process(
    COMMAND ${LLVM_TOOLS_BINARY_DIR}/llvm-config --ldflags
    OUTPUT_VARIABLE LLVM_LDFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# 设置编译标志，移除-fno-exceptions并添加调试信息
string(REPLACE "-fno-exceptions" "" LLVM_CXXFLAGS "${LLVM_CXXFLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LLVM_CXXFLAGS} -g")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LLVM_LDFLAGS}")

# 添加LLVM的头文件路径和定义
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# 添加项目的头文件路径
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${LLVM_INCLUDE_DIRS}
)

# 添加源文件
add_executable(luac
    src/main.cpp
    src/Lexer.cpp
    src/Parser.cpp
    src/CodeGen.cpp
    src/AST.cpp
)

# 获取LLVM库路径
execute_process(
    COMMAND ${LLVM_TOOLS_BINARY_DIR}/llvm-config --libfiles
    OUTPUT_VARIABLE LLVM_LIBRARIES
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# 转换库路径为列表
string(REPLACE " " ";" LLVM_LIBRARIES_LIST "${LLVM_LIBRARIES}")

# 链接LLVM库
target_link_libraries(luac PRIVATE ${LLVM_LIBRARIES_LIST})

# 设置输出目录
set_target_properties(luac PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 打印调试信息
message(STATUS "Project source dir: ${CMAKE_SOURCE_DIR}")
message(STATUS "LLVM include dirs: ${LLVM_INCLUDE_DIRS}")
message(STATUS "LLVM definitions: ${LLVM_DEFINITIONS}")
message(STATUS "LLVM libraries: ${LLVM_LIBRARIES}")
message(STATUS "LLVM CXXFLAGS: ${LLVM_CXXFLAGS}")
message(STATUS "LLVM LDFLAGS: ${LLVM_LDFLAGS}") 