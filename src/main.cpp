#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.lua>" << std::endl;
        return 1;
    }
    
    try {
        // 读取输入文件
        std::ifstream file(argv[1]);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open input file: " + std::string(argv[1]));
        }
        std::string source((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
        
        // 词法分析
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.scanTokens();
        
        // 语法分析
        Parser parser(tokens);
        auto ast = parser.parse();
        
        // 代码生成
        CodeGenerator codegen;
        codegen.generateCode(ast.get());
        
        // 获取输入文件的目录
        std::string inputDir = std::string(argv[1]);
        size_t lastSlash = inputDir.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            inputDir = inputDir.substr(0, lastSlash + 1);
        } else {
            inputDir = "./";
        }
        
        // 保存生成的代码到与输入文件相同目录下的 output.ll
        std::string outputFile = inputDir + "output.ll";
        codegen.saveModuleToFile(outputFile);
        
        std::cout << "Successfully generated LLVM IR: " << outputFile << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
} 