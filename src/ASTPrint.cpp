#include "ASTPrint.h"
#include <iostream>
#include <cxxabi.h>   // __cxa_demangle
#include <memory>
#include <string>
#include <typeinfo>


inline std::string demangle(const char* mangled) {
    int status = 0;
    std::unique_ptr<char, void (*)(void *)> res{
            abi::__cxa_demangle(mangled, nullptr, nullptr, &status),
            std::free
    };
    return (status == 0 && res) ? std::string(res.get()) : std::string(mangled);
}

template<typename T>
std::string type_name(T) { return demangle(typeid(T).name()); }

ASTPrint::~ASTPrint() = default;
ASTPrint::ASTPrint() {

}

void ASTPrint::printAST(Stmt* root) {
    // 第一阶段：收集所有函数声明
    collectFunctionDeclarations(root);
    
    // 第二阶段：生成所有函数的实现
    if (auto* blockStmt = dynamic_cast<BlockStmt*>(root)) {
        for (const auto& stmt : blockStmt->getStatements()) {
            if (auto* funcDecl = dynamic_cast<FunctionDecl*>(stmt.get())) {
                visit(funcDecl);
            }
        }
    }
    
    // 生成全局代码（非函数声明的语句）
    if (auto* blockStmt = dynamic_cast<BlockStmt*>(root)) {
        for (const auto& stmt : blockStmt->getStatements()) {
            if (!dynamic_cast<FunctionDecl*>(stmt.get())) {
                stmt->accept(*this);
            }
        }
    } else if (dynamic_cast<FunctionDecl*>(root)) {
        root->accept(*this);
    }
}

// 添加函数声明收集方法
void ASTPrint::collectFunctionDeclarations(Stmt* node) {
    // 如果是块语句，递归处理所有语句
    if (auto* blockStmt = dynamic_cast<BlockStmt*>(node)) {
        std::cout << type_name(blockStmt)<< std::endl;

        for (const auto& stmt : blockStmt->getStatements()) {
            collectFunctionDeclarations(stmt.get());
        }
        return;
    }
}

void ASTPrint::visit(NumberExpr* node) {
    std::cout << type_name(node) << ": " << node->getValue() << std::endl;
}

void ASTPrint::visit(BinaryExpr* node) {
    node->getLeft()->accept(*this);

    switch (node->getOp()) {
        case BinaryOp::ADD:
            std::cout << '+' << std::endl;
            break;
        case BinaryOp::SUB:
            std::cout << '-' << std::endl;
            break;
        case BinaryOp::MUL:
            std::cout << '*' << std::endl;
            break;
        case BinaryOp::DIV:
            std::cout << '/' << std::endl;
            break;
        default:
            throw std::runtime_error("Unknown binary operator");
    }

    node->getRight()->accept(*this);

}

void ASTPrint::visit(PrintExpr* node) {
    std::cout << type_name(node) << std::endl;

    // 生成要打印的表达式的代码
    node->getExpr()->accept(*this);
}

void ASTPrint::visit(IfStmt* node) {
    std::cout << type_name(node) << std::endl;

    
    // 生成条件代码
    node->getCondition()->accept(*this);
    node->getThenBranch()->accept(*this);
    if (node->getElseBranch()) {
        node->getElseBranch()->accept(*this);
    }
}

void ASTPrint::visit(WhileStmt* node) {
    std::cout << type_name(node) << std::endl;
    node->getCondition()->accept(*this);
    node->getBody()->accept(*this);
}

void ASTPrint::visit(RepeatStmt* node) {
    std::cout << type_name(node) << std::endl;
    node->getBody()->accept(*this);
    node->getCondition()->accept(*this);
}

void ASTPrint::visit(FunctionDecl* node) {
    std::string name = node->getName();
    std::cout << type_name(node) << ": " << name << std::endl;
    for(const auto& param : node->getParams()) {
        std::cout << "Param: " << param << std::endl;
    }
    // 生成函数体
    for (const auto& stmt : node->getBody()) {
        stmt->accept(*this);
    }
}

void ASTPrint::visit(ReturnStmt* node) {
    std::cout << type_name(node) << std::endl;
    for (const auto& value : node->getValues()) {
        value->accept(*this);
    }
}

void ASTPrint::visit(LocalVarDecl* node) {
    std::cout << type_name(node) << std::endl;
}

void ASTPrint::visit(StringExpr* node) {
    std::cout << type_name(node) << ": " << node->getValue() << std::endl;
}

void ASTPrint::visit(NilExpr* node) {
    std::cout << type_name(node) << std::endl;
}

void ASTPrint::visit(UnaryExpr* node) {
    std::cout << type_name(node) << std::endl;
    node->getExpr()->accept(*this);

    switch (node->getOp()) {
        case UnaryOp::NOT_OP:
            std::cout << "not" << std::endl;
            break;
        case UnaryOp::NEG:
            std::cout << "neg" << std::endl;
            break;
        default:
            throw std::runtime_error("Unknown unary operator");
    }

}

void ASTPrint::visit(ExprStmt* node) {
    std::cout << type_name(node) << std::endl;
    if (node->getExpr()) {
        node->getExpr()->accept(*this);
    }
}

void ASTPrint::visit(CallExpr* node) {
    std::string calleeName = node->getCallee();
    std::cout << type_name(node) << std::endl;
    std::cout << "Callee: " << calleeName << std::endl;

    std::vector<llvm::Value*> args;
    for (const auto& arg : node->getArguments()) {
        arg->accept(*this);
    }
}

void ASTPrint::visit(VarExpr* expr) {
    std::cout << type_name(expr) << std::endl;
    std::cout << "Name: " << expr->getName() << std::endl;
}

void ASTPrint::visit(BlockStmt* node) {
    std::cout << type_name(node) << std::endl;
    for (const auto& stmt : node->getStatements()) {
        stmt->accept(*this);
    }
}
