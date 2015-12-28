#ifndef R64FX_WIDGET_TEXT_HPP
#define R64FX_WIDGET_TEXT_HPP

#include "Widget.hpp"
#include "TextFlags.hpp"

namespace r64fx{

class Font;
class Image;

class Widget_Text : public Widget{
    void*         m       = nullptr;
    std::string*  m_text  = nullptr;
    Font*         m_font  = nullptr;
    Image*        m_image = nullptr;

public:
    Widget_Text(std::string* textptr, Font* font, Widget* parent = nullptr);

    Widget_Text(const std::string &text, Font* font = nullptr, Widget* parent = nullptr);

    Widget_Text(const std::string &text, Widget* parent = nullptr);

    Widget_Text(Widget* parent = nullptr);

    virtual ~Widget_Text();

    void setText(std::string *text);

    void setText(const std::string &text);

    void setFont(Font* font);

    void setFont(std::string font_name);

    bool ownsText() const;

    bool ownsFont() const;

    void setWrapMode(TextWrap::Mode wrap_mode);

    TextWrap::Mode wrapMode() const;

    void resizeToText();

protected:
    virtual void reconfigureEvent(ReconfigureEvent* event);

    virtual void focusInEvent();

    virtual void focusOutEvent();

    virtual void resizeEvent(ResizeEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);

    virtual void keyPressEvent(KeyPressEvent* event);

    virtual void keyReleaseEvent(KeyReleaseEvent* event);

    virtual void textInputEvent(TextInputEvent* event);

    virtual void closeEvent();
};

}//namespace r64fx

#endif//R64FX_WIDGET_TEXT_HPP