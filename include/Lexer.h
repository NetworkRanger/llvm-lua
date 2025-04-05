#pragma once
#include "Token.h"
#include <string>
#include <vector>
#include <map>

class Lexer {
private:
    std::string source;
    size_t currentPos = 0;
    size_t startPos = 0;
    size_t line = 1;
    size_t column = 1;
    char current = '\0';
    
    static std::map<std::string, TokenType> keywords;
    void initKeywords();
    void skipComment();
    
    bool isAtEnd() const;
    char advance();
    char peek() const;
    bool match(char expected);
    void skipWhitespace();
    Token number();
    Token identifier();
    Token string();
    Token makeToken(TokenType type) const;
    Token makeToken(TokenType type, const std::string& value) const;
    Token errorToken(const std::string& message) const;

public:
    explicit Lexer(const std::string& source);
    std::vector<Token> scanTokens();
    Token getNextToken();
}; 