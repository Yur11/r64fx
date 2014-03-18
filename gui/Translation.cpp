#define R64FX_GUI_TRANSLATION_IMPLEMENTATION
#include "Translation.h"
#undef R64FX_GUI_TRANSLATION_IMPLEMENTATION

#ifdef DEBUG
#include <iostream>
#endif//DEBUG

using namespace std;

namespace r64fx{
    
extern string data_prefix;
    
std::string Translation::operator()(std::string key)
{
    auto it = find(key);
    if(it != end())
        return it->second;
    
    auto file = fopen((lang_dir + key).c_str(), "r");
    if(file == nullptr)
    {
#ifdef DEBUG
        cerr << "Failed to find translation for \"" << key << "\"!\n";
        cerr << "Looking in \"" << lang_dir << "\".";
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


void Translation::loadLanguage(std::string lang)
{
//     lang_dir = data_prefix + "translations/" + lang + "/";
}
    
}//namespace r64fx