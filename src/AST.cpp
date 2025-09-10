#include "AST.h"
#include "Visitor.h"

// ExprStmt 实现
void ExprStmt::accept(Visitor& visitor) {
	visitor.visit(this);
}

// ReturnStmt 实现
void ReturnStmt::accept(Visitor& visitor) {
	visitor.visit(this);
}

// FunctionDecl 实现
void FunctionDecl::accept(Visitor& visitor) {
	visitor.visit(this);
}

// WhileStmt 实现
void WhileStmt::accept(Visitor& visitor) {
	visitor.visit(this);
}

// RepeatStmt 实现
void RepeatStmt::accept(Visitor& visitor) {
	visitor.visit(this);
}

// LocalVarDecl 实现
void LocalVarDecl::accept(Visitor& visitor) {
	visitor.visit(this);
}

// AssignmentStmt 实现
void AssignmentStmt::accept(Visitor& visitor) {
	visitor.visit(this);
}

// ArrayAssignmentStmt 实现
void ArrayAssignmentStmt::accept(Visitor& visitor) {
	visitor.visit(this);
}

// FieldAccessExpr 实现
void FieldAccessExpr::accept(Visitor& visitor) {
	visitor.visit(this);
}

// ArrayLiteralExpr 实现
void ArrayLiteralExpr::accept(Visitor& visitor) {
	visitor.visit(this);
}
