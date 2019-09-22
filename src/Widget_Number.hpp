#ifndef R64FX_WIDGET_NUMBER_HPP
#define R64FX_WIDGET_NUMBER_HPP

#include "Widget.hpp"
#include "Value.hpp"

namespace r64fx{

class Widget_Number : public Widget, public Value, public ValueChangeCallback{
    void* m = nullptr;
    int m_text_x = 0;

public:
    Widget_Number(Widget* parent = nullptr);

    virtual ~Widget_Number();

    void beginTextEditing();

    void endTextEditing(bool commit);

    bool doingTextEditing() const;

    void setValue(float value, bool notify = false);

    void setValue(const std::string &text, bool notify = false);

private:
    void renderImage();

    virtual void addedToWindowEvent(WidgetAddedToWindowEvent* event) override final;

    virtual void removedFromWindowEvent(WidgetRemovedFromWindowEvent* event) override final;

    virtual void paintEvent(WidgetPaintEvent* event) override final;

    virtual void resizeEvent(WidgetResizeEvent* event);

    virtual void mousePressEvent(MousePressEvent* event) override final;

    virtual void mouseReleaseEvent(MouseReleaseEvent* event) override final;

    virtual void mouseMoveEvent(MouseMoveEvent* event);

    virtual void focusInEvent() override final;

    virtual void focusOutEvent() override final;

    virtual void keyPressEvent(KeyPressEvent* event) override final;

    virtual void keyReleaseEvent(KeyReleaseEvent* event) override final;

    virtual void textInputEvent(TextInputEvent* event) override final;
};

}//namespace r64fx

#endif//R64FX_WIDGET_NUMBER_HPP
