#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"
#include <llvm/Support/raw_ostream.h>
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        llvm::errs() << "Usage: " << argv[0] << " <input.lua>\n";
        return 1;
    }

    // 读取源文件
    std::string source;
    {
        std::ifstream file(argv[1]);
        if (!file) {
            llvm::errs() << "Error: Cannot open input file\n";
            return 1;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        source = buffer.str();
    }
    
    // 创建词法分析器
    Lexer lexer(source);
    
    // 创建语法分析器
    Parser parser(lexer);
    
    // 解析代码生成AST
    auto ast = parser.parse();
    if (!ast) {
        llvm::errs() << "Error: Failed to parse input\n";
        return 1;
    }
    
    // 创建代码生成器
    CodeGenerator codegen;
    
    // 生成LLVM IR
    codegen.generateCode(ast.get());
    
    // 生成目标文件
    if (!codegen.generateObjectFile("output.o")) {
        llvm::errs() << "Error: Failed to generate object file\n";
        return 1;
    }
    
    llvm::errs() << "Compilation successful!\n";
    return 0;
} 