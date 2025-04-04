#pragma once
#include "Lexer.h"
#include "AST.h"
#include <vector>
#include <memory>

class Parser {
private:
    std::vector<Token> tokens;
    size_t current = 0;
    Token currentToken;
    Token previousToken;
    
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(TokenType type);
    Token advance();
    Token previous() const;
    Token peek() const;
    Token consume(TokenType type, const std::string& message);
    
    std::unique_ptr<Stmt> parseStmt();
    std::unique_ptr<Stmt> parseFunctionDecl();
    std::unique_ptr<Stmt> parseExprStmt();
    std::unique_ptr<Stmt> parseIfStmt();
    std::unique_ptr<Stmt> parseWhileStmt();
    std::unique_ptr<Stmt> parseRepeatStmt();
    std::unique_ptr<Stmt> parseReturnStmt();
    std::unique_ptr<Stmt> parseLocalVarDecl();
    
    std::unique_ptr<Expr> parseExpr();
    std::unique_ptr<Expr> parseTerm();
    std::unique_ptr<Expr> parseFactor();
    std::unique_ptr<Expr> parsePrimary();
    std::unique_ptr<Expr> parseUnary();
    
    void synchronize();

public:
    explicit Parser(const std::vector<Token>& tokens);
    std::unique_ptr<Stmt> parse();
}; 