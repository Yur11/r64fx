#ifndef R64FX_CONFIG_H
#define R64FX_CONFIG_H

#include <string>

namespace r64fx{
    
/** @brief Program configuration. */
class Config{
    
public:
    static bool init();
    
    static std::string item(std::string key);
};

}//namespace r64fx

#endif//R64FX_CONFIG_H