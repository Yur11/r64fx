#define R64FX_GUI_TRANSLATION_IMPLEMENTATION
#include "Translation.hpp"
#undef R64FX_GUI_TRANSLATION_IMPLEMENTATION

#ifdef DEBUG
#include <iostream>
#endif//DEBUG

using namespace std;

namespace r64fx{
        
std::string Translation::operator()(std::string key)
{
    auto it = find(key);
    if(it != end())
        return it->second;
    
    auto file = fopen((lang_dir + key).c_str(), "r");
    if(file == nullptr)
    {
#ifdef DEBUG
        cerr << "Looking in \"" << lang_dir << "\".\n";
        cerr << "Failed to find translation for \"" << key << "\"!\n";
#endif//DEBUG
        operator[](key) = key;
        return key;
    }
    
    auto &str = operator[](key);
    char ch;
    while(fread(&ch, 1, 1, file) == 1)
    {
        str.push_back(ch);
    }
    fclose(file);
    
    return str;
}


void Translation::loadLanguage(std::string lang_dir)
{
    this->lang_dir = lang_dir;
}
    
}//namespace r64fx