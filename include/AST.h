#pragma once
#include <string>
#include <vector>
#include <memory>

class ASTVisitor;  // 添加前向声明

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(class ASTVisitor& visitor) = 0;
};

class NumberExpr : public ASTNode {
public:
    NumberExpr(double value) : value(value) {}
    double getValue() const { return value; }
    void accept(ASTVisitor& visitor) override;
    
private:
    double value;
};

class BinaryExpr : public ASTNode {
public:
    BinaryExpr(std::unique_ptr<ASTNode> left, std::string op, std::unique_ptr<ASTNode> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}
    
    const std::unique_ptr<ASTNode>& getLeft() const { return left; }
    const std::string& getOp() const { return op; }
    const std::unique_ptr<ASTNode>& getRight() const { return right; }
    void accept(ASTVisitor& visitor) override;
    
private:
    std::unique_ptr<ASTNode> left;
    std::string op;
    std::unique_ptr<ASTNode> right;
}; 