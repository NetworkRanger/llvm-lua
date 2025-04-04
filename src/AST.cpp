#include "AST.h"

// NumberExpr实现
void NumberExpr::accept(ASTVisitor& visitor) {
    visitor.visitNumberExpr(this);
}

// BinaryExpr实现
void BinaryExpr::accept(ASTVisitor& visitor) {
    visitor.visitBinaryExpr(this);
}

// UnaryExpr实现
void UnaryExpr::accept(ASTVisitor& visitor) {
    visitor.visitUnaryExpr(this);
}

// StringExpr实现
void StringExpr::accept(ASTVisitor& visitor) {
    visitor.visitStringExpr(this);
}

// NilExpr实现
void NilExpr::accept(ASTVisitor& visitor) {
    visitor.visitNilExpr(this);
}

// ExprStmt实现
void ExprStmt::accept(ASTVisitor& visitor) {
    visitor.visitExprStmt(this);
}

// PrintExpr实现
void PrintExpr::accept(ASTVisitor& visitor) {
    visitor.visitPrintExpr(this);
}

// IfStmt实现
void IfStmt::accept(ASTVisitor& visitor) {
    visitor.visitIfStmt(this);
}

// WhileStmt实现
void WhileStmt::accept(ASTVisitor& visitor) {
    visitor.visitWhileStmt(this);
}

// RepeatStmt实现
void RepeatStmt::accept(ASTVisitor& visitor) {
    visitor.visitRepeatStmt(this);
}

// ReturnStmt实现
void ReturnStmt::accept(ASTVisitor& visitor) {
    visitor.visitReturnStmt(this);
}

// LocalVarDecl实现
void LocalVarDecl::accept(ASTVisitor& visitor) {
    visitor.visitLocalVarDecl(this);
}

// BlockStmt实现
void BlockStmt::accept(ASTVisitor& visitor) {
    visitor.visitBlockStmt(this);
}

// FunctionDecl实现
void FunctionDecl::accept(ASTVisitor& visitor) {
    visitor.visitFunctionDecl(this);
}

// CallExpr实现
void CallExpr::accept(ASTVisitor& visitor) {
    visitor.visitCallExpr(this);
}

// VarExpr实现
void VarExpr::accept(ASTVisitor& visitor) {
    visitor.visitVarExpr(this);
}

