#include "Lexer.h"
#include <cctype>
#include <iostream>
#include <map>

// 初始化关键字表
std::map<std::string, TokenType> Lexer::keywords;

void Lexer::initKeywords() {
    keywords["and"] = TokenType::TOKEN_AND;
    keywords["break"] = TokenType::TOKEN_BREAK;
    keywords["do"] = TokenType::TOKEN_DO;
    keywords["else"] = TokenType::TOKEN_ELSE;
    keywords["elseif"] = TokenType::TOKEN_ELSEIF;
    keywords["end"] = TokenType::TOKEN_END;
    keywords["false"] = TokenType::TOKEN_FALSE;
    keywords["for"] = TokenType::TOKEN_FOR;
    keywords["function"] = TokenType::TOKEN_FUNCTION;
    keywords["if"] = TokenType::TOKEN_IF;
    keywords["in"] = TokenType::TOKEN_IN;
    keywords["local"] = TokenType::TOKEN_LOCAL;
    keywords["nil"] = TokenType::TOKEN_NIL;
    keywords["not"] = TokenType::TOKEN_NOT;
    keywords["or"] = TokenType::TOKEN_OR;
    keywords["repeat"] = TokenType::TOKEN_REPEAT;
    keywords["return"] = TokenType::TOKEN_RETURN;
    keywords["then"] = TokenType::TOKEN_THEN;
    keywords["true"] = TokenType::TOKEN_TRUE;
    keywords["until"] = TokenType::TOKEN_UNTIL;
    keywords["while"] = TokenType::TOKEN_WHILE;
}

Lexer::Lexer(const std::string& source) 
    : source(source)
    , currentPos(0)
    , startPos(0)
    , line(1)
    , column(1)
    , current(source.empty() ? '\0' : source[0]) {
    if (keywords.empty()) {
        initKeywords();
    }
}

char Lexer::advance() {
    if (current == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    
    currentPos++;
    current = currentPos < source.length() ? source[currentPos] : '\0';
    return current;
}

char Lexer::peek() const {
    if (currentPos + 1 >= source.length()) return '\0';
    return source[currentPos + 1];
}

bool Lexer::match(char expected) {
    if (current != expected) return false;
    advance();
    return true;
}

void Lexer::skipWhitespace() {
    while (true) {
        switch (current) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                advance();
                break;
            case '-':
                if (peek() == '-') {
                    skipComment();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

void Lexer::skipComment() {
    // 跳过 --
    advance();
    advance();
    
    // 跳过到行尾
    while (current != '\n' && current != '\0') {
        advance();
    }
}

Token Lexer::makeToken(TokenType type) const {
    std::string text = source.substr(startPos, currentPos - startPos);
    return Token(type, text, line, column - (currentPos - startPos));
}

Token Lexer::makeToken(TokenType type, const std::string& value) const {
    return Token(type, value, line, column - value.length());
}

Token Lexer::errorToken(const std::string& message) const {
    return Token(TokenType::TOKEN_ERROR, message, line, column);
}

Token Lexer::number() {
    while (isdigit(current)) advance();
    
    // 处理小数点
    if (current == '.' && isdigit(peek())) {
        advance(); // 消费 '.'
        while (isdigit(current)) advance();
    }
    
    return makeToken(TokenType::TOKEN_NUMBER);
}

Token Lexer::string() {
    advance(); // 消费开始的引号
    
    startPos = currentPos;
    while (current != '"' && current != '\0') {
        if (current == '\\') {
            advance(); // 跳过转义字符
        }
        advance();
    }
    
    if (current == '\0') {
        return errorToken("Unterminated string");
    }
    
    std::string value = source.substr(startPos, currentPos - startPos);
    advance(); // 消费结束的引号
    
    return makeToken(TokenType::TOKEN_STRING, value);
}

Token Lexer::identifier() {
    while (isalnum(current) || current == '_') advance();
    
    std::string text = source.substr(startPos, currentPos - startPos);
    
    // 检查是否是关键字
    auto it = keywords.find(text);
    TokenType type = it != keywords.end() ? it->second : TokenType::TOKEN_IDENTIFIER;
    
    return makeToken(type);
}

Token Lexer::getNextToken() {
    skipWhitespace();
    
    startPos = currentPos;
    
    if (current == '\0') return makeToken(TokenType::TOKEN_EOF);
    
    // 标识符
    if (isalpha(current) || current == '_') 
        return identifier();
    
    // 数字
    if (isdigit(current)) 
        return number();
    
    // 字符串
    if (current == '"') 
        return string();
    
    // 单字符token
    switch (current) {
        case '(': advance(); return makeToken(TokenType::TOKEN_LPAREN);
        case ')': advance(); return makeToken(TokenType::TOKEN_RPAREN);
        case '{': advance(); return makeToken(TokenType::TOKEN_LBRACE);
        case '}': advance(); return makeToken(TokenType::TOKEN_RBRACE);
        case ',': advance(); return makeToken(TokenType::TOKEN_COMMA);
        case '.': advance(); return makeToken(TokenType::TOKEN_DOT);
        case '-': advance(); return makeToken(TokenType::TOKEN_MINUS);
        case '+': advance(); return makeToken(TokenType::TOKEN_PLUS);
        case ';': advance(); return makeToken(TokenType::TOKEN_SEMICOLON);
        case '*': advance(); return makeToken(TokenType::TOKEN_MULT);
        case '/': advance(); return makeToken(TokenType::TOKEN_DIV);
        
        // 双字符token
        case '=':
            if (match('=')) return makeToken(TokenType::TOKEN_EQ);
            advance();
            return makeToken(TokenType::TOKEN_ASSIGN);
        case '~':
            if (match('=')) return makeToken(TokenType::TOKEN_NE);
            return errorToken("Unexpected character");
        case '<':
            if (match('=')) return makeToken(TokenType::TOKEN_LE);
            advance();
            return makeToken(TokenType::TOKEN_LT);
        case '>':
            if (match('=')) return makeToken(TokenType::TOKEN_GE);
            advance();
            return makeToken(TokenType::TOKEN_GT);
    }
    
    advance();
    return errorToken("Unexpected character");
}

std::vector<Token> Lexer::scanTokens() {
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        startPos = currentPos;
        Token token = getNextToken();
        tokens.push_back(token);
        
        if (token.getType() == TokenType::TOKEN_EOF) {
            break;
        }
    }
    
    return tokens;
}

bool Lexer::isAtEnd() const {
    return currentPos >= source.length();
} 