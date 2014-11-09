#include <iostream>
#include "util/Arg.hpp"
#include "util/Dir.hpp"

using namespace std;
using namespace r64fx;

const char* helptext = 
    "Makefile generator for r64fx.\n\n"
    "Usage: build_tool --src_dir=<path> [ --build_dir=<path> ]\n\n"
    "By default build_dir is set to current working directory.\n"
;

int main(int argc, char* argv[])
{
    Arg arg;
    arg["help"] = "0";
    arg["src_dir"] = "0";
    arg["build_dir"] = Dir::current();
    if(!arg.parse(argc, argv))
    {
        cerr << helptext;
        return 1;
    }

    if(arg["help"] != "0")
    {
        cerr << helptext;
    }
    
    Dir src_dir(arg["src_dir"]);
    if(!src_dir)
    {
        cerr << "Bad src_dir " << arg["src_dir"] << " !\n";
        return 2;
    }
    
    Dir build_dir(arg["build_dir"]);
    if(!build_dir)
    {
        cerr << "Bad build_dir " << arg["build_dir"] << " !\n";
        return 3;
    }
    
    cout << src_dir.path() << "\n";
    cout << build_dir.path() << "\n";
    
    return 0;
}