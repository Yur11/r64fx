#ifndef R64FX_WIDGET_NUMBER_HPP
#define R64FX_WIDGET_NUMBER_HPP

#include "Widget.hpp"
#include "Value.hpp"

namespace r64fx{

class Widget_Number : public Widget, public Value{
public:
    Widget_Number(Widget* parent = nullptr);

    virtual ~Widget_Number();

    void enableTextEditing();

    void disableTextEditing();

    bool doingTextEditing() const;

    void insertText(const std::string &text);

    void setText(const std::string &text);

private:
    virtual void addedToWindowEvent(WidgetAddedToWindowEvent* event) override final;

    virtual void removedFromWindowEvent(WidgetRemovedFromWindowEvent* event) override final;

    virtual void paintEvent(WidgetPaintEvent* event) override final;

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
