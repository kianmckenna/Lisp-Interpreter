#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "sexpression.h"

#include <iostream>

class Interpreter {
    public:
        std::ostream* output = &std::cout;

        SExpression* eval(SExpression& expr);

    private:
        SExpression* makeTrue();
        SExpression* makeFalse();

        // environment
        void varToCurrentFrame(SExpression& name, SExpression& value);
        void popStack();
        void pushStack();
        void bindVariables(SExpression& variables);
        void initializeStack(SExpression& names, SExpression& values);

        // IO
        void print(SExpression& expr);
        void printList(SExpression& expr);

        // evaluator
        SExpression* evalArgs(SExpression& arguments);
        void evalVars(SExpression& variables);

        SExpression* lookup(SExpression& symbol);

        // each of these sections could even go in their own subclasses
        // or am i taking organization too far

        // functions
        SExpression* car(const SExpression& expr);
        SExpression* cdr(const SExpression& expr);
        SExpression* cons(SExpression& car, SExpression& cdr);
        SExpression* set(SExpression& name, SExpression& value);
        SExpression* quote(SExpression& expr);

        // Strings
        SExpression* charAt(const SExpression& string, const int position);
        SExpression* concat(const SExpression& first, const SExpression& second);
        SExpression* streq(const SExpression& first, const SExpression& second);
        SExpression* strlen(const SExpression& string);

        // conditions
        SExpression* andPredicate(SExpression& first, SExpression& second);
        SExpression* atomPredicate(const SExpression& expr);
        SExpression* eqPredicate(const SExpression& first, const SExpression& second);
        SExpression* nilPredicate(const SExpression& expr);
        SExpression* lessThan(SExpression& first, SExpression& second);
        SExpression* listPredicate(const SExpression& expr);
        SExpression* numberPredicate(const SExpression& expr);
        SExpression* orPredicate(SExpression& first, SExpression& second);

        // control flow
        SExpression* condPredicate(const SExpression& list);
        SExpression* ifPredicate(SExpression& condition, SExpression& ifBody, SExpression& elseBody);
        SExpression* whilePredicate(SExpression& condition, SExpression& body);

        // helpers
        bool isNumber(const SExpression& expr);
        SExpression* makeNumber(const int num);
        int toNumber(const SExpression* expr);

        // math
        SExpression* add(SExpression& arg1, SExpression& arg2);
        SExpression* div(SExpression& arg1, SExpression& arg2);
        SExpression* mul(SExpression& arg1, SExpression& arg2);
        SExpression* sub(SExpression& arg1, SExpression& arg2);
        SExpression* rem(SExpression& arg1, SExpression& arg2);
};

#endif