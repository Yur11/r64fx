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

    Widget_Text(const std::string &text, Widget* parent = nullptr);

    Widget_Text(Widget* parent = nullptr);

    virtual ~Widget_Text();

    void setText(std::string *text);

    void setText(const std::string &text);

    void setFont(Font* font);

    void setFont(std::string font_name);

    bool ownsText() const;

    bool ownsFont() const;

    void reflow(TextWrap wrap_mode);

    void reflow(TextWrap wrap_mode, int width);

    void resizeToText();

    inline int lineCount() const { return m_text_painter.lineCount(); };

    inline Size<int> textSize() const { return m_text_painter.textSize(); }

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