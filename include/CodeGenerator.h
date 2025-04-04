class CodeGenerator : public ASTVisitor {
public:
    CodeGenerator();
    void generateCode(Stmt* root);
    void saveModuleToFile(const std::string& filename);
    void executeCode();
    
    // 实现所有 ASTVisitor 的虚函数
    void visitNumberExpr(NumberExpr* expr) override;
    void visitBinaryExpr(BinaryExpr* expr) override;
    void visitUnaryExpr(UnaryExpr* expr) override;
    void visitStringExpr(StringExpr* expr) override;
    void visitNilExpr(NilExpr* expr) override;
    void visitExprStmt(ExprStmt* stmt) override;
    void visitPrintExpr(PrintExpr* expr) override;
    void visitIfStmt(IfStmt* stmt) override;
    void visitWhileStmt(WhileStmt* stmt) override;
    void visitRepeatStmt(RepeatStmt* stmt) override;
    void visitReturnStmt(ReturnStmt* stmt) override;
    void visitLocalVarDecl(LocalVarDecl* stmt) override;
    void visitBlockStmt(BlockStmt* stmt) override;
    void visitFunctionDecl(FunctionDecl* stmt) override;
    void visitCallExpr(CallExpr* expr) override;
    void visitVarExpr(VarExpr* expr) override;
    
    // ... 其他成员保持不变 ...
}; 