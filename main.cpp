#include "interpreter.h"
#include "parser.h"
#include "sexpression.h"
#include "token.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

int main()
{
    ifstream file("input.txt");

    stringstream buffer;
    buffer << file.rdbuf();

    string input = buffer.str();

    Parser parser;
    parser.tokenize(input);

    file.close();

    size_t position = 0;

    Interpreter interpreter;

    while (position < parser.tokens.size())
    {
        SExpression expr;
        parser.parse(expr, position);

        SExpression* result = interpreter.eval(expr);

        delete result;
    }

    return 0;
}