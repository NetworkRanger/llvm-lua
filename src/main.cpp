#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.lua>" << std::endl;
        return 1;
    }

    try {
        // 读取源文件
        std::string source = readFile(argv[1]);
        
        // 创建词法分析器
        Lexer lexer(source);
        
        // 创建语法分析器
        Parser parser(lexer);
        
        // 解析代码生成AST
        auto ast = parser.parse();
        
        // 创建代码生成器
        CodeGenerator codegen;
        
        // 生成LLVM IR
        codegen.generateCode(ast.get());
        
        std::cout << "Compilation successful!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 