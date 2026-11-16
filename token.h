#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>

enum class TokenType
{
    Atom,
    String,
    LeftParen,
    RightParen,
    Quote
};

class Token
{
public:

    TokenType type;
    std::string value;
};

#endif
