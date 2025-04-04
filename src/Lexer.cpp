#include "Lexer.h"
#include <cctype>
#include <unordered_map>

static std::unordered_map<std::string, TokenType> keywords = {
    {"and", TOKEN_AND},
    {"break", TOKEN_BREAK},
    {"do", TOKEN_DO},
    {"else", TOKEN_ELSE},
    {"elseif", TOKEN_ELSEIF},
    {"end", TOKEN_END},
    {"false", TOKEN_FALSE},
    {"for", TOKEN_FOR},
    {"function", TOKEN_FUNCTION},
    {"if", TOKEN_IF},
    {"in", TOKEN_IN},
    {"local", TOKEN_LOCAL},
    {"nil", TOKEN_NIL},
    {"not", TOKEN_NOT},
    {"or", TOKEN_OR},
    {"repeat", TOKEN_REPEAT},
    {"return", TOKEN_RETURN},
    {"then", TOKEN_THEN},
    {"true", TOKEN_TRUE},
    {"until", TOKEN_UNTIL},
    {"while", TOKEN_WHILE},
    {"print", TOKEN_PRINT},
};

Lexer::Lexer(const std::string& source) 
    : source(source), currentPos(0), line(1), column(1) {}

char Lexer::peek() {
    if (currentPos >= source.length()) {
        return '\0';
    }
    return source[currentPos];
}

char Lexer::advance() {
    char current = peek();
    if (current != '\0') {
        currentPos++;
        if (current == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
    }
    return current;
}

void Lexer::skipWhitespace() {
    while (isspace(peek())) {
        advance();
    }
}

Token Lexer::readNumber() {
    std::string number;
    while (isdigit(peek()) || peek() == '.') {
        number += advance();
    }
    
    return Token{TOKEN_NUMBER, number, line, column - static_cast<int>(number.length())};
}

Token Lexer::readIdentifier() {
    std::string identifier;
    while (isalnum(peek()) || peek() == '_') {
        identifier += advance();
    }
    
    auto it = keywords.find(identifier);
    if (it != keywords.end()) {
        return Token{it->second, identifier, line, column - static_cast<int>(identifier.length())};
    }
    
    return Token{TOKEN_IDENTIFIER, identifier, line, column - static_cast<int>(identifier.length())};
}

Token Lexer::readString() {
    std::string str;
    advance(); // Skip opening quote
    
    while (peek() != '"' && peek() != '\0') {
        if (peek() == '\\') {
            advance();
            switch (peek()) {
                case 'n': str += '\n'; break;
                case 't': str += '\t'; break;
                case 'r': str += '\r'; break;
                default: str += peek(); break;
            }
        } else {
            str += peek();
        }
        advance();
    }
    
    if (peek() == '"') {
        advance(); // Skip closing quote
    }
    
    return Token{TOKEN_STRING, str, line, column - static_cast<int>(str.length()) - 2};
}

Token Lexer::getNextToken() {
    skipWhitespace();
    
    char c = peek();
    if (c == '\0') {
        return Token{TOKEN_EOF, "", line, column};
    }
    
    if (isdigit(c)) {
        return readNumber();
    }
    
    if (isalpha(c) || c == '_') {
        return readIdentifier();
    }
    
    if (c == '"') {
        return readString();
    }
    
    advance();
    switch (c) {
        case '+': return Token{TOKEN_PLUS, "+", line, column - 1};
        case '-': return Token{TOKEN_MINUS, "-", line, column - 1};
        case '*': return Token{TOKEN_MULT, "*", line, column - 1};
        case '/': return Token{TOKEN_DIV, "/", line, column - 1};
        case '(': return Token{TOKEN_LPAREN, "(", line, column - 1};
        case ')': return Token{TOKEN_RPAREN, ")", line, column - 1};
        case '=':
            if (peek() == '=') {
                advance();
                return Token{TOKEN_EQ, "==", line, column - 2};
            }
            return Token{TOKEN_ASSIGN, "=", line, column - 1};
    }
    
    // 处理错误情况
    std::string invalid(1, c);
    return Token{TOKEN_EOF, "Unexpected character: " + invalid, line, column - 1};
} 