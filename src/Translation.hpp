#ifndef R64FX_GUI_TRANSLATION_H
#define R64FX_GUI_TRANSLATION_H

#include <string>
#include <map>


namespace r64fx{

class Translation : public std::map<std::string, std::string>{
    std::string lang_dir;

public:
    std::string operator()(std::string key);

    void loadLanguage(std::string lang_dir);
};


#ifndef R64FX_GUI_TRANSLATION_IMPLEMENTATION
extern
#endif//R64FX_GUI_TRANSLATION_IMPLEMENTATION
Translation tr;

}//namespace r64fx

#endif//R64FX_GUI_TRANSLATION_H