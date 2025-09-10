#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Visitor.h"

// 二元操作符
enum class BinaryOp {
    ADD, SUB, MUL, DIV, MOD,
    EQ_OP, NEQ_OP, LT, LT_EQ, GT, GT_EQ,
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
    virtual ~LocalVarDecl() = default;
    
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

// true表达式
class TrueExpr : public Expr {
public:
    void accept(Visitor& visitor) override {
        visitor.visit(this);
    }
};

// false表达式
class FalseExpr : public Expr {
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
    std::unique_ptr<Expr> calleeExpr;
public:
    CallExpr(const std::string& c, std::vector<std::unique_ptr<Expr>> args)
        : callee(c), arguments(std::move(args)) {}
    CallExpr(std::unique_ptr<Expr> calleeExpr, std::vector<std::unique_ptr<Expr>> args)
        : callee(""), arguments(std::move(args)), calleeExpr(std::move(calleeExpr)) {}
    
    const std::string& getCallee() const { return callee; }
    const std::vector<std::unique_ptr<Expr>>& getArguments() const { return arguments; }
    Expr* getCalleeExpr() const { return calleeExpr.get(); }
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
};// for循环语句
class ForStmt : public Stmt {
    std::string varName;
    std::unique_ptr<Expr> start;
    std::unique_ptr<Expr> end;
    std::unique_ptr<Stmt> body;
public:
    ForStmt(const std::string& var, std::unique_ptr<Expr> s, std::unique_ptr<Expr> e, std::unique_ptr<Stmt> b)
        : varName(var), start(std::move(s)), end(std::move(e)), body(std::move(b)) {}
    
    const std::string& getVarName() const { return varName; }
    Expr* getStart() const { return start.get(); }
    Expr* getEnd() const { return end.get(); }
    Stmt* getBody() const { return body.get(); }
    void accept(Visitor& visitor) override {
        visitor.visit(this);
    }
};


// 数组访问表达式
class ArrayAccessExpr : public Expr {
    std::unique_ptr<Expr> array;
    std::unique_ptr<Expr> index;
public:
    ArrayAccessExpr(std::unique_ptr<Expr> arr, std::unique_ptr<Expr> idx)
        : array(std::move(arr)), index(std::move(idx)) {}

    Expr* getArray() const { return array.get(); }
    Expr* getIndex() const { return index.get(); }

    void accept(Visitor& visitor) override {
        visitor.visit(this);
    }
};

// 变量赋值语句
class AssignmentStmt : public Stmt {
    std::string varName;
    std::unique_ptr<Expr> value;
public:
    AssignmentStmt(const std::string& name, std::unique_ptr<Expr> val)
        : varName(name), value(std::move(val)) {}
    
    const std::string& getVarName() const { return varName; }
    Expr* getValue() const { return value.get(); }
    void accept(Visitor& visitor) override;
};


// 数组赋值语句  
class ArrayAssignmentStmt : public Stmt {
    std::unique_ptr<Expr> array;
    std::unique_ptr<Expr> index;
    std::unique_ptr<Expr> value;
public:
    ArrayAssignmentStmt(std::unique_ptr<Expr> arr, std::unique_ptr<Expr> idx, std::unique_ptr<Expr> val)
        : array(std::move(arr)), index(std::move(idx)), value(std::move(val)) {}
    
    Expr* getArray() const { return array.get(); }
    Expr* getIndex() const { return index.get(); }
    Expr* getValue() const { return value.get(); }
    void accept(Visitor& visitor) override;
};


// 字段访问表达式  
class FieldAccessExpr : public Expr {
    std::unique_ptr<Expr> object;
    std::string fieldName;
public:
    FieldAccessExpr(std::unique_ptr<Expr> obj, const std::string& field)
        : object(std::move(obj)), fieldName(field) {}
    
    Expr* getObject() const { return object.get(); }
    const std::string& getFieldName() const { return fieldName; }
    void accept(Visitor& visitor) override;
};


// 数组字面量表达式  
class ArrayLiteralExpr : public Expr {
    std::vector<std::unique_ptr<Expr>> elements;
public:
    ArrayLiteralExpr(std::vector<std::unique_ptr<Expr>> elems)
        : elements(std::move(elems)) {}
    
    const std::vector<std::unique_ptr<Expr>>& getElements() const { return elements; }
    void accept(Visitor& visitor) override;
};

