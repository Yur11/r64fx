/* To be included in Widget.cpp */

namespace r64fx{

/* Maximum number of individual rectangles
* that can be repainted during a singe update cycle. */
constexpr int max_rects = 16;

/* Collection of data attached to the window.
    * We should be able to cast back and forth
    * between WindowWidgetData and Window::PaintEvent. */
struct WindowWidgetData{

    /* Root widget shown in the window that
        * this context is attached to. */
    Widget*  root_widget = nullptr;

    /* Painter serving the window. */
    Painter* painter = nullptr;

    /* Updated widget's position in root coordinates. */
    Point<int> widget_root_pos;

    /* Current visible rect. passed to widget update method. */
    Rect<int> visible_rect;

    /* List of rectangles to be repainted after update cycle. */
    Rect<int> rects[max_rects];

    /* Number of rectangles that must be repainted. */
    int num_rects = 0;

    /* Used in update logic. */
    bool got_rect = false;

    Point<int> global_position;
};


Painter* Widget::PaintEvent::painter()
{
    auto d = (WindowWidgetData*) m;
    return d->painter;
}


Rect<int> Widget::PaintEvent::visibleRect()
{
    auto d = (WindowWidgetData*) m;
    return d->visible_rect;
}


void Widget::repaint()
{
    m_flags |= R64FX_WIDGET_WANTS_REPAINT;
    auto widget = parent();
    while(widget && !(widget->m_flags & R64FX_WIDGET_UPDATE_FLAGS))
    {
        widget->m_flags |= R64FX_CHILD_WANTS_REPAINT;
        widget = widget->parent();
    }
}


void Widget::performUpdates()
{
    if(!isWindow())
        return;

    window()->makeCurrent();

    auto d = (WindowWidgetData*) window()->data();
    auto p = d->painter;

    if(m_flags & R64FX_WIDGET_UPDATE_FLAGS)
    {
        p->adjustForWindowSize();
        d->num_rects = 0;
        d->visible_rect = {0, 0, width(), height()};
        d->got_rect = false;
        d->global_position = {0, 0};

        Widget::PaintEvent event(d);

        if(m_flags & R64FX_WIDGET_WANTS_REPAINT)
        {
            cout << "repaint A\n";
            paintEvent(&event);
            p->repaint();
        }
        else
        {
            cout << "repaint B\n";
            cout << d->num_rects << "\n";
            paintChildren(&event);
            if(d->num_rects > 0)
            {
                for(int i=0; i<d->num_rects; i++)
                {
                    auto rect = d->rects[i];
                    rect = intersection(rect, {0, 0, window()->width(), window()->height()});
                }

                p->repaint(
                    d->rects,
                    d->num_rects
                );
            }
        }

        set_bits(m_flags, false, R64FX_WIDGET_UPDATE_FLAGS);
    }
}


void Widget::paintEvent(Widget::PaintEvent* event)
{
    paintChildren(event);
}


void Widget::paintChildren(Widget::PaintEvent* event)
{
    auto d = (WindowWidgetData*) event->m;
    auto p = d->painter;

    if(m_flags & R64FX_WIDGET_WANTS_REPAINT)
    {
        for(auto child : m_children)
        {
            child->m_flags |= R64FX_WIDGET_WANTS_REPAINT;
        }
    }

    for(auto child : m_children)
    {
        if(!(child->m_flags & R64FX_WIDGET_UPDATE_FLAGS))
            continue;

        auto old_offset = p->offset();
        p->setOffset(old_offset + child->position() + contentOffset());

        child->paintEvent(event);

        p->setOffset(old_offset);

        set_bits(child->m_flags, false, R64FX_WIDGET_UPDATE_FLAGS);
    }
}

}//namespace r64fx