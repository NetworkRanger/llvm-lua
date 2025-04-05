#include "CodeGen.h"
#include <llvm/IR/Constants.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/TargetParser/Host.h>
#include <system_error>
#include <iostream>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>

CodeGenerator::~CodeGenerator() = default;

void CodeGenerator::declarePrintf() {
    // 声明printf函数
    std::vector<llvm::Type*> printfArgs;
    printfArgs.push_back(llvm::PointerType::get(builder->getInt8Ty(), 0));
    llvm::FunctionType* printfType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(*context), printfArgs, true);
    printfFunc = llvm::Function::Create(
        printfType, llvm::Function::ExternalLinkage, "printf", module.get());
}

CodeGenerator::CodeGenerator() {
    // 创建LLVM上下文和模块
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("lua", *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
    
    // 声明 printf 函数
    std::vector<llvm::Type*> printfArgs;
    printfArgs.push_back(llvm::PointerType::get(builder->getInt8Ty(), 0));
    llvm::FunctionType* printfType = 
        llvm::FunctionType::get(builder->getInt32Ty(), printfArgs, true);
    module->getOrInsertFunction("printf", printfType);
}

void CodeGenerator::generateCode(Stmt* root) {
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
    
    // 创建 main 函数
    llvm::FunctionType* mainType = 
        llvm::FunctionType::get(llvm::Type::getInt32Ty(*context), false);
    llvm::Function* mainFunc = 
        llvm::Function::Create(mainType, llvm::Function::ExternalLinkage,
                             "main", module.get());
    
    currentFunction = mainFunc;
    
    // 创建入口基本块
    llvm::BasicBlock* block = 
        llvm::BasicBlock::Create(*context, "entry", mainFunc);
    builder->SetInsertPoint(block);
    
    // 生成全局代码（非函数声明的语句）
    if (auto* blockStmt = dynamic_cast<BlockStmt*>(root)) {
        for (const auto& stmt : blockStmt->getStatements()) {
            if (!dynamic_cast<FunctionDecl*>(stmt.get())) {
                stmt->accept(*this);
            }
        }
    } else if (!dynamic_cast<FunctionDecl*>(root)) {
        root->accept(*this);
    }
    
    // 确保基本块有终止指令
    if (!block->getTerminator()) {
        builder->CreateRet(llvm::ConstantInt::get(
            llvm::Type::getInt32Ty(*context), 0));
    }
    
    // 验证生成的代码
    std::string errorInfo;
    llvm::raw_string_ostream errorStream(errorInfo);
    if (llvm::verifyModule(*module, &errorStream)) {
        throw std::runtime_error("Module verification failed: " + errorInfo);
    }
}

// 添加函数声明收集方法
void CodeGenerator::collectFunctionDeclarations(Stmt* node) {
    // 如果是块语句，递归处理所有语句
    if (auto* blockStmt = dynamic_cast<BlockStmt*>(node)) {
        for (const auto& stmt : blockStmt->getStatements()) {
            collectFunctionDeclarations(stmt.get());
        }
        return;
    }
    
    // 处理函数声明
    if (auto* funcDecl = dynamic_cast<FunctionDecl*>(node)) {
        std::string name = funcDecl->getName();
        
        // 如果函数已经声明，跳过
        if (module->getFunction(name)) {
            return;
        }
        
        // 创建参数类型列表
        std::vector<llvm::Type*> paramTypes(
            funcDecl->getParams().size(),
            llvm::Type::getDoubleTy(*context));
        
        // 确定返回类型
        llvm::Type* returnType;
        if (hasMultipleReturns(funcDecl)) {
            std::vector<llvm::Type*> returnTypes(2, 
                llvm::Type::getDoubleTy(*context));
            returnType = llvm::StructType::create(
                *context, returnTypes, name + "_return");
        } else {
            returnType = llvm::Type::getDoubleTy(*context);
        }
        
        // 创建函数类型
        llvm::FunctionType* functionType = 
            llvm::FunctionType::get(returnType, paramTypes, false);
        
        // 创建函数
        llvm::Function* func = llvm::Function::Create(
            functionType,
            llvm::Function::ExternalLinkage,
            name,
            module.get());
            
        // 设置函数参数名称
        size_t idx = 0;
        for (auto& arg : func->args()) {
            arg.setName(funcDecl->getParams()[idx++]);
        }
        
        // 设置函数属性以防止优化
        func->addFnAttr(llvm::Attribute::NoInline);
        func->addFnAttr(llvm::Attribute::OptimizeNone);
    }
}

void CodeGenerator::visit(NumberExpr* node) {
    lastValue = llvm::ConstantFP::get(*context, llvm::APFloat(node->getValue()));
}

void CodeGenerator::visit(BinaryExpr* node) {
    node->getLeft()->accept(*this);
    llvm::Value* L = lastValue;
    node->getRight()->accept(*this);
    llvm::Value* R = lastValue;
    
    switch (node->getOp()) {
        case BinaryOp::ADD:
            lastValue = builder->CreateFAdd(L, R, "addtmp");
            break;
        case BinaryOp::SUB:
            lastValue = builder->CreateFSub(L, R, "subtmp");
            break;
        case BinaryOp::MUL:
            lastValue = builder->CreateFMul(L, R, "multmp");
            break;
        case BinaryOp::DIV:
            lastValue = builder->CreateFDiv(L, R, "divtmp");
            break;
        default:
            throw std::runtime_error("Unknown binary operator");
    }
}

void CodeGenerator::visit(PrintExpr* node) {
    // 生成要打印的表达式的代码
    node->getExpr()->accept(*this);
    llvm::Value* exprValue = lastValue;
    
    // 如果表达式结果是结构体类型，提取第一个元素
    if (exprValue->getType()->isStructTy()) {
        exprValue = builder->CreateExtractValue(exprValue, 0);
    }
    
    // 声明 printf 函数（如果还没有声明）
    llvm::Function* printf = module->getFunction("printf");
    if (!printf) {
        std::vector<llvm::Type*> args;
        args.push_back(llvm::PointerType::get(builder->getInt8Ty(), 0));
        llvm::FunctionType* printfType = 
            llvm::FunctionType::get(builder->getInt32Ty(), args, true);
        printf = llvm::Function::Create(printfType, 
            llvm::Function::ExternalLinkage, "printf", module.get());
    }

    // 创建格式字符串
    llvm::Value* formatStr = builder->CreateGlobalString("%g\n", "fmt");
    llvm::Value* formatStrPtr = builder->CreatePointerCast(
        formatStr, 
        llvm::PointerType::get(builder->getInt8Ty(), 0)
    );

    // 调用 printf
    std::vector<llvm::Value*> args;
    args.push_back(formatStrPtr);
    args.push_back(exprValue);
    builder->CreateCall(printf, args);
}

void CodeGenerator::visit(IfStmt* node) {
    llvm::Function* function = builder->GetInsertBlock()->getParent();
    
    llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(*context, "then", function);
    llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(*context, "else");
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*context, "ifcont");
    
    // 生成条件代码
    node->getCondition()->accept(*this);
    llvm::Value* condV = lastValue;
    condV = builder->CreateFCmpONE(condV, llvm::ConstantFP::get(*context, llvm::APFloat(0.0)), "ifcond");
    
    builder->CreateCondBr(condV, thenBB, elseBB);
    
    // 生成then分支
    builder->SetInsertPoint(thenBB);
    node->getThenBranch()->accept(*this);
    builder->CreateBr(mergeBB);
    
    // 生成else分支
    function->insert(function->end(), elseBB);
    builder->SetInsertPoint(elseBB);
    if (node->getElseBranch()) {
        node->getElseBranch()->accept(*this);
    }
    builder->CreateBr(mergeBB);
    
    // 生成合并块
    function->insert(function->end(), mergeBB);
    builder->SetInsertPoint(mergeBB);
}

void CodeGenerator::visit(WhileStmt* node) {
    llvm::Function* function = builder->GetInsertBlock()->getParent();
    
    llvm::BasicBlock* condBB = llvm::BasicBlock::Create(*context, "whilecond", function);
    llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create(*context, "whilebody");
    llvm::BasicBlock* afterBB = llvm::BasicBlock::Create(*context, "whileend");
    
    builder->CreateBr(condBB);
    
    builder->SetInsertPoint(condBB);
    node->getCondition()->accept(*this);
    llvm::Value* condV = lastValue;
    condV = builder->CreateFCmpONE(condV, llvm::ConstantFP::get(*context, llvm::APFloat(0.0)), "whilecond");
    
    builder->CreateCondBr(condV, bodyBB, afterBB);
    
    function->insert(function->end(), bodyBB);
    builder->SetInsertPoint(bodyBB);
    node->getBody()->accept(*this);
    builder->CreateBr(condBB);
    
    function->insert(function->end(), afterBB);
    builder->SetInsertPoint(afterBB);
}

void CodeGenerator::visit(RepeatStmt* node) {
    llvm::Function* function = builder->GetInsertBlock()->getParent();
    
    llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create(*context, "repeatbody", function);
    llvm::BasicBlock* condBB = llvm::BasicBlock::Create(*context, "repeatcond");
    llvm::BasicBlock* afterBB = llvm::BasicBlock::Create(*context, "repeatend");
    
    builder->CreateBr(bodyBB);
    
    builder->SetInsertPoint(bodyBB);
    node->getBody()->accept(*this);
    builder->CreateBr(condBB);
    
    function->insert(function->end(), condBB);
    builder->SetInsertPoint(condBB);
    node->getCondition()->accept(*this);
    llvm::Value* condV = lastValue;
    condV = builder->CreateFCmpONE(condV, llvm::ConstantFP::get(*context, llvm::APFloat(0.0)), "repeatcond");
    
    builder->CreateCondBr(condV, afterBB, bodyBB);
    
    function->insert(function->end(), afterBB);
    builder->SetInsertPoint(afterBB);
}

void CodeGenerator::visit(FunctionDecl* node) {
    std::string name = node->getName();
    
    // 获取已声明的函数
    llvm::Function* function = module->getFunction(name);
    if (!function) {
        throw std::runtime_error("Function " + name + " not found in module");
    }
    
    // 保存当前函数
    currentFunction = function;
    
    // 创建基本块
    llvm::BasicBlock* block = 
        llvm::BasicBlock::Create(*context, "entry", function);
    builder->SetInsertPoint(block);
    
    // 处理参数
    namedValues.clear();
    size_t idx = 0;
    for (auto& arg : function->args()) {
        arg.setName(node->getParams()[idx++]);
        llvm::AllocaInst* alloca = createEntryBlockAlloca(function, arg.getName().str());
        builder->CreateStore(&arg, alloca);
        namedValues[arg.getName().str()] = alloca;
    }
    
    // 生成函数体
    for (const auto& stmt : node->getBody()) {
        stmt->accept(*this);
    }
    
    // 确保有返回值
    if (!block->getTerminator()) {
        if (function->getReturnType()->isStructTy()) {
            llvm::Value* returnStruct = llvm::UndefValue::get(function->getReturnType());
            returnStruct = builder->CreateInsertValue(returnStruct,
                llvm::ConstantFP::get(*context, llvm::APFloat(0.0)), 0);
            returnStruct = builder->CreateInsertValue(returnStruct,
                llvm::ConstantFP::get(*context, llvm::APFloat(0.0)), 1);
            builder->CreateRet(returnStruct);
        } else {
            builder->CreateRet(llvm::ConstantFP::get(*context, llvm::APFloat(0.0)));
        }
    }
}

void CodeGenerator::visit(ReturnStmt* node) {
    if (!currentFunction) {
        throw std::runtime_error("Return statement outside of function");
    }
    
    llvm::Type* returnTy = currentFunction->getReturnType();
    if (llvm::StructType* structTy = llvm::dyn_cast<llvm::StructType>(returnTy)) {
        // 处理多返回值
        std::vector<llvm::Value*> returnValues;
        
        for (const auto& value : node->getValues()) {
            value->accept(*this);
            // 如果返回值本身是结构体，提取第一个元素
            if (lastValue->getType()->isStructTy()) {
                lastValue = builder->CreateExtractValue(lastValue, 0);
            }
            returnValues.push_back(lastValue);
        }
        
        // 创建返回结构体
        llvm::Value* returnStruct = llvm::UndefValue::get(structTy);
        for (size_t i = 0; i < returnValues.size() && i < 2; ++i) {
            returnStruct = builder->CreateInsertValue(returnStruct, returnValues[i], i);
        }
        
        // 如果返回值不足，用0.0填充
        for (size_t i = returnValues.size(); i < 2; ++i) {
            returnStruct = builder->CreateInsertValue(returnStruct,
                llvm::ConstantFP::get(*context, llvm::APFloat(0.0)), i);
        }
        
        builder->CreateRet(returnStruct);
    } else {
        // 单返回值
        if (!node->getValues().empty()) {
            node->getValues()[0]->accept(*this);
            // 如果返回值是结构体，提取第一个元素
            if (lastValue->getType()->isStructTy()) {
                lastValue = builder->CreateExtractValue(lastValue, 0);
            }
            builder->CreateRet(lastValue);
        } else {
            builder->CreateRet(llvm::ConstantFP::get(*context, llvm::APFloat(0.0)));
        }
    }
}

void CodeGenerator::visit(LocalVarDecl* node) {
    if (node->getInitializer()) {
        node->getInitializer()->accept(*this);
        llvm::AllocaInst* alloca = builder->CreateAlloca(
            llvm::Type::getDoubleTy(*context), nullptr, node->getName());
        builder->CreateStore(lastValue, alloca);
    }
}

void CodeGenerator::visit(StringExpr* node) {
    llvm::Value* str = builder->CreateGlobalString(node->getValue());
    lastValue = builder->CreatePointerCast(str, 
        llvm::PointerType::get(builder->getInt8Ty(), 0));
}

void CodeGenerator::visit(NilExpr* node) {
    lastValue = llvm::ConstantFP::get(*context, llvm::APFloat(0.0));
}

void CodeGenerator::visit(UnaryExpr* node) {
    node->getExpr()->accept(*this);
    llvm::Value* exprValue = lastValue;
    
    switch (node->getOp()) {
        case UnaryOp::NOT_OP:
            lastValue = builder->CreateFCmpOEQ(
                exprValue, llvm::ConstantFP::get(*context, llvm::APFloat(0.0)));
            break;
        case UnaryOp::NEG:
            lastValue = builder->CreateFNeg(exprValue);
            break;
        default:
            throw std::runtime_error("Unknown unary operator");
    }
}

void CodeGenerator::visit(ExprStmt* node) {
    if (node->getExpr()) {
        node->getExpr()->accept(*this);
    }
}

void CodeGenerator::saveModuleToFile(const std::string& filename) {
    // 确保目录存在
    std::string dir = filename.substr(0, filename.find_last_of("/\\"));
    if (!dir.empty()) {
        std::string cmd = "mkdir -p " + dir;
        system(cmd.c_str());
    }

    std::error_code EC;
    llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);
    
    if (EC) {
        std::cerr << "Could not open file: " << filename << std::endl;
        std::cerr << "Error: " << EC.message() << std::endl;
        throw std::runtime_error("Could not open output file: " + EC.message());
    }
    
    // 打印模块到文件
    module->print(dest, nullptr);
    dest.close();
}

void CodeGenerator::visit(CallExpr* node) {
    std::string calleeName = node->getCallee();
    llvm::Function* callee = module->getFunction(calleeName);
    
    if (!callee) {
        if (calleeName == "print") {
            // 处理 print 函数调用
            std::vector<llvm::Value*> args;
            for (const auto& arg : node->getArguments()) {
                arg->accept(*this);
                args.push_back(lastValue);
            }
            
            // 创建格式字符串
            llvm::Value* formatStr = builder->CreateGlobalString("%g\n", "fmt");
            
            // 调用 printf
            std::vector<llvm::Value*> printfArgs;
            printfArgs.push_back(builder->CreatePointerCast(formatStr,
                llvm::PointerType::get(builder->getInt8Ty(), 0)));
            printfArgs.push_back(args[0]);
            
            lastValue = builder->CreateCall(module->getFunction("printf"), printfArgs);
            return;
        }
        throw std::runtime_error("Unknown function: " + calleeName);
    }
    
    // 生成参数
    std::vector<llvm::Value*> args;
    for (const auto& arg : node->getArguments()) {
        arg->accept(*this);
        
        // 如果参数是函数调用的结果，并且是结构体类型
        if (lastValue->getType()->isStructTy()) {
            // 如果当前函数只需要一个参数，提取第一个元素
            if (callee->arg_size() == 1) {
                lastValue = builder->CreateExtractValue(lastValue, 0);
            }
            // 如果当前函数需要两个参数，提取两个元素
            else if (callee->arg_size() == 2) {
                llvm::Value* firstValue = builder->CreateExtractValue(lastValue, 0);
                llvm::Value* secondValue = builder->CreateExtractValue(lastValue, 1);
                args.push_back(firstValue);
                args.push_back(secondValue);
                continue;  // 跳过下面的 args.push_back(lastValue)
            }
        }
        args.push_back(lastValue);
    }
    
    // 创建函数调用
    lastValue = builder->CreateCall(callee, args, calleeName + "_result");
}

void CodeGenerator::visit(VarExpr* expr) {
    llvm::AllocaInst* alloca = namedValues[expr->getName()];
    if (!alloca) {
        throw std::runtime_error("Unknown variable: " + expr->getName());
    }
    lastValue = builder->CreateLoad(alloca->getAllocatedType(), alloca, expr->getName().c_str());
}

void CodeGenerator::visit(BlockStmt* node) {
    for (const auto& stmt : node->getStatements()) {
        stmt->accept(*this);
    }
}

// 辅助函数：检查函数是否有多个返回值
bool CodeGenerator::hasMultipleReturns(FunctionDecl* node) {
    for (const auto& stmt : node->getBody()) {
        if (const auto* returnStmt = dynamic_cast<const ReturnStmt*>(stmt.get())) {
            if (returnStmt->getValues().size() > 1) {
                return true;
            }
        }
    }
    return false;
}

// 添加一个辅助函数来创建entry block alloca
llvm::AllocaInst* CodeGenerator::createEntryBlockAlloca(llvm::Function* function,
                                                       const std::string& varName) {
    llvm::IRBuilder<> tmpBuilder(&function->getEntryBlock(),
                                function->getEntryBlock().begin());
    return tmpBuilder.CreateAlloca(llvm::Type::getDoubleTy(*context),
                                 nullptr, varName);
}

void CodeGenerator::initBuiltins() {
    // 声明 printf 函数
    std::vector<llvm::Type*> printfArgs;
    printfArgs.push_back(llvm::PointerType::get(builder->getInt8Ty(), 0));
    llvm::FunctionType* printfType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(*context), printfArgs, true);
    printfFunc = llvm::Function::Create(
        printfType, llvm::Function::ExternalLinkage, "printf", module.get());
        
    // 声明 fflush 函数
    std::vector<llvm::Type*> flushArgs;
    flushArgs.push_back(llvm::PointerType::get(builder->getInt8Ty(), 0));
    llvm::FunctionType* flushType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(*context), flushArgs, false);
    llvm::Function::Create(
        flushType, llvm::Function::ExternalLinkage, "fflush", module.get());
}

void CodeGenerator::executeCode() {
    // 初始化 LLVM 目标
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    std::string error;
    
    // 创建执行引擎
    std::unique_ptr<llvm::Module> modulePtr(std::move(module));
    llvm::ExecutionEngine* engine = 
        llvm::EngineBuilder(std::move(modulePtr))
            .setEngineKind(llvm::EngineKind::JIT)
            .setErrorStr(&error)
            .create();
            
    if (!engine) {
        throw std::runtime_error("Failed to create execution engine: " + error);
    }

    // 获取 main 函数
    auto mainFunc = (int (*)())engine->getFunctionAddress("main");
    if (!mainFunc) {
        throw std::runtime_error("Failed to get main function");
    }

    // 执行 main 函数
    mainFunc();

    // 清理
    delete engine;
}
