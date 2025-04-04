#pragma once
#include "Lexer.h"
#include <llvm/Support/raw_ostream.h>
#include <memory>
#include <vector>

class ASTNode;

class Parser {
public:
    Parser(Lexer& lexer);
    std::unique_ptr<ASTNode> parse();

private:
    Lexer& lexer;
    Token currentToken;
    
    void advance();
    void expect(TokenType type);
    
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parsePrimary();
}; 