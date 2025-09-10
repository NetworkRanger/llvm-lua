#pragma once

// 前向声明所有 AST 节点类
class BlockStmt;
class FunctionDecl;
class ReturnStmt;
class ForStmt;
class IfStmt;
class WhileStmt;
class RepeatStmt;
class ExprStmt;
class BinaryExpr;
class UnaryExpr;
class NumberExpr;
class StringExpr;
class NilExpr;
class TrueExpr;
class FalseExpr;
class VarExpr;
class CallExpr;
class AssignmentStmt;
class ArrayAssignmentStmt;class PrintExpr;
class LocalVarDecl;
class ArrayAccessExpr;
class FieldAccessExpr;
class ArrayLiteralExpr;// 访问者基类
class Visitor {
public:
    virtual ~Visitor() = default;
    
    // 所有可能的访问方法
    virtual void visit(BlockStmt* node) = 0;
    virtual void visit(FunctionDecl* node) = 0;
    virtual void visit(ReturnStmt* node) = 0;
    virtual void visit(ForStmt* node) = 0;
    virtual void visit(IfStmt* node) = 0;
    virtual void visit(WhileStmt* node) = 0;
    virtual void visit(RepeatStmt* node) = 0;
    virtual void visit(ExprStmt* node) = 0;
    virtual void visit(BinaryExpr* node) = 0;
    virtual void visit(UnaryExpr* node) = 0;
    virtual void visit(NumberExpr* node) = 0;
    virtual void visit(StringExpr* node) = 0;
    virtual void visit(NilExpr* node) = 0;
    virtual void visit(TrueExpr* node) = 0;
    virtual void visit(FalseExpr* node) = 0;
    virtual void visit(VarExpr* node) = 0;
    virtual void visit(CallExpr* node) = 0;
    virtual void visit(PrintExpr* node) = 0;
    virtual void visit(LocalVarDecl* node) = 0;
    virtual void visit(AssignmentStmt* node) = 0;
    virtual void visit(ArrayAssignmentStmt* node) = 0;
    virtual void visit(FieldAccessExpr* node) = 0;
    virtual void visit(ArrayAccessExpr* node) = 0;
    virtual void visit(ArrayLiteralExpr* node) = 0;
};
