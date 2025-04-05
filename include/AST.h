#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Visitor.h"

// 二元操作符
enum class BinaryOp {
    ADD, SUB, MUL, DIV, MOD,
    EQ, NEQ, LT, LT_EQ, GT, GT_EQ,
    AND_OP, OR_OP, CONCAT
};

// 一元操作符
enum class UnaryOp {
    NEG, NOT_OP, LEN
};

// AST 基类
class Node {
public:
    virtual ~Node() = default;
};

// 表达式基类
class Expr : public Node {
public:
    virtual ~Expr() = default;
    virtual void accept(Visitor& visitor) = 0;
};

// 语句基类
class Stmt : public Node {
public:
    virtual ~Stmt() = default;
    virtual void accept(Visitor& visitor) = 0;
};

// 块语句
class BlockStmt : public Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;
public:
    BlockStmt(std::vector<std::unique_ptr<Stmt>> stmts)
        : statements(std::move(stmts)) {}
    
    const std::vector<std::unique_ptr<Stmt>>& getStatements() const { return statements; }
    void accept(Visitor& visitor) override { visitor.visit(this); }
};

// 二元表达式
class BinaryExpr : public Expr {
    std::unique_ptr<Expr> left;
    BinaryOp op;
    std::unique_ptr<Expr> right;
public:
    BinaryExpr(BinaryOp o, std::unique_ptr<Expr> l, std::unique_ptr<Expr> r)
        : op(o), left(std::move(l)), right(std::move(r)) {}
    
    Expr* getLeft() { return left.get(); }
    Expr* getRight() { return right.get(); }
    BinaryOp getOp() const { return op; }
    void accept(Visitor& visitor) override {
        visitor.visit(this);
    }
};

// 数字表达式
class NumberExpr : public Expr {
private:
    double value;
public:
    NumberExpr(double v) : value(v) {}
    void accept(Visitor& visitor) override {
        visitor.visit(this);
    }
    double getValue() const { return value; }
};

// 打印表达式
class PrintExpr : public Stmt {
    std::unique_ptr<Expr> expr;
public:
    PrintExpr(std::unique_ptr<Expr> e) : expr(std::move(e)) {}
    Expr* getExpr() { return expr.get(); }
    void accept(Visitor& visitor) override {
        visitor.visit(this);
    }
};

// if语句
class IfStmt : public Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;
public:
    IfStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> then, std::unique_ptr<Stmt> els = nullptr)
        : condition(std::move(cond))
        , thenBranch(std::move(then))
        , elseBranch(std::move(els)) {}
    
    Expr* getCondition() { return condition.get(); }
    Stmt* getThenBranch() { return thenBranch.get(); }
    Stmt* getElseBranch() { return elseBranch.get(); }
    void accept(Visitor& visitor) override {
        visitor.visit(this);
    }
};

// while语句
class WhileStmt : public Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;
public:
    WhileStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> b)
        : condition(std::move(cond)), body(std::move(b)) {}
    
    Expr* getCondition() const { return condition.get(); }
    Stmt* getBody() const { return body.get(); }
    void accept(Visitor& visitor) override;
};

// repeat语句
class RepeatStmt : public Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;
public:
    RepeatStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> b)
        : condition(std::move(cond)), body(std::move(b)) {}
    
    Expr* getCondition() const { return condition.get(); }
    Stmt* getBody() const { return body.get(); }
    void accept(Visitor& visitor) override;
};

// 函数声明
class FunctionDecl : public Stmt {
    std::string name;
    std::vector<std::string> params;
    std::vector<std::unique_ptr<Stmt>> body;
public:
    FunctionDecl(const std::string& n, 
                std::vector<std::string> p,
                std::vector<std::unique_ptr<Stmt>> b)
        : name(n), params(std::move(p)), body(std::move(b)) {}
    
    const std::string& getName() const { return name; }
    const std::vector<std::string>& getParams() const { return params; }
    const std::vector<std::unique_ptr<Stmt>>& getBody() const { return body; }
    void accept(Visitor& visitor) override;
};

// return语句
class ReturnStmt : public Stmt {
    std::vector<std::unique_ptr<Expr>> values;
public:
    ReturnStmt(std::vector<std::unique_ptr<Expr>> v) : values(std::move(v)) {}
    const std::vector<std::unique_ptr<Expr>>& getValues() const { return values; }
    void accept(Visitor& visitor) override;
};

// 局部变量声明
class LocalVarDecl : public Stmt {
    std::string name;
    std::unique_ptr<Expr> initializer;
public:
    LocalVarDecl(const std::string& n, std::unique_ptr<Expr> init = nullptr)
        : name(n), initializer(std::move(init)) {}
    
    const std::string& getName() const { return name; }
    Expr* getInitializer() const { return initializer.get(); }
    void accept(Visitor& visitor) override;
};

// 字符串表达式
class StringExpr : public Expr {
private:
    std::string value;
public:
    StringExpr(const std::string& v) : value(v) {}
    void accept(Visitor& visitor) override {
        visitor.visit(this);
    }
    const std::string& getValue() const { return value; }
};

// nil表达式
class NilExpr : public Expr {
public:
    void accept(Visitor& visitor) override {
        visitor.visit(this);
    }
};

// 一元表达式
class UnaryExpr : public Expr {
    UnaryOp op;
    std::unique_ptr<Expr> expr;
public:
    UnaryExpr(UnaryOp o, std::unique_ptr<Expr> e)
        : op(o), expr(std::move(e)) {}
    
    UnaryOp getOp() const { return op; }
    Expr* getExpr() const { return expr.get(); }
    void accept(Visitor& visitor) override {
        visitor.visit(this);
    }
};

// 表达式语句
class ExprStmt : public Stmt {
    std::unique_ptr<Expr> expr;
public:
    ExprStmt(std::unique_ptr<Expr> e) : expr(std::move(e)) {}
    Expr* getExpr() { return expr.get(); }
    void accept(Visitor& visitor) override;
};

class CallExpr : public Expr {
    std::string callee;
    std::vector<std::unique_ptr<Expr>> arguments;
public:
    CallExpr(const std::string& c, std::vector<std::unique_ptr<Expr>> args)
        : callee(c), arguments(std::move(args)) {}
    
    const std::string& getCallee() const { return callee; }
    const std::vector<std::unique_ptr<Expr>>& getArguments() const { return arguments; }
    void accept(Visitor& visitor) override {
        visitor.visit(this);
    }
};

class VarExpr : public Expr {
    std::string name;
public:
    VarExpr(const std::string& name) : name(name) {}
    
    const std::string& getName() const { return name; }
    
    void accept(Visitor& visitor) override {
        visitor.visit(this);
    }
}; 