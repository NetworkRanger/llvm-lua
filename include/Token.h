#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    TOKEN_EOF = 0,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULT,
    TOKEN_DIV,
    TOKEN_NUMBER,
    TOKEN_PRINT,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_NIL,
    TOKEN_SEMICOLON,
    TOKEN_ERROR,
    TOKEN_NOT,
    TOKEN_IF,
    TOKEN_THEN,
    TOKEN_ELSE,
    TOKEN_ELSEIF,
    TOKEN_END,
    TOKEN_WHILE,
    TOKEN_DO,
    TOKEN_REPEAT,
    TOKEN_UNTIL,
    TOKEN_FUNCTION,
    TOKEN_RETURN,
    TOKEN_LOCAL,
    TOKEN_ASSIGN,
    TOKEN_COMMA,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_EQ,
    TOKEN_DOT,    // 点操作符 '.'
    TOKEN_NE,     // 不等于 '~='
    TOKEN_LE,     // 小于等于 '<='
    TOKEN_LT,     // 小于 '<'
    TOKEN_GE,     // 大于等于 '>='
    TOKEN_GT,     // 大于 '>'
    TOKEN_TRUE,   // true
    TOKEN_FALSE,  // false
    TOKEN_BREAK,  // break
    TOKEN_FOR,    // for
    TOKEN_IN,     // in
    TOKEN_LBRACE, // {
    TOKEN_RBRACE  // }
};

class Token {
private:
    TokenType type;
    std::string value;
    int line;
    int column;
    
public:
    // 默认构造函数
    Token() : type(TokenType::TOKEN_EOF), value(""), line(1), column(1) {}
    
    // 只有类型的构造函数
    Token(TokenType t) : type(t), value(""), line(1), column(1) {}
    
    // 完整的构造函数
    Token(TokenType type, const std::string& value, int line, int column)
        : type(type), value(value), line(line), column(column) {}
    
    TokenType getType() const { return type; }
    const std::string& getValue() const { return value; }
    int getLine() const { return line; }
    int getColumn() const { return column; }
};

#endif // TOKEN_H 