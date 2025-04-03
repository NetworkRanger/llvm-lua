#include "AST.h"
#include "ASTVisitor.h"

void NumberExpr::accept(ASTVisitor& visitor) {
    visitor.visitNumberExpr(this);
}

void BinaryExpr::accept(ASTVisitor& visitor) {
    visitor.visitBinaryExpr(this);
} 