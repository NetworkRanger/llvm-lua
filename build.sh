#!/bin/bash

# 清理之前的构建
./clean.sh

# 创建并进入build目录
mkdir -p build
cd build

# 根据系统类型运行cmake
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS系统
    if [ -d "/opt/homebrew/opt/llvm" ]; then
        # Apple Silicon Mac
        cmake -DLLVM_DIR=/opt/homebrew/opt/llvm/lib/cmake/llvm ..
    elif [ -d "/usr/local/opt/llvm" ]; then
        # Intel Mac
        cmake -DLLVM_DIR=/usr/local/opt/llvm/lib/cmake/llvm ..
    else
        echo "Error: LLVM not found!"
        exit 1
    fi
else
    # Linux系统
    cmake ..
fi

# 执行编译
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)

# 检查编译结果
if [ $? -eq 0 ]; then
    echo "编译成功！"
    
    # 创建lua目录（如果不存在）
    cd ..
    mkdir -p lua
    
    # 拷贝luac到lua目录
    cp build/bin/luac lua/
    
    # 设置可执行权限
    chmod +x lua/luac
    
    echo "可执行文件已拷贝到: $(pwd)/lua/luac"
else
    echo "编译失败！"
    exit 1
fi 