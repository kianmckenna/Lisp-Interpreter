#include "sexpression.h"

bool SExpression::printable()
{
    if (car->atom == "set" || car->atom == "def" || car->atom == "function" || car->atom == "use")
    {
        return false;
    }

    return true;
}

SExpression::~SExpression()
{
    delete car;
    delete cdr;
}

bool SExpression::isAtom() const
{
    return type == ExpressionType::Atom;
}

bool SExpression::isNil() const
{
    return type == ExpressionType::Nil;
}

bool SExpression::isPair() const
{
    return type == ExpressionType::Pair;
}

SExpression* SExpression::clone() // SExpression arg
{
    SExpression* copy = new SExpression();

    copy->type = type;
    copy->atom = atom;

    if (car != nullptr)
    {
        copy->car = car->clone();
    }

    if (cdr != nullptr)
    {
        copy->cdr = cdr->clone();
    }

    return copy;
}
