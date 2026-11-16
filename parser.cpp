#include "parser.h"

#include <iostream>
#include <cctype>
#include <string>
#include <vector>
#include <stdexcept>

// Do I structure this alphabetically or by order things happen?
// Bof
// tokenize -> parse -> parseList

std::vector<Token> Parser::tokenize(const std::string &input)
{
    tokens.clear();
    std::string current;

    for (size_t i = 0; i < input.size(); i++)
    {
        char c = input[i];

        if (c == '\'')
        {
            if (!current.empty())
            {
                tokens.push_back({TokenType::Atom, current});
                current.clear();
            }

            tokens.push_back({TokenType::Quote, "\'"});
        }
        else if (c == '(')
        {
            if (!current.empty())
            {
                tokens.push_back({TokenType::Atom, current});
                current.clear();
            }

            tokens.push_back({TokenType::LeftParen, "("});
        }
        else if (c == ')')
        {
            if (!current.empty())
            {
                tokens.push_back({TokenType::Atom, current});
                current.clear();
            }

            tokens.push_back({TokenType::RightParen, ")"});
        } 
        else if (c == '"')
        {
            if (!current.empty())
            {
                tokens.push_back({TokenType::Atom, current});
                current.clear();
            }

            i++; // move past opening quote

            while (i < input.size() && input[i] != '"')
            {
                current += input[i];
                i++;
            }

            if (i >= input.size())
            {
                throw std::runtime_error("Unterminated string");
            }

            tokens.push_back({TokenType::String, current});
            current.clear();
        }
        else if (isspace(c))
        {
            if (!current.empty())
            {
                tokens.push_back({TokenType::Atom, current});
                current.clear();
            }
        }
        else
        {
            current += c;
        }
    }

    if (!current.empty())
    {
        tokens.push_back({TokenType::Atom, current});
    }

    return tokens;
}

void Parser::parse(SExpression& expr, size_t& position)
{

    if (position >= tokens.size())
    {
        throw std::runtime_error("Unexpected end of input");
    }

    // <EXPR> => <ATOM>
    if (tokens[position].type == TokenType::Atom)
    {
        expr.type = ExpressionType::Atom;
        expr.atom = tokens[position].value;

        position++;
        return;
    }

    if (tokens[position].type == TokenType::String)
    {
        expr.type = ExpressionType::String;
        expr.atom = tokens[position].value;
        
        position++;
        return;
    }

    // <EXPR> => ( <LIST>
    if (tokens[position].type == TokenType::LeftParen)
    {
        position++; // consume '('

        parseList(expr, position);
        return;
    }

    // expr -> '(...)
    if (tokens[position].type == TokenType::Quote)
    {
        position++; // consume

        expr.type = ExpressionType::Pair;

        expr.car = new SExpression();
        expr.car->type = ExpressionType::Atom;
        expr.car->atom = "quote";

        expr.cdr = new SExpression();
        expr.cdr->type = ExpressionType::Pair;

        expr.cdr->car = new SExpression();
        parse(*expr.cdr->car, position);

        expr.cdr->cdr = new SExpression();
        expr.cdr->cdr->type = ExpressionType::Nil;

        return;
    }

    throw std::runtime_error("Unexpected ')'");
}

void Parser::parseList(SExpression& expr, size_t& position)
{
    if (position >= tokens.size())
    {
        throw std::runtime_error("Missing ')'");
    }

    // <LIST> => )
    if (tokens[position].type == TokenType::RightParen)
    {
        expr.type = ExpressionType::Nil;
        position++; // consume ')'
        return;
    }

    // <LIST> => <EXPR> <LIST>

    expr.type = ExpressionType::Pair;

    expr.car = new SExpression();
    parse(*expr.car, position);

    expr.cdr = new SExpression();
    parseList(*expr.cdr, position);
}