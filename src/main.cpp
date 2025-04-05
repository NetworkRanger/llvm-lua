#include <iostream>
#include <fstream>
#include <sstream>
#include "CodeGen.h"

extern int yyparse();
extern FILE* yyin;
extern std::unique_ptr<BlockStmt> root;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.lua>" << std::endl;
        return 1;
    }

    // 打开输入文件
    FILE* input = fopen(argv[1], "r");
    if (!input) {
        std::cerr << "Error: Could not open input file: " << argv[1] << std::endl;
        return 1;
    }
    yyin = input;

    try {
        // 解析输入文件
        if (yyparse() != 0) {
            std::cerr << "Error: Parsing failed" << std::endl;
            return 1;
        }

        // 生成代码
        CodeGenerator codegen;
        codegen.generateCode(root.get());

        // 获取输入文件的目录
        std::string inputPath(argv[1]);
        size_t lastSlash = inputPath.find_last_of("/\\");
        std::string outputPath;
        if (lastSlash != std::string::npos) {
            outputPath = inputPath.substr(0, lastSlash + 1) + "output.ll";
        } else {
            outputPath = "output.ll";
        }

        // 保存生成的代码
        codegen.saveModuleToFile(outputPath);
        std::cout << "Successfully generated LLVM IR: " << outputPath << std::endl;

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
} 