cmake_minimum_required(VERSION 3.10)
project(llvm-lua)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置 CMake 策略
cmake_policy(SET CMP0075 NEW)

# 查找 LLVM 包
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# 查找 Flex 和 Bison
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

# 生成词法分析器和语法分析器
FLEX_TARGET(Lexer src/lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
BISON_TARGET(Parser src/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
             DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.h)
ADD_FLEX_BISON_DEPENDENCY(Lexer Parser)

# 包含目录
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 添加定义
add_definitions(${LLVM_DEFINITIONS})
add_definitions(
    -DNDEBUG
    -DLLVM_DISABLE_OPTIMIZATIONS
    -DLLVM_DISABLE_ABI_BREAKING_CHECKS_ENFORCING
)

# 源文件
set(SOURCES
    src/main.cpp
    src/AST.cpp
    src/CodeGen.cpp
    ${FLEX_Lexer_OUTPUTS}
    ${BISON_Parser_OUTPUTS}
)

# 创建可执行文件
add_executable(luac ${SOURCES})

# 获取本地目标架构的LLVM组件
execute_process(
    COMMAND ${LLVM_TOOLS_BINARY_DIR}/llvm-config --host-target
    OUTPUT_VARIABLE LLVM_HOST_TARGET
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# 根据目标架构设置组件
if(LLVM_HOST_TARGET MATCHES ".*aarch64.*")
    set(LLVM_TARGET_COMPONENTS AArch64)
elseif(LLVM_HOST_TARGET MATCHES ".*x86_64.*")
    set(LLVM_TARGET_COMPONENTS X86)
endif()

# 确保链接正确的LLVM组件
llvm_map_components_to_libnames(llvm_libs
    core
    support
    native
    mcjit
    ${LLVM_TARGET_COMPONENTS}
    interpreter
    analysis
    bitwriter
    codegen
    ipo
)

# 链接库
target_link_libraries(luac PRIVATE 
    ${llvm_libs}
    c
)

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