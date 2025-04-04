#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"
#include <llvm/Support/raw_ostream.h>
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.lua>\n";
        return 1;
    }

    // 读取源文件
    std::string source;
    {
        std::ifstream file(argv[1]);
        if (!file) {
            std::cerr << "Error: Cannot open input file\n";
            return 1;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        source = buffer.str();
    }
    
    std::cout << "Source code: '" << source << "'" << std::endl;
    
    // 创建词法分析器
    Lexer lexer(source);
    
    // 打印所有token
    std::cout << "\nTokenizing..." << std::endl;
    Token token;
    do {
        token = lexer.getNextToken();
        std::cout << "Token: type=" << token.type 
                  << ", value='" << token.value 
                  << "', line=" << token.line 
                  << ", column=" << token.column << std::endl;
    } while (token.type != TOKEN_EOF);
    
    // 重置词法分析器
    lexer = Lexer(source);
    
    // 创建语法分析器
    Parser parser(lexer);
    
    try {
        auto ast = parser.parse();
        if (!ast) {
            std::cerr << "Error: Failed to parse input\n";
            return 1;
        }
        
        CodeGenerator codegen;
        codegen.generateCode(ast.get());
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 