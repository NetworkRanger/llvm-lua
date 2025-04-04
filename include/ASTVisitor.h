#pragma once
#include "AST.h"

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visitNumberExpr(NumberExpr* node) = 0;
    virtual void visitBinaryExpr(BinaryExpr* node) = 0;
    virtual void visitPrintExpr(PrintExpr* node) = 0;
}; 