#include "AlgebraUtils.hpp"
#include <iostream>
#include <string>

using namespace std;
using namespace r64fx;


struct Variable{
    std::string name = "";

    Variable(){}

    Variable(const std::string &name) : name(name) {}
};


template<typename StreamT> StreamT &operator<<(StreamT &stream, const Variable &variable)
{
    stream << variable.name;
    return stream;
}


int main()
{
    Variable ImaginaryUnit("i");

    Variable x("x");
    Variable y("y");

    for(int i=-8; i<9; i++)
    {
        Polynomial<Variable*> a;
        a += Term<Variable*>(&ImaginaryUnit, i);

        cout << a << "\n";
        simplify_imaginary(a, &ImaginaryUnit);
        cout << a << "\n";
    }

    return 0;
}