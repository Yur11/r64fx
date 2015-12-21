#ifndef R64FX_WIDGET_TEXT_HPP
#define R64FX_WIDGET_TEXT_HPP

#include "Widget.hpp"
#include "TextPainter.hpp"

namespace r64fx{

class Widget_Text : public Widget{
    TextPainter m_text_painter;

public:
    Widget_Text(std::string* textptr, Font* font, Widget* parent = nullptr);

    Widget_Text(const std::string &text, Font* font = nullptr, Widget* parent = nullptr);

    Widget_Text(Widget* parent = nullptr);

    virtual ~Widget_Text();

    void setText(std::string *text);

    void setText(const std::string &text);

    void setFont(Font* font);

    void setFont(std::string font_name);

    bool ownsText() const;

    bool ownsFont() const;

protected:
    virtual void reconfigureEvent(ReconfigureEvent* event);

    virtual void focusInEvent();

    virtual void focusOutEvent();

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);

    virtual void keyPressEvent(KeyPressEvent* event);

    virtual void keyReleaseEvent(KeyReleaseEvent* event);

    virtual void textInputEvent(TextInputEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_TEXT_HPP