#pragma once
#include <string>
#include <vector>

enum TokenType {
    // 关键字
    TOKEN_AND,
    TOKEN_BREAK,
    TOKEN_DO,
    TOKEN_ELSE,
    TOKEN_ELSEIF,
    TOKEN_END,
    TOKEN_FALSE,
    TOKEN_FOR,
    TOKEN_FUNCTION,
    TOKEN_IF,
    TOKEN_IN,
    TOKEN_LOCAL,
    TOKEN_NIL,
    TOKEN_NOT,
    TOKEN_OR,
    TOKEN_REPEAT,
    TOKEN_RETURN,
    TOKEN_THEN,
    TOKEN_TRUE,
    TOKEN_UNTIL,
    TOKEN_WHILE,
    
    // 运算符和分隔符
    TOKEN_PLUS,    // +
    TOKEN_MINUS,   // -
    TOKEN_MULT,    // *
    TOKEN_DIV,     // /
    TOKEN_ASSIGN,  // =
    TOKEN_EQ,      // ==
    TOKEN_LPAREN,  // (
    TOKEN_RPAREN,  // )
    
    // 其他
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_EOF
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

class Lexer {
public:
    Lexer(const std::string& source);
    Token getNextToken();
    
private:
    std::string source;
    size_t currentPos;
    int line;
    int column;
    
    char peek();
    char advance();
    void skipWhitespace();
    Token readNumber();
    Token readIdentifier();
    Token readString();
}; 