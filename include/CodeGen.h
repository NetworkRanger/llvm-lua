#ifndef CODEGEN_H
#define CODEGEN_H

#include "AST.h"
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <memory>
#include <string>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>

class CodeGenerator : public ASTVisitor {
private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    llvm::Value* lastValue;
    llvm::Function* printfFunc;
    llvm::Function* currentFunction;  // 当前正在处理的函数
    std::map<std::string, llvm::AllocaInst*> namedValues;  // 变量名到alloca的映射
    
    bool hasMultipleReturns(FunctionDecl* node);
    
    void declarePrintf();
    
    llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* function,
                                            const std::string& varName);
    
    void collectFunctionDeclarations(Stmt* node);
    
    void initBuiltins();
    
public:
    CodeGenerator();
    void generateCode(Stmt* root);
    void saveModuleToFile(const std::string& filename);
    void executeCode();
    
    // 实现所有ASTVisitor接口
    void visitNumberExpr(NumberExpr* node) override;
    void visitBinaryExpr(BinaryExpr* node) override;
    void visitUnaryExpr(UnaryExpr* node) override;
    void visitPrintExpr(PrintExpr* node) override;
    void visitExprStmt(ExprStmt* node) override;
    void visitIfStmt(IfStmt* node) override;
    void visitWhileStmt(WhileStmt* node) override;
    void visitRepeatStmt(RepeatStmt* node) override;
    void visitFunctionDecl(FunctionDecl* node) override;
    void visitReturnStmt(ReturnStmt* node) override;
    void visitLocalVarDecl(LocalVarDecl* node) override;
    void visitStringExpr(StringExpr* node) override;
    void visitNilExpr(NilExpr* node) override;
    void visitCallExpr(CallExpr* expr) override;
    void visitVarExpr(VarExpr* expr) override;
    void visitBlockStmt(BlockStmt* stmt) override;
};

#endif // CODEGEN_H 