#include "Parser.h"
#include "AST.h"
#include <stdexcept>

Parser::Parser(Lexer& lexer) : lexer(lexer) {
    advance();
}

void Parser::advance() {
    currentToken = lexer.getNextToken();
}

void Parser::expect(TokenType type) {
    if (currentToken.type != type) {
        throw std::runtime_error("Unexpected token");
    }
    advance();
}

std::unique_ptr<ASTNode> Parser::parse() {
    return parseStatement();
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    // 这里简化处理，只解析表达式语句
    return parseExpression();
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    auto left = parsePrimary();
    
    while (currentToken.type == TOKEN_PLUS || 
           currentToken.type == TOKEN_MINUS || 
           currentToken.type == TOKEN_MULT || 
           currentToken.type == TOKEN_DIV) {
        std::string op = currentToken.value;
        advance();
        auto right = parsePrimary();
        left = std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parsePrimary() {
    if (currentToken.type == TOKEN_NUMBER) {
        double value = std::stod(currentToken.value);
        advance();
        return std::make_unique<NumberExpr>(value);
    }
    
    if (currentToken.type == TOKEN_LPAREN) {
        advance();
        auto expr = parseExpression();
        expect(TOKEN_RPAREN);
        return expr;
    }
    
    throw std::runtime_error("Unexpected token in primary expression");
} 