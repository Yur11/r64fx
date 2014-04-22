#ifndef R64FX_GUI_TEXT_LINE_H
#define R64FX_GUI_TEXT_LINE_H

#include "Widget.h"
#include "Font.h"
#include "Padding.h"
#include <string>


namespace r64fx{
    
/** @brief Widget that can render text. */
class TextLine : public Widget, public Padding{
    Font* _font;
    
    Color text_color;
    
    float _max_width = 200;
    float _min_width = 10;
    
public:
    Utf8String text;
    
    enum class Alignment{
        Left,
        Right,
        Centered
    };
    
    TextLine(Utf8String text, Font* font = Font::defaultFont(), Widget* parent = nullptr) 
    : Widget(parent)
    , _font(font)
    , text_color(0.0, 0.0, 0.0)
    , text(text)
    {
    }
    
    inline Font* font() const { return _font; }
    
    inline void setTextColor(Color color) { text_color = color; }
    
    inline void setTextColor(float r, float g, float b) { text_color = Color(r, g, b); }
    
    inline Color textColor() const { return text_color; }
    
    inline void setMaxWidth(float width) { _max_width = width; }
    
    inline float maxWidth() const { return _max_width; }
    
    inline void setMinWidth(float width) { _min_width = width; }
    
    inline float minWidth() const { return _min_width; }
    
    virtual void render();
    
    /** @brief Update widget to fit the text. */
    void update();
    
    static void setDefaultFont(Font* font);
    
    static Font* defaultFont();
};
    
}//namespace r64fx

#endif//R64FX_GUI_TEXT_LINE_H