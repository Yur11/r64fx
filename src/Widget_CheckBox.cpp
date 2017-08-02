#include "Widget_CheckBox.hpp"
#include "WidgetFlags.hpp"
#include "IconSupply.hpp"
#include "Painter.hpp"


namespace r64fx{

namespace{

void g_on_check_stub(bool checked, Widget_CheckBox*, void*) {}

PainterTexture2D* g_check_texture = nullptr;
long g_check_texture_user_count = 0;

}//namespace


Widget_CheckBox::Widget_CheckBox(Widget* parent) : Widget(parent)
{
    setSize({16, 16});
    onCheck(nullptr);
}


void Widget_CheckBox::check(bool notify)
{
    m_flags |= R64FX_WIDGET_IS_CHECKED;
    if(notify)
        m_on_check(isChecked(), this, m_on_check_data);
}


void Widget_CheckBox::uncheck(bool notify)
{
    m_flags &= ~R64FX_WIDGET_IS_CHECKED;
    if(notify)
        m_on_check(isChecked(), this, m_on_check_data);
}


bool Widget_CheckBox::isChecked() const
{
    return m_flags & R64FX_WIDGET_IS_CHECKED;
}


void Widget_CheckBox::onCheck(void (*on_check)(bool checked, Widget_CheckBox*, void*), void* on_check_data)
{
    if(on_check)
    {
        m_on_check = on_check;
        m_on_check_data = on_check_data;
    }
    else
    {
        m_on_check = g_on_check_stub;
        m_on_check_data = nullptr;
    }
}


void Widget_CheckBox::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();
    p->strokeRect({0, 0, width(), height()}, Color(31, 31, 31), Color(190, 190, 190), 2);
    if(isChecked())
    {
        p->putImage(g_check_texture, {0, 0});
    }
}


void Widget_CheckBox::addedToWindowEvent(WidgetAddedToWindowEvent* event)
{
    if(g_check_texture_user_count == 0)
    {
        IconColors ic;
        Image* img = get_icon(IconName::Check, 16, &ic);
        g_check_texture = event->textureManager()->newTexture(img);
        free_icon(&img);
    }
    g_check_texture_user_count++;
}


void Widget_CheckBox::removedFromWindowEvent(WidgetRemovedFromWindowEvent* event)
{
    g_check_texture_user_count--;
    if(g_check_texture_user_count == 0)
    {
        event->textureManager()->deleteTexture(g_check_texture);
        g_check_texture = nullptr;
    }
}


void Widget_CheckBox::mousePressEvent(MousePressEvent* event)
{
    if(event->button() == MouseButton::Left())
    {
        if(isChecked())
            uncheck(true);
        else
            check(true);
        repaint();
    }
}

}//namespace r64fx
