#ifndef R64FX_UTIL_ARG_HPP
#define R64FX_UTIL_ARG_HPP

#include <map>
#include <string>

namespace r64fx{
    
/** @brief Command line argument parser. */
class Arg : public std::map<std::string, std::string>{
public:
    bool parse(int argc, char* argv[]);
};
    
}//namespace r64fx

#endif//R64FX_UTIL_ARG_HPP