#pragma once

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <map>
#include "AST.h"

class CodeGenerator : public Visitor {
public:
    CodeGenerator();
    ~CodeGenerator();

    void generateCode(Stmt* root);
    void saveModuleToFile(const std::string& filename);
    void executeCode();

private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    llvm::Value* lastValue;
    std::map<std::string, llvm::AllocaInst*> namedValues;
    llvm::Function* currentFunction;
    llvm::Function* printfFunc;

    // 私有辅助方法
    void declarePrintf();
    void collectFunctionDeclarations(Stmt* node);
    bool hasMultipleReturns(FunctionDecl* node);
    llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* function, const std::string& name);

    // 添加辅助方法声明
    void initBuiltins();

    // 实现所有 Visitor 接口方法
    void visit(BlockStmt* node) override;
    void visit(FunctionDecl* node) override;
    void visit(ReturnStmt* node) override;
    void visit(IfStmt* node) override;
    void visit(WhileStmt* node) override;
    void visit(RepeatStmt* node) override;
    void visit(ExprStmt* node) override;
    void visit(BinaryExpr* node) override;
    void visit(UnaryExpr* node) override;
    void visit(NumberExpr* node) override;
    void visit(StringExpr* node) override;
    void visit(NilExpr* node) override;
    void visit(VarExpr* node) override;
    void visit(CallExpr* node) override;
    void visit(PrintExpr* node) override;
    void visit(LocalVarDecl* node) override;
}; 