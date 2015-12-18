#ifndef R64FX_WIDGET_TEXT_HPP
#define R64FX_WIDGET_TEXT_HPP

#include "Widget.hpp"

namespace r64fx{

class Font;

class Widget_Text : public Widget{
    std::string*  m_text = nullptr;
    Font*         m_font = nullptr;

public:
    Widget_Text(std::string* textptr, Font* font, Widget* parent = nullptr);

    Widget_Text(const std::string &text, Font* font, Widget* parent = nullptr);

    Widget_Text(Widget* parent = nullptr);

    virtual ~Widget_Text();

    void free();

    bool ownsData() const;

    void setText(std::string *text);

    void setText(const std::string &text);

    void setFont(Font* font);

protected:
    virtual void reconfigureEvent(ReconfigureEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);

    virtual void keyPressEvent(KeyEvent* event);

    virtual void keyReleaseEvent(KeyEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_TEXT_HPP