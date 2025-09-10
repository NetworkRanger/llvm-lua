//
// Created by suse on 2025/9/4.
//


#include <iostream>
#include "AST.h"
// #include "LuaWriter.h"


extern int yylex();
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

        // LuaWriter luaWriter;
        // luaWriter.write(root.get());

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}