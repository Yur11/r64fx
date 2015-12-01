#ifndef R64FX_FONT_HPP
#define R64FX_FONT_HPP

#include <string>

namespace r64fx{

class Font{
public:
    virtual ~Font(){};

    virtual bool isGood() = 0;

    virtual int glyphCount() = 0;

    virtual void setSize(int char_width, int char_height, int horz_res, int vert_res) = 0;

    virtual void setText(std::string text) = 0;

    static Font* newInstance(std::string name = "");

    static void deleteInstance(Font*);
};

}//namespace r64fx

#endif//R64FX_FONT_HPP