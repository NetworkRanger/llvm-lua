#include "CodeGen.h"
#include <llvm/IR/Constants.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

CodeGenerator::CodeGenerator() {
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("lua", *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
    
    // 创建main函数
    llvm::FunctionType* mainType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(*context), false);
    llvm::Function* mainFunc = llvm::Function::Create(
        mainType, llvm::Function::ExternalLinkage, "main", module.get());
        
    // 创建入口基本块
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*context, "entry", mainFunc);
    builder->SetInsertPoint(entry);
}

void CodeGenerator::generateCode(ASTNode* root) {
    if (root) {
        root->accept(*this);
    }
    
    // 添加返回语句到main函数
    builder->CreateRet(llvm::ConstantInt::get(*context, llvm::APInt(32, 0)));
    
    // 验证生成的代码
    llvm::verifyModule(*module, &llvm::errs());
}

void CodeGenerator::visitNumberExpr(NumberExpr* node) {
    lastValue = llvm::ConstantFP::get(*context, llvm::APFloat(node->getValue()));
}

void CodeGenerator::visitBinaryExpr(BinaryExpr* node) {
    node->getLeft()->accept(*this);
    llvm::Value* L = lastValue;
    
    node->getRight()->accept(*this);
    llvm::Value* R = lastValue;
    
    if (node->getOp() == "+") {
        lastValue = builder->CreateFAdd(L, R, "addtmp");
    } else if (node->getOp() == "-") {
        lastValue = builder->CreateFSub(L, R, "subtmp");
    } else if (node->getOp() == "*") {
        lastValue = builder->CreateFMul(L, R, "multmp");
    } else if (node->getOp() == "/") {
        lastValue = builder->CreateFDiv(L, R, "divtmp");
    } else {
        lastValue = nullptr;
        throw std::runtime_error("Unknown binary operator");
    }
} 