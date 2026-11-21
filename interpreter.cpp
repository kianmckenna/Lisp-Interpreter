#include "interpreter.h"
#include "parser.h"

#include <stdexcept>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

// global variable holder rho
SExpression* rho = new SExpression();

// local variable stack
SExpression* stack = new SExpression();

SExpression* Interpreter::makeTrue()
{
    SExpression* result = new SExpression();

    result->type = ExpressionType::Atom;
    result->atom = "T";

    return result;
}

SExpression* Interpreter::makeFalse()
{
    return new SExpression();
}
/*
void Interpreter::print(SExpression& expr) // SExpression arg
{
    bool shouldPrint = true;
    if (expr.isPair() && expr.car->isAtom())
    {
        shouldPrint = expr.printable();
    }

    if (shouldPrint)
    {
        if (expr.isAtom())
        {
            std::cout << expr.atom;
            return;
        }

        if (expr.isNil())
        {
            std::cout << "()";
            return;
        }

        if (expr.isPair())
        {
            std::cout << "(";
            printList(expr);
            std::cout << ")";
        }
    }
}
*/

void Interpreter::popStack()
{
    if (stack->isNil())
    {
        return;
    }

    SExpression* newStack = stack->cdr->clone();

    delete stack;
    stack = newStack;
}

void Interpreter::pushStack(SExpression& names, SExpression& values)
{
    SExpression nil;

    SExpression* valueList = cons(values, nil);
    SExpression* frame = cons(names, *valueList);

    SExpression* newStack = cons(*frame, *stack);
    delete stack;
    stack = newStack;

    delete frame;
    delete valueList;
}

void Interpreter::print(SExpression& expr)
{
    SExpression* current = &expr;

    while (current->isPair())
    {
        SExpression* result = current->car;

        if (result->atom == "nl")
        {
            std::cout << std::endl;

            current = current->cdr;
            continue;
        }

        result = eval(*result);

        if (result->isPair())
        {
            std::cout << "(";
            printList(*result);
            std::cout << ")";
        }
        else if (result->type == ExpressionType::String)
        {
            std::cout << result->atom;
        }
        else if (isNumber(*result))
        {
            std::cout << result->atom;
        }
        else if (result->isNil()) 
        {
            std::cout << "()";
        }
        else if (result->atom == "T")
        {
            std::cout << "T";
        }

        delete result;

        current = current->cdr;
    }
}

void Interpreter::printList(SExpression& expr)
{
    std::cout << expr.car->atom;

    SExpression* current = expr.cdr;

    while (current->isPair())
    {
        std::cout << " ";
        std::cout << current->car->atom;

        current = current->cdr;
    }
}

SExpression* Interpreter::eval(SExpression& expr)
{
    if (expr.isNil())
    {
        return expr.clone();
    }

    if (expr.type == ExpressionType::String)
    {
        return expr.clone();
    }

    if (isNumber(expr))
    {
        return expr.clone();
    }

    if (expr.isAtom())
    {
        return lookup(expr);
    }

    if (!expr.isPair())
    {
        throw std::runtime_error("Invalid expression");
    }

    if (!expr.car->isAtom())
    {
        throw std::runtime_error("Expected function name");
    }

    std::string function = expr.car->atom;

    if (function == "begin")
    {
        SExpression* current = expr.cdr;
        SExpression* result = nullptr;

        while (current->isPair())
        {
            delete result;

            result = eval(*current->car);

            current = current->cdr;
        }

        if (result == nullptr)
        {
            return makeFalse();
        }

        return result;
    }

    if (function == "charAt")
    {
        if (!expr.cdr->cdr->isPair() || expr.cdr->cdr->cdr->isPair())
        {
            throw std::runtime_error("concat takes only two arguments");
        }

        const SExpression* string = eval(*expr.cdr->car);
        int position = toNumber(eval(*expr.cdr->cdr->car));

        SExpression* result = charAt(*string, position);

        delete string;

        return result;
    }

    if (function == "concat")
    {
        if (!expr.cdr->cdr->isPair() || expr.cdr->cdr->cdr->isPair())
        {
            throw std::runtime_error("concat takes only two arguments");
        }

        const SExpression* first = eval(*expr.cdr->car);
        const SExpression* second = eval(*expr.cdr->cdr->car);

        SExpression* result = concat(*first, *second);

        delete first;
        delete second;

        return result;
    }

    if (function == "strlen")
    {
        if (expr.cdr->cdr->isPair() || expr.cdr->car->isNil())
        {
            throw std::runtime_error("strlen takes only one element");
        }

        SExpression* string = eval(*expr.cdr->car);

        SExpression* result = strlen(*string);

        delete string;

        return result;        
    }

    if (function == "streq?")
    {
        if (!expr.cdr->cdr->isPair() || expr.cdr->cdr->cdr->isPair())
        {
            throw std::runtime_error("concat takes only two arguments");
        }

        const SExpression* first = eval(*expr.cdr->car);
        const SExpression* second = eval(*expr.cdr->cdr->car);

        SExpression* result = streq(*first, *second);

        delete first;
        delete second;

        return result;
    }

    if (function == "use")
    {
        if (expr.cdr->car->type != ExpressionType::String)
        {
            throw std::runtime_error("Library name must be a string");
        }
        std::string libName = "libraries\\" + expr.cdr->car->atom + ".txt";

        std::ifstream library(libName); 

        std::stringstream buffer;
        buffer << library.rdbuf();

        std::string input = buffer.str();

        Parser parser;
        parser.tokenize(input);

        library.close();

        size_t position = 0;

        while (position < parser.tokens.size())
        {
            SExpression expr;
            parser.parse(expr, position);

            SExpression* result = eval(expr);

            delete result;
        }

        return makeFalse();
    }

    if (function == "quote")
    {
        return quote(*expr.cdr->car);
    }

    if (function == "car")
    {
        SExpression* argument = eval(*expr.cdr->car);

        SExpression* result = car(*argument);

        delete argument;

        return result;
    }

    if (function == "cdr")
    {
        SExpression* argument = eval(*expr.cdr->car);

        SExpression* result = cdr(*argument);

        delete argument;

        return result;
    }

    if (function == "cons")
    {
        SExpression* first = eval(*expr.cdr->car);

        SExpression* second = eval(*expr.cdr->cdr->car);

        SExpression* result = cons(*first, *second);

        delete first;
        delete second;

        return result;
    }

    if (function == "eval")
    {
        SExpression* argument = eval(*expr.cdr->car);

        SExpression* result = eval(*argument);

        delete argument;

        return result;
    }

    if (function == "set")
    {
        SExpression* name = expr.cdr->car;

        SExpression* value = eval(*expr.cdr->cdr->car);

        SExpression* bind = set(*name, *value);
        SExpression* newRho = cons(*bind, *rho);

        delete rho;
        rho = newRho;

        delete bind;

        return value;
    }

    if (function == "print") 
    {
        print(*expr.cdr);

        return makeFalse();
    }

    if (function == "nl")
    {
        expr.atom = "\n";

        return expr.clone();
    }

    if (function == "nil?")
    {
        SExpression* argument = eval(*expr.cdr->car);
        SExpression* result = nilPredicate(*argument);

        delete argument;

        return result;
    }

    if (function == "atom?")
    {
        SExpression* argument = eval(*expr.cdr->car);
        SExpression* result = atomPredicate(*argument);

        delete argument;

        return result;
    }

    if (function == "list?")
    {
        SExpression* argument = eval(*expr.cdr->car);
        SExpression* result = listPredicate(*argument);

        delete argument;

        return result;
    }

    if (function == "not?")
    {
        SExpression* argument = eval(*expr.cdr->car);
        SExpression* result = nilPredicate(*argument);

        delete argument;

        return result;
    }

    if (function == "number?")
    {
        SExpression* argument = eval(*expr.cdr->car);
        SExpression* result = numberPredicate(*argument);

        delete argument;

        return result;
    }

    if (function == "and?")
    {
        SExpression& first = *expr.cdr->car;
        SExpression& second = *expr.cdr->cdr->car;
        SExpression* result = andPredicate(first, second);

        return result;
    }

    if (function == "or?")
    {
        SExpression& first = *expr.cdr->car;
        SExpression& second = *expr.cdr->cdr->car;
        SExpression* result = orPredicate(first, second);

        return result;
    }

    if (function == "eq?")
    {
        SExpression* first = eval(*expr.cdr->car);
        SExpression* second = eval(*expr.cdr->cdr->car);
        SExpression* result = eqPredicate(*first, *second);

        delete first;
        delete second;

        return result;
    }

    if (function == "if")
    {
        SExpression& a0 = *expr.cdr->car;
        SExpression& a1 = *expr.cdr->cdr->car;
        SExpression& a2 = *expr.cdr->cdr->cdr->car;

       return ifPredicate(a0, a1, a2);
    }

    if (function == "while")
    {
        if (!expr.cdr->isPair()) 
        {
            throw std::runtime_error("expects (while CONDITION body iterator)");
        }
        SExpression& condition = *expr.cdr->car;

        if (!expr.cdr->cdr->isPair())
        {
            throw std::runtime_error("expects (while condition BODY iterator)");
        }
        SExpression& body = *expr.cdr->cdr->car;

        if (!expr.cdr->cdr->cdr->isPair()) 
        {
            throw std::runtime_error("expects (while condition body ITERATOR)");
        }
        SExpression& iterate = *expr.cdr->cdr->cdr->car;

        if (iterate.car->atom != "set")
        {
            throw std::runtime_error("iterator must set variable");
        }

        return whilePredicate(condition, body, iterate);
    }

    if (function == "cond")
    {
        const SExpression* list = expr.cdr->car;

        return condPredicate(*list);
    }

    if (function == "add")
    {
        SExpression& arg1 = *expr.cdr->car;
        SExpression& arg2 = *expr.cdr->cdr->car;

        SExpression* result = add(arg1, arg2);

        return result;
    }

    if (function == "sub")
    {
        SExpression& arg1 = *expr.cdr->car;
        SExpression& arg2 = *expr.cdr->cdr->car;

        SExpression* result = sub(arg1, arg2);

        return result;
    }

    if (function == "mul")
    {
        SExpression& arg1 = *expr.cdr->car;
        SExpression& arg2 = *expr.cdr->cdr->car;

        SExpression* result = mul(arg1, arg2);

        return result;
    }

    if (function == "div")
    {
        SExpression& arg1 = *expr.cdr->car;
        SExpression& arg2 = *expr.cdr->cdr->car;

        SExpression* result = div(arg1, arg2);

        return result;
    }

    if (function == "rem")
    {
        SExpression& arg1 = *expr.cdr->car;
        SExpression& arg2 = *expr.cdr->cdr->car;

        SExpression* result = rem(arg1, arg2);

        return result;
    }

    if (function == "lt")
    {
        SExpression& arg1 = *expr.cdr->car;
        SExpression& arg2 = *expr.cdr->cdr->car;

        SExpression* result = lessThan(arg1, arg2);

        return result;
    }

    if (function == "def")
    {
        SExpression* funcName = expr.cdr->car;

        SExpression nil;

        SExpression* args = expr.cdr->cdr->car;
        SExpression* body = expr.cdr->cdr->cdr->car;

        SExpression* temp = cons(*body, nil);
        SExpression* argsAndBody = cons(*args, *temp);

        SExpression functionAtom;
        functionAtom.atom = "function";
        functionAtom.type = ExpressionType::Atom;

        SExpression* def = cons(functionAtom, *argsAndBody);

        delete temp;
        delete argsAndBody;

        SExpression* bind = set(*funcName, *def);
        SExpression* newRho = cons(*bind, *rho);

        delete rho;
        rho = newRho;

        delete bind;

        return def;
    }

    if (function == "function")
    {
        return expr.clone();
    }

    SExpression* funcDef = lookup(*expr.car);

    if (funcDef->isPair() && funcDef->car->isAtom() && funcDef->car->atom == "function")
    {
        SExpression* arguments = evalArgs(*expr.cdr);

        pushStack(*funcDef->cdr->car, *arguments);

        SExpression* body = funcDef->cdr->cdr->car;

        SExpression* result = eval(*body);

        popStack();

        delete arguments;
        delete funcDef;

        return result;
    }

    delete funcDef;

    return expr.clone();
}

SExpression* Interpreter::evalArgs(SExpression& arguments)
{
    if (arguments.isNil())
    {
        return arguments.clone();
    }

    SExpression* first = eval(*arguments.car);
    SExpression* rest = evalArgs(*arguments.cdr);

    SExpression* result = cons(*first, *rest);

    delete first;
    delete rest;

    return result;
}

SExpression* Interpreter::lookup(SExpression& symbol)
{
    if (symbol.type == ExpressionType::String)
    {
        return symbol.clone();
    }

    SExpression* current = stack;

    while (current->isPair())
    {
        SExpression* frame = current->car;

        SExpression* names = frame->car;
        SExpression* values = frame->cdr->car;

        while (names->isPair() && values->isPair())
        {
            SExpression* name = names->car;
            SExpression* val = values->car;

            if (name->atom == symbol.atom)
            {
                return val->clone();
            }

            names = names->cdr;
            values = values->cdr;
        }

        current = current->cdr;
    }

    current = rho;

    while (current->isPair())
    {
        SExpression* bind = current->car;

        SExpression* name = bind->car;
        SExpression* value = bind->cdr->car;

        if (name->atom == symbol.atom)
        {
            return value->clone();
        }

        current = current->cdr;
    }

    throw std::runtime_error("Variable \'" + symbol.atom + "\' does not exist in this context");
}

SExpression* Interpreter::car(const SExpression& expr)
{
    if (expr.type != ExpressionType::Pair)
    {
        throw std::runtime_error("car expects a pair");
    }

    return expr.car->clone();
}

SExpression* Interpreter::cdr(const SExpression& expr)
{
    if (expr.type != ExpressionType::Pair)
    {
        throw std::runtime_error("cdr expects a pair");
    }

    return expr.cdr->clone();
}

SExpression* Interpreter::cons(SExpression& car, SExpression& cdr)
{
    SExpression* result = new SExpression();

    result->type = ExpressionType::Pair;

    result->car = car.clone();
    result->cdr = cdr.clone();

    return result;
}

SExpression* Interpreter::charAt(const SExpression& string, const int position)
{
    if (string.type != ExpressionType::String)
    {
        throw std::runtime_error("argument is not a string");
    }

    SExpression* result = new SExpression();

    result->atom = string.atom.at(position);
    result->type = ExpressionType::String;

    return result;
}

SExpression* Interpreter::concat(const SExpression& first, const SExpression& second)
{
    if (first.type != ExpressionType::String || second.type != ExpressionType::String)
    {
        throw std::runtime_error("arguments must be strings");
    }

    std::string atom = first.atom + second.atom;

    SExpression* result = new SExpression();

    result->atom = atom;
    result->type = ExpressionType::String;

    return result;
}

SExpression* Interpreter::strlen(const SExpression& string)
{
    if (string.type != ExpressionType::String)
    {
        throw std::runtime_error("strlen only operates on strings");
    }

    int length = string.atom.length();

    return makeNumber(length);
}

SExpression* Interpreter::streq(const SExpression& first, const SExpression& second)
{
    if (first.type != ExpressionType::String || second.type != ExpressionType::String)
    {
        throw std::runtime_error("streq only operates on strings");
    }

    if (first.atom == second.atom)
    {
        return makeTrue();
    }

    return makeFalse();
}

SExpression* Interpreter::set(SExpression& name, SExpression& value)
{
    SExpression nil;

    SExpression* val = cons(value, nil);
    SExpression* assignment = cons(name, *val);

    delete val;

    return assignment;
}

SExpression* Interpreter::quote(SExpression& expr)
{
    return expr.clone();
}

SExpression* Interpreter::andPredicate(SExpression& first, SExpression& second)
{
    SExpression* arg1 = eval(first);

    if (arg1->isNil())
    {
        delete arg1;
        return makeFalse();
    }

    SExpression* arg2 = eval(second);

    if (arg2->isNil())
    {
        delete arg1;
        delete arg2;
        return makeFalse();
    }

    delete arg1;
    delete arg2;

    return makeTrue();
}

SExpression* Interpreter::atomPredicate(const SExpression& expr)
{
    if (expr.isAtom())
    {
        return makeTrue();
    }

    return makeFalse();
}

SExpression* Interpreter::eqPredicate(const SExpression& first, const SExpression& second)
{
    if (!first.isAtom() || !second.isAtom())
    {
        return makeFalse();
    }

    if (first.atom != second.atom)
    {
        return makeFalse();
    }


    return makeTrue();
}

SExpression* Interpreter::lessThan(SExpression& arg1, SExpression& arg2)
{
    SExpression* first = eval(arg1);

    if (!isNumber(*first))
    {
        delete first;
        throw std::runtime_error("first operand is not a number");
    }

    int left = toNumber(first);

    SExpression* second = eval(arg2);

    if (!isNumber(*second))
    {
        delete first;
        delete second;

        throw std::runtime_error("second operand is not a number");
    }

    int right = toNumber(second);

    delete first;
    delete second;

    bool lessThan = left < right;

    if (lessThan)
    {
        return makeTrue();
    }

    return makeFalse();
}

SExpression* Interpreter::listPredicate(const SExpression& expr)
{
    if (expr.isPair())
    {
        return makeTrue();
    }

    return makeFalse();
}

SExpression* Interpreter::numberPredicate(const SExpression& expr)
{
    if (!expr.isAtom())
    {
        return makeFalse();
    }

    const std::string& value = expr.atom;

    if (value.empty())
    {
        return makeFalse();
    }

    size_t start = 0;

    if (value[0] == '+' || value[0] == '-')
    {
        if (value.size() == 1)
        {
            return makeFalse();
        }

        start = 1;
    }

    for (size_t i = start; i < value.size(); i++)
    {
        if (!std::isdigit(static_cast<unsigned char>(value[i])))
        {
            return makeFalse();
        }
    }

    return makeTrue();
}

SExpression* Interpreter::nilPredicate(const SExpression& expr)
{
    if (expr.isNil())
    {
        return makeTrue();
    }

    return makeFalse();
}

SExpression* Interpreter::orPredicate(SExpression& first, SExpression& second)
{
    SExpression* arg1 = eval(first);

    if (!arg1->isNil())
    {
        delete arg1;
        return makeTrue();
    }

    SExpression* arg2 = eval(second);

    if (!arg2->isNil())
    {
        delete arg1;
        delete arg2;
        return makeTrue();
    }

    delete arg1;
    delete arg2;

    return makeFalse();
}

SExpression* Interpreter::condPredicate(const SExpression& list)
{
    const SExpression* current = &list;

    while (!current->isNil())
    {
        SExpression* condition = eval(*current->car);

        if (!condition->isNil())
        {
            delete condition;

            return eval(*current->cdr->car);
        }

        delete condition;

        current = current->cdr->cdr;
    }

    throw std::runtime_error("last expression must be 'T");
}

SExpression* Interpreter::ifPredicate(SExpression& condition, SExpression& ifBody, SExpression& elseBody)
{
    SExpression* cond = eval(condition);

    if (!cond->isNil())
    {
        delete cond;
        return eval(ifBody);
    } 
    else
    {
        delete cond;
        return eval(elseBody);
    }
}

SExpression* Interpreter::whilePredicate(SExpression& condition, SExpression& body, SExpression& iterate)
{
    // check if variable has been declared in the scope
    // evaluate condition using that variable

    const SExpression* condResult = eval(condition);
    if (condResult->isNil())
    {
        delete condResult;
        return makeFalse();
    }
    delete condResult;

    // execute body

    SExpression* bodyResult = eval(body);
    delete bodyResult;

    SExpression* iterateResult = eval(iterate);
    delete iterateResult;

    return whilePredicate(condition, body, iterate);
}

SExpression* Interpreter::add(SExpression& arg1, SExpression& arg2)
{
    SExpression* first = eval(arg1);

    if (!isNumber(*first))
    {
        delete first;
        throw std::runtime_error("first operand is not a number");
    }

    int left = toNumber(first);

    SExpression* second = eval(arg2);

    if (!isNumber(*second))
    {
        delete first;
        delete second;

        throw std::runtime_error("second operand is not a number");
    }

    int right = toNumber(second);

    delete first;
    delete second;

    SExpression* result = makeNumber(left + right);

    return result;
}

SExpression* Interpreter::div(SExpression& arg1, SExpression& arg2)
{
    SExpression* first = eval(arg1);

    if (!isNumber(*first))
    {
        delete first;
        throw std::runtime_error("first operand is not a number");
    }

    int left = toNumber(first);

    SExpression* second = eval(arg2);

    if (!isNumber(*second))
    {
        delete first;
        delete second;

        throw std::runtime_error("second operand is not a number");
    }

    int right = toNumber(second);

    delete first;
    delete second;

    SExpression* result = makeNumber(left / right);

    return result;
}

SExpression* Interpreter::mul(SExpression& arg1, SExpression& arg2)
{
    SExpression* first = eval(arg1);

    if (!isNumber(*first))
    {
        delete first;
        throw std::runtime_error("first operand is not a number");
    }

    int left = toNumber(first);

    SExpression* second = eval(arg2);

    if (!isNumber(*second))
    {
        delete first;
        delete second;

        throw std::runtime_error("second operand is not a number");
    }

    int right = toNumber(second);

    delete first;
    delete second;

    SExpression* result = makeNumber(left * right);

    return result;
}

SExpression* Interpreter::sub(SExpression& arg1, SExpression& arg2)
{
    SExpression* first = eval(arg1);

    if (!isNumber(*first))
    {
        delete first;
        throw std::runtime_error("first operand is not a number");
    }

    int left = toNumber(first);

    SExpression* second = eval(arg2);

    if (!isNumber(*second))
    {
        delete first;
        delete second;

        throw std::runtime_error("second operand is not a number");
    }

    int right = toNumber(second);

    delete first;
    delete second;

    SExpression* result = makeNumber(left - right);

    return result;
}

SExpression* Interpreter::rem(SExpression& arg1, SExpression& arg2)
{
    SExpression* first = eval(arg1);

    if (!isNumber(*first))
    {
        delete first;
        throw std::runtime_error("first operand is not a number");
    }

    int left = toNumber(first);

    SExpression* second = eval(arg2);

    if (!isNumber(*second))
    {
        delete first;
        delete second;

        throw std::runtime_error("second operand is not a number");
    }

    int right = toNumber(second);

    delete first;
    delete second;

    SExpression* result = makeNumber(left % right);

    return result;
}

bool Interpreter::isNumber(const SExpression& expr)
{
    SExpression* condition = numberPredicate(expr);

    if (condition->isNil())
    {
        delete condition;
        return false;
    }

    delete condition;
    return true;
}

SExpression* Interpreter::makeNumber(const int num)
{
    SExpression* result = new SExpression();

    result->atom = std::to_string(num);
    result->type = ExpressionType::Atom;

    return result;
}

int Interpreter::toNumber(const SExpression* expr)
{
    return std::stoi(expr->atom);
}