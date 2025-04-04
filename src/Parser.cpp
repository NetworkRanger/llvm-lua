#include "Parser.h"
#include <stdexcept>
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) 
    : tokens(tokens)
    , current(0)
    , currentToken(tokens[0])  // 初始化当前token
    , previousToken(tokens[0]) // 初始化前一个token
{
}

Token Parser::advance() {
    previousToken = currentToken;
    if (!isAtEnd()) {
        current++;
        currentToken = tokens[current];
    }
    return previousToken;
}

bool Parser::isAtEnd() const {
    return current >= tokens.size() || 
           tokens[current].getType() == TokenType::TOKEN_EOF;
}

Token Parser::previous() const {
    return previousToken;
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return currentToken.getType() == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error(message);
}

std::unique_ptr<Stmt> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    
    while (!isAtEnd()) {
        if (match(TokenType::TOKEN_FUNCTION)) {
            statements.push_back(parseFunctionDecl());
        } else {
            statements.push_back(parseStmt());
        }
    }
    
    return std::make_unique<BlockStmt>(std::move(statements));
}

std::unique_ptr<Stmt> Parser::parseFunctionDecl() {
    // 获取函数名
    consume(TokenType::TOKEN_IDENTIFIER, "Expect function name.");
    std::string name = previous().getValue();
    
    // 解析参数列表
    consume(TokenType::TOKEN_LPAREN, "Expect '(' after function name.");
    std::vector<std::string> params;
    
    if (!check(TokenType::TOKEN_RPAREN)) {
        do {
            consume(TokenType::TOKEN_IDENTIFIER, "Expect parameter name.");
            params.push_back(previous().getValue());
        } while (match(TokenType::TOKEN_COMMA));
    }
    
    consume(TokenType::TOKEN_RPAREN, "Expect ')' after parameters.");
    
    // 解析函数体
    std::vector<std::unique_ptr<Stmt>> body;
    while (!check(TokenType::TOKEN_END) && !isAtEnd()) {
        body.push_back(parseStmt());
    }
    
    consume(TokenType::TOKEN_END, "Expect 'end' after function body.");
    
    return std::make_unique<FunctionDecl>(name, std::move(params), std::move(body));
}

std::unique_ptr<Stmt> Parser::parseStmt() {
    std::cout << "Parsing statement, current token: " 
              << currentToken.getValue() 
              << " (type: " << static_cast<int>(currentToken.getType()) 
              << ")" << std::endl;

    // 处理函数调用和标识符
    if (check(TokenType::TOKEN_IDENTIFIER)) {
        std::string name = currentToken.getValue();
        
        // 检查是否是 print 函数
        if (name == "print") {
            advance(); // 消费 print 标识符
            consume(TokenType::TOKEN_LPAREN, "Expected '(' after print");
            auto expr = parseExpr();
            consume(TokenType::TOKEN_RPAREN, "Expected ')' after expression");
            return std::make_unique<PrintExpr>(std::move(expr));
        }
        
        // 处理其他函数调用
        advance(); // 消费标识符
        if (match(TokenType::TOKEN_LPAREN)) {
            std::vector<std::unique_ptr<Expr>> arguments;
            if (!check(TokenType::TOKEN_RPAREN)) {
                do {
                    arguments.push_back(parseExpr());
                } while (match(TokenType::TOKEN_COMMA));
            }
            consume(TokenType::TOKEN_RPAREN, "Expected ')' after arguments");
            return std::make_unique<ExprStmt>(
                std::make_unique<CallExpr>(name, std::move(arguments)));
        }
        
        // 如果不是函数调用，回退
        current--;
        currentToken = tokens[current];
    }
    
    // 处理其他语句类型
    if (match(TokenType::TOKEN_RETURN)) {
        return parseReturnStmt();
    }
    
    if (match(TokenType::TOKEN_IF)) {
        return parseIfStmt();
    }
    
    if (match(TokenType::TOKEN_WHILE)) {
        return parseWhileStmt();
    }
    
    if (match(TokenType::TOKEN_REPEAT)) {
        return parseRepeatStmt();
    }
    
    if (match(TokenType::TOKEN_LOCAL)) {
        return parseLocalVarDecl();
    }
    
    // 默认作为表达式语句处理
    return parseExprStmt();
}

std::unique_ptr<Stmt> Parser::parseExprStmt() {
    auto expr = parseExpr();
    return std::make_unique<ExprStmt>(std::move(expr));
}

std::unique_ptr<Expr> Parser::parseExpr() {
    std::cout << "Parsing expression, current token: " 
              << currentToken.getValue() 
              << " (type: " << static_cast<int>(currentToken.getType()) 
              << ")" << std::endl;
    auto left = parseTerm();
    
    while (match(TokenType::TOKEN_PLUS) || match(TokenType::TOKEN_MINUS)) {
        TokenType op = previous().getType();
        auto right = parseTerm();
        left = std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
    }
    
    return left;
}

std::unique_ptr<Expr> Parser::parseTerm() {
    auto left = parseFactor();
    
    while (match(TokenType::TOKEN_MULT) || match(TokenType::TOKEN_DIV)) {
        TokenType op = previous().getType();
        auto right = parseFactor();
        left = std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
    }
    
    return left;
}

std::unique_ptr<Expr> Parser::parseFactor() {
    if (match(TokenType::TOKEN_MINUS) || match(TokenType::TOKEN_NOT)) {
        TokenType op = previous().getType();
        auto expr = parseFactor();
        return std::make_unique<UnaryExpr>(op, std::move(expr));
    }
    
    return parsePrimary();
}

std::unique_ptr<Expr> Parser::parsePrimary() {
    std::cout << "Parsing primary, current token: " 
              << currentToken.getValue() 
              << " (type: " << static_cast<int>(currentToken.getType()) 
              << ")" << std::endl;

    if (match(TokenType::TOKEN_NUMBER)) {
        double value = std::stod(previous().getValue());
        return std::make_unique<NumberExpr>(value);
    }
    
    if (match(TokenType::TOKEN_STRING)) {
        std::string value = previous().getValue();
        return std::make_unique<StringExpr>(value);
    }
    
    if (match(TokenType::TOKEN_NIL)) {
        return std::make_unique<NilExpr>();
    }
    
    if (check(TokenType::TOKEN_IDENTIFIER)) {
        std::string name = currentToken.getValue();
        advance();  // 消费标识符
        
        // 检查函数调用
        if (check(TokenType::TOKEN_LPAREN)) {
            advance();  // 消费左括号
            std::vector<std::unique_ptr<Expr>> arguments;
            
            if (!check(TokenType::TOKEN_RPAREN)) {
                do {
                    arguments.push_back(parseExpr());
                } while (match(TokenType::TOKEN_COMMA));
            }
            
            consume(TokenType::TOKEN_RPAREN, "Expected ')' after arguments");
            return std::make_unique<CallExpr>(name, std::move(arguments));
        }
        
        return std::make_unique<VarExpr>(name);
    }
    
    if (match(TokenType::TOKEN_LPAREN)) {
        auto expr = parseExpr();
        consume(TokenType::TOKEN_RPAREN, "Expected ')' after expression");
        return expr;
    }
    
    throw std::runtime_error("Unexpected token in expression: " + 
        currentToken.getValue());
}

std::unique_ptr<Stmt> Parser::parseIfStmt() {
    consume(TokenType::TOKEN_IF, "Expected 'if'");
    auto condition = parseExpr();
    consume(TokenType::TOKEN_THEN, "Expected 'then'");
    auto thenBranch = parseStmt();
    std::unique_ptr<Stmt> elseBranch = nullptr;
    
    if (match(TokenType::TOKEN_ELSEIF)) {
        elseBranch = parseIfStmt();
    } else if (match(TokenType::TOKEN_ELSE)) {
        elseBranch = parseStmt();
    }
    
    consume(TokenType::TOKEN_END, "Expected 'end'");
    return std::make_unique<IfStmt>(std::move(condition), 
                                   std::move(thenBranch), 
                                   std::move(elseBranch));
}

std::unique_ptr<Stmt> Parser::parseWhileStmt() {
    consume(TokenType::TOKEN_WHILE, "Expected 'while'");
    auto condition = parseExpr();
    consume(TokenType::TOKEN_DO, "Expected 'do'");
    auto body = parseStmt();
    consume(TokenType::TOKEN_END, "Expected 'end'");
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::parseRepeatStmt() {
    consume(TokenType::TOKEN_REPEAT, "Expected 'repeat'");
    auto body = parseStmt();
    consume(TokenType::TOKEN_UNTIL, "Expected 'until'");
    auto condition = parseExpr();
    return std::make_unique<RepeatStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::parseReturnStmt() {
    std::cout << "Parsing return statement, current token: " 
              << currentToken.getValue() 
              << " (type: " << static_cast<int>(currentToken.getType()) 
              << ")" << std::endl;

    std::vector<std::unique_ptr<Expr>> values;
    
    // 如果下一个 token 不是分号和 end，解析返回值
    if (!check(TokenType::TOKEN_SEMICOLON) && !check(TokenType::TOKEN_END)) {
        // 解析第一个表达式
        values.push_back(parseExpr());
        
        // 解析其他返回值
        while (match(TokenType::TOKEN_COMMA)) {
            values.push_back(parseExpr());
        }
    }
    
    // 可选的分号
    match(TokenType::TOKEN_SEMICOLON);
    
    return std::make_unique<ReturnStmt>(std::move(values));
}

std::unique_ptr<Stmt> Parser::parseLocalVarDecl() {
    consume(TokenType::TOKEN_LOCAL, "Expected 'local'");
    std::string name = consume(TokenType::TOKEN_IDENTIFIER, "Expected identifier").getValue();
    std::unique_ptr<Expr> initializer = nullptr;
    
    if (match(TokenType::TOKEN_ASSIGN)) {
        initializer = parseExpr();
    }
    
    return std::make_unique<LocalVarDecl>(name, std::move(initializer));
}

std::unique_ptr<Expr> Parser::parseUnary() {
    if (match(TokenType::TOKEN_NOT) || match(TokenType::TOKEN_MINUS)) {
        TokenType op = previous().getType();
        auto expr = parsePrimary();
        return std::make_unique<UnaryExpr>(op, std::move(expr));
    }
    return parsePrimary();
}

void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previousToken.getType() == TokenType::TOKEN_SEMICOLON) return;
        
        TokenType type = currentToken.getType();
        if (type == TokenType::TOKEN_FUNCTION ||
            type == TokenType::TOKEN_LOCAL ||
            type == TokenType::TOKEN_IF ||
            type == TokenType::TOKEN_WHILE ||
            type == TokenType::TOKEN_REPEAT ||
            type == TokenType::TOKEN_RETURN) {
            return;
        }
        
        advance();
    }
}

Token Parser::peek() const {
    if (isAtEnd()) return tokens[current];
    return tokens[current + 1];
}

