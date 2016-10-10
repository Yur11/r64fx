#include "Program.hpp"

using namespace r64fx;

int main(int argc, char* argv[])
{
    Program program(argc, argv);
    
    return program.exec();
}
