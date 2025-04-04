#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include "Lexer.h"
#include <memory>

// 先声明所有需要的类
class Expr;
class Stmt;
class NumberExpr;
class BinaryExpr;
class UnaryExpr;
class PrintExpr;
class ExprStmt;
class IfStmt;
class WhileStmt;
class RepeatStmt;
class FunctionDecl;
class ReturnStmt;
class LocalVarDecl;
class StringExpr;
class NilExpr;
class CallExpr;
class VarExpr;
class BlockStmt;

// 将 ASTVisitor 的定义移到前面
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visitNumberExpr(NumberExpr* expr) = 0;
    virtual void visitBinaryExpr(BinaryExpr* expr) = 0;
    virtual void visitUnaryExpr(UnaryExpr* expr) = 0;
    virtual void visitStringExpr(StringExpr* expr) = 0;
    virtual void visitNilExpr(NilExpr* expr) = 0;
    virtual void visitExprStmt(ExprStmt* stmt) = 0;
    virtual void visitPrintExpr(PrintExpr* expr) = 0;
    virtual void visitIfStmt(IfStmt* stmt) = 0;
    virtual void visitWhileStmt(WhileStmt* stmt) = 0;
    virtual void visitRepeatStmt(RepeatStmt* stmt) = 0;
    virtual void visitReturnStmt(ReturnStmt* stmt) = 0;
    virtual void visitLocalVarDecl(LocalVarDecl* stmt) = 0;
    virtual void visitBlockStmt(BlockStmt* stmt) = 0;
    virtual void visitFunctionDecl(FunctionDecl* stmt) = 0;
    virtual void visitCallExpr(CallExpr* expr) = 0;
    virtual void visitVarExpr(VarExpr* expr) = 0;
};

// 表达式基类
class Expr {
public:
    virtual ~Expr() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
};

// 语句基类
class Stmt {
public:
    virtual ~Stmt() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
};

// 数字表达式
class NumberExpr : public Expr {
private:
    double value;
public:
    NumberExpr(double v) : value(v) {}
    void accept(ASTVisitor& visitor) override;
    double getValue() const { return value; }
};

// 二元表达式
class BinaryExpr : public Expr {
    std::unique_ptr<Expr> left;
    TokenType op;
    std::unique_ptr<Expr> right;
public:
    BinaryExpr(std::unique_ptr<Expr> l, TokenType o, std::unique_ptr<Expr> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}
    
    Expr* getLeft() const { return left.get(); }
    TokenType getOp() const { return op; }
    Expr* getRight() const { return right.get(); }
    void accept(ASTVisitor& visitor) override;
};

// 打印表达式
class PrintExpr : public Stmt {
    std::unique_ptr<Expr> expr;
public:
    PrintExpr(std::unique_ptr<Expr> e) : expr(std::move(e)) {}
    Expr* getExpr() const { return expr.get(); }
    void accept(ASTVisitor& visitor) override;
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
    
    Expr* getCondition() const { return condition.get(); }
    Stmt* getThenBranch() const { return thenBranch.get(); }
    Stmt* getElseBranch() const { return elseBranch.get(); }
    void accept(ASTVisitor& visitor) override;
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
    void accept(ASTVisitor& visitor) override;
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
    void accept(ASTVisitor& visitor) override;
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
    void accept(ASTVisitor& visitor) override;
};

// return语句
class ReturnStmt : public Stmt {
    std::vector<std::unique_ptr<Expr>> values;
public:
    ReturnStmt(std::vector<std::unique_ptr<Expr>> v = std::vector<std::unique_ptr<Expr>>())
        : values(std::move(v)) {}
    
    const std::vector<std::unique_ptr<Expr>>& getValues() const { return values; }
    void accept(ASTVisitor& visitor) override;
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
    void accept(ASTVisitor& visitor) override;
};

// 字符串表达式
class StringExpr : public Expr {
private:
    std::string value;
public:
    StringExpr(const std::string& v) : value(v) {}
    void accept(ASTVisitor& visitor) override;
    const std::string& getValue() const { return value; }
};

// nil表达式
class NilExpr : public Expr {
public:
    void accept(ASTVisitor& visitor) override;
};

// 一元表达式
class UnaryExpr : public Expr {
    TokenType op;
    std::unique_ptr<Expr> expr;
public:
    UnaryExpr(TokenType o, std::unique_ptr<Expr> e)
        : op(o), expr(std::move(e)) {}
    
    TokenType getOp() const { return op; }
    Expr* getExpr() const { return expr.get(); }
    void accept(ASTVisitor& visitor) override;
};

// 表达式语句
class ExprStmt : public Stmt {
    std::unique_ptr<Expr> expr;
public:
    ExprStmt(std::unique_ptr<Expr> e) : expr(std::move(e)) {}
    Expr* getExpr() const { return expr.get(); }
    void accept(ASTVisitor& visitor) override;
};

class CallExpr : public Expr {
    std::string callee;
    std::vector<std::unique_ptr<Expr>> arguments;
public:
    CallExpr(const std::string& c, std::vector<std::unique_ptr<Expr>> args)
        : callee(c), arguments(std::move(args)) {}
    
    const std::string& getCallee() const { return callee; }
    const std::vector<std::unique_ptr<Expr>>& getArguments() const { return arguments; }
    void accept(ASTVisitor& visitor) override;
};

class VarExpr : public Expr {
    std::string name;
public:
    VarExpr(const std::string& name) : name(name) {}
    
    const std::string& getName() const { return name; }
    
    void accept(ASTVisitor& visitor) override;
};

// 添加块语句类
class BlockStmt : public Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;
    
public:
    BlockStmt(std::vector<std::unique_ptr<Stmt>> stmts)
        : statements(std::move(stmts)) {}
    
    const std::vector<std::unique_ptr<Stmt>>& getStatements() const { return statements; }
    void accept(ASTVisitor& visitor) override;
};

#endif // AST_H 