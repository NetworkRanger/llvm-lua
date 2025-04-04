#include "CodeGen.h"
#include <llvm/IR/Constants.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/Support/TargetSelect.h>
#include <iostream>
#include <system_error>

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
    
    // 保存LLVM IR到文件
    std::error_code EC;
    llvm::raw_fd_ostream dest("output.ll", EC, llvm::sys::fs::OF_None);
    
    if (EC) {
        llvm::errs() << "Could not open file: " << EC.message() << "\n";
        return;
    }
    
    // 将模块写入文件
    module->print(dest, nullptr);
    dest.close();
    
    llvm::errs() << "LLVM IR has been saved to output.ll\n";
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
        llvm::errs() << "Error: Unknown binary operator\n";
        std::exit(1);
    }
}

bool CodeGenerator::generateObjectFile(const std::string& filename) {
    // 初始化所有目标
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    auto targetTriple = llvm::sys::getDefaultTargetTriple();
    module->setTargetTriple(targetTriple);

    std::string error;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, error);

    if (!target) {
        llvm::errs() << error;
        return false;
    }

    auto CPU = "generic";
    auto features = "";

    llvm::TargetOptions opt;
    auto RM = std::optional<llvm::Reloc::Model>();
    auto targetMachine = target->createTargetMachine(targetTriple, CPU, features, opt, RM);

    module->setDataLayout(targetMachine->createDataLayout());

    std::error_code EC;
    llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);

    if (EC) {
        llvm::errs() << "Could not open file: " << EC.message();
        return false;
    }

    llvm::legacy::PassManager pass;
    auto fileType = llvm::CodeGenFileType::ObjectFile;

    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
        llvm::errs() << "TargetMachine can't emit a file of this type";
        return false;
    }

    pass.run(*module);
    dest.close();
    
    llvm::errs() << "Object file has been generated: " << filename << "\n";
    return true;
} 