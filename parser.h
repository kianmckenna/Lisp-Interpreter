#ifndef PARSER_H
#define PARSER_H

#include "sexpression.h"
#include "token.h"

#include <string>
#include <vector>

class Parser {
    public: 
        std::vector<Token> tokens;

        std::vector<Token> tokenize(const std::string &input);

        void parse(SExpression& expr, size_t& position);

    private:
        void parseList(SExpression& expr, size_t& position);
};

#endif