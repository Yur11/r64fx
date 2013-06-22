#include "data_paths.h"

using namespace std;

namespace r64fx{
    
/* Directories where r64fx will look for the data. */
vector<string> ____data_paths = {
    "./",
    "~/.r64fx/",
    "/usr/share/r64fx/",
};


std::vector<std::string> &data_paths()
{
    return ____data_paths;
}
    
}//namespace r64fx