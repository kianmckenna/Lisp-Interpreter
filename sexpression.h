#ifndef SEXPRESSION_H
#define SEXPRESSION_H

#include <string>

enum class ExpressionType
{
    Atom,
    String,
    Pair,
    Nil
};

class SExpression
{
public:
    ExpressionType type = ExpressionType::Nil;

    std::string atom;

    SExpression* car = nullptr;
    SExpression* cdr = nullptr;

    SExpression* clone();

    ~SExpression();

    bool isAtom() const;
    bool isNil() const;
    bool isPair() const;

    bool printable();
};

#endif