# llvm-lua
使用llvm编写lua编译器

## 构建

MacOS:

```bash
cmake -DLLVM_DIR=/opt/homebrew/opt/llvm/lib/cmake/llvm -DCMAKE_BUILD_TYPE=Release ..
```

Ubuntu:

```bash
cmake -DLLVM_DIR=/usr/lib/llvm-11/cmake/ ..
```