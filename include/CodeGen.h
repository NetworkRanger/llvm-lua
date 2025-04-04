#pragma once
#include "AST.h"
#include "ASTVisitor.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <string>

class CodeGenerator : public ASTVisitor {
public:
    CodeGenerator();
    void generateCode(ASTNode* root);
    bool generateObjectFile(const std::string& filename);
    
private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;
    llvm::Value* lastValue;
    llvm::Function* printfFunc;
    
    void visitNumberExpr(NumberExpr* node) override;
    void visitBinaryExpr(BinaryExpr* node) override;
    void visitPrintExpr(PrintExpr* node) override;
    void declarePrintf();
}; 