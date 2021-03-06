#ifndef R64FX_WIDGET_TEXT_HPP
#define R64FX_WIDGET_TEXT_HPP

#include "Widget.hpp"
#include "Padding.hpp"
#include "TextFlags.hpp"

namespace r64fx{

class Font;
class Image;

class Widget_Text : public Widget, public Padding<int>{
    void*         m[2]    = {nullptr, nullptr};
    Image*        m_image = nullptr;

public:
    Widget_Text(const std::string &text, Font* font = nullptr, Widget* parent = nullptr);

    Widget_Text(const std::string &text, Widget* parent = nullptr);

    Widget_Text(Font* font, Widget* parent = nullptr);

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
    virtual void paintEvent(WidgetPaintEvent* event);

    virtual void focusInEvent();

    virtual void focusOutEvent();

    virtual void resizeEvent(WidgetResizeEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);

    virtual void keyPressEvent(KeyPressEvent* event);

    virtual void keyReleaseEvent(KeyReleaseEvent* event);

    virtual void textInputEvent(TextInputEvent* event);

    virtual void clipboardDataRecieveEvent(ClipboardDataRecieveEvent* event);

    virtual void clipboardDataTransmitEvent(ClipboardDataTransmitEvent* event);

    virtual void clipboardMetadataRecieveEvent(ClipboardMetadataRecieveEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_TEXT_HPP
