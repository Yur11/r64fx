#include "SystemUtils.hpp"
#include <cstdlib>

namespace r64fx{
    
std::string get_env(const std::string &var)
{
    if(var.empty())
        return "";
    
    char* val = getenv(var.c_str());
    if(!val)
        return "";
    
    return std::string(val);
}
    
std::string home_dir()
{
    return get_env("HOME");
}
    
}//namespace r64fx
