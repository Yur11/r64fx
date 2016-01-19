#ifndef R64FX_WIDGET_TEXT_HPP
#define R64FX_WIDGET_TEXT_HPP

#include "Widget.hpp"
#include "TextFlags.hpp"

namespace r64fx{

class Font;
class Image;

class Widget_Text : public Widget{
    void*         m[2]    = {nullptr, nullptr};
    Image*        m_image = nullptr;

public:
    Widget_Text(const std::string &text, Font* font = nullptr, Widget* parent = nullptr);

    Widget_Text(const std::string &text, Widget* parent = nullptr);

    Widget_Text(Widget* parent = nullptr);

private:
    void initUndoRedoChain();

public:
    virtual ~Widget_Text();

    void setText(const std::string &text);

    void insertText(const std::string &text);

    void deleteAtCursorPosition(bool backspace);

    void setFont(Font* font);

    void setFont(std::string font_name);

    bool ownsText() const;

    bool ownsFont() const;

    void setTextWrap(TextWrap text_wrap);

    TextWrap textWrap() const;

    void setTextAlignment(TextAlignment alignment);

    TextAlignment textAlignment() const;

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

    virtual void clipboardInputEvent(ClipboardEvent* event);

    virtual void closeEvent();

};

}//namespace r64fx

#endif//R64FX_WIDGET_TEXT_HPP