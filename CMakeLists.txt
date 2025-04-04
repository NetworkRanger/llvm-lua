cmake_minimum_required(VERSION 3.10)
project(lua_compiler)

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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LLVM_CXXFLAGS} -g -O0 -fno-inline")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -fno-inline")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LLVM_LDFLAGS}")

# 添加LLVM的头文件路径和定义
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS} -DLLVM_DISABLE_OPTIMIZATIONS)

# 添加项目的头文件路径
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${LLVM_INCLUDE_DIRS}
)

# 添加源文件
set(SOURCES
    src/main.cpp
    src/Lexer.cpp
    src/Parser.cpp
    src/AST.cpp
    src/CodeGen.cpp
)

# 添加可执行文件
add_executable(luac ${SOURCES})

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

# 链接LLVM库
llvm_map_components_to_libnames(llvm_libs
    core
    support
    native
    mcjit
    x86codegen  # 或其他目标架构
    interpreter
)

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

# 链接LLVM库
llvm_map_components_to_libnames(llvm_libs
    core
    support
    native
    mcjit
    x86codegen  # 或其他目标架构
    interpreter
)

target_link_libraries(luac PRIVATE ${llvm_libs})

# 添加LLVM的定义
add_definitions(
    ${LLVM_DEFINITIONS}
    -DLLVM_DISABLE_ABI_BREAKING_CHECKS_ENFORCING
    -DNDEBUG
    -DLLVM_DISABLE_OPTIMIZATIONS
)

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
    ipo  # 添加这个组件
) 