#include "LuaWriter.h"
#include <iostream>
#include <cxxabi.h>   // __cxa_demangle
#include <memory>
#include <string>
#include <typeinfo>
#include <llvm/IR/Value.h>


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

LuaWriter::~LuaWriter() = default;
LuaWriter::LuaWriter() {

}

void LuaWriter::write(Stmt* root) {
    
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

void LuaWriter::visit(NumberExpr* node) {
    std::cout << node->getValue();
}

void LuaWriter::visit(BinaryExpr* node) {
    node->getLeft()->accept(*this);

    switch (node->getOp()) {
        case BinaryOp::ADD:
            std::cout << " + ";
            break;
        case BinaryOp::SUB:
            std::cout << " - ";
            break;
        case BinaryOp::MUL:
            std::cout << " * ";
            break;
        case BinaryOp::DIV:
            std::cout << " / ";
            break;
        default:
            throw std::runtime_error("Unknown binary operator");
    }

    node->getRight()->accept(*this);

}

void LuaWriter::visit(PrintExpr* node) {
    std::cout << type_name(node) << std::endl;

    // 生成要打印的表达式的代码
    node->getExpr()->accept(*this);
}

void LuaWriter::visit(ForStmt* node) {
    std::cout << type_name(node) << std::endl;
}

void LuaWriter::visit(IfStmt* node) {
    std::cout << type_name(node) << std::endl;

    
    // 生成条件代码
    node->getCondition()->accept(*this);
    node->getThenBranch()->accept(*this);
    if (node->getElseBranch()) {
        node->getElseBranch()->accept(*this);
    }
}

void LuaWriter::visit(WhileStmt* node) {
    std::cout << type_name(node) << std::endl;
    node->getCondition()->accept(*this);
    node->getBody()->accept(*this);
}

void LuaWriter::visit(RepeatStmt* node) {
    std::cout << type_name(node) << std::endl;
    node->getBody()->accept(*this);
    node->getCondition()->accept(*this);
}

void LuaWriter::visit(FunctionDecl* node) {
    std::string name = node->getName();
    std::cout << "function "  << name << "(";
    for(const auto& param : node->getParams()) {
        std::cout  << param;
        if (&param != &node->getParams().back()) { // 如果不是最后一个参数，则添加逗号
            std::cout << ", ";
        }
    }
    std::cout << ")" << std::endl;
    // 生成函数体
    for (const auto& stmt : node->getBody()) {
        std::cout << "  ";
        stmt->accept(*this);
        std::cout << std::endl;
    }
    std::cout << "end" << std::endl << std::endl;
}

void LuaWriter::visit(ReturnStmt* node) {
    std::cout << "return ";
    for (const auto& value : node->getValues()) {
        value->accept(*this);
        if (&value != &node->getValues().back()) {
            std::cout << ", ";
        }
    }
}

void LuaWriter::visit(LocalVarDecl* node) {
    std::cout << type_name(node) << std::endl;
}

void LuaWriter::visit(StringExpr* node) {
    std::cout << '"' << node->getValue() << '"';
}

void LuaWriter::visit(NilExpr* node) {
    std::cout << type_name(node) << std::endl;
}

void LuaWriter::visit(UnaryExpr* node) {
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

void LuaWriter::visit(ExprStmt* node) {
    if (node->getExpr()) {
        node->getExpr()->accept(*this);
    }
}

void LuaWriter::visit(CallExpr* node) {
    std::string calleeName = node->getCallee();
    std::cout << calleeName << "(";

    std::vector<llvm::Value*> args;
    for (const auto& arg : node->getArguments()) {
        arg->accept(*this);
        if (&arg != &node->getArguments().back()) {
            std::cout << ", ";
        }
    }

    std::cout << ")";
}

void LuaWriter::visit(VarExpr* expr) {
    std::cout << expr->getName();
}

void LuaWriter::visit(BlockStmt* node) {
    std::cout << type_name(node) << std::endl;
    for (const auto& stmt : node->getStatements()) {
        stmt->accept(*this);
        std::cout << std::endl;
    }
}

void LuaWriter::visit(ArrayAccessExpr* expr) {
    expr->getArray()->accept(*this);
    std::cout << "[";
    expr->getIndex()->accept(*this);
    std::cout << "]";
}

void LuaWriter::visit(AssignmentStmt* node) {
    std::cout << node->getVarName() << " = ";
    node->getValue()->accept(*this);
}

void LuaWriter::visit(ArrayAssignmentStmt* node) {
    node->getArray()->accept(*this);
    std::cout << "[";
    node->getIndex()->accept(*this);
    std::cout << "] = ";
    node->getValue()->accept(*this);
}

void LuaWriter::visit(FieldAccessExpr* node) {
    node->getObject()->accept(*this);
    std::cout << "." << node->getFieldName();
}
