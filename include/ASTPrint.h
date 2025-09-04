#pragma once

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <map>
#include "AST.h"

class ASTPrint : public Visitor {
public:
    ASTPrint();
    ~ASTPrint();

    void printAST(Stmt* root);

private:

    void collectFunctionDeclarations(Stmt* node);

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