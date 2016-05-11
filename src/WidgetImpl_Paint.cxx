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

    /* List of rectangles to be repainted after update cycle. */
    Rect<int> rects[max_rects];

    /* Number of rectangles that must be repainted. */
    int num_rects = 0;

    /* Used in update logic. */
    bool got_rect = false;
};


Painter* Widget::PaintEvent::painter()
{
    auto d = (WindowWidgetData*) m;
    return d->painter;
}


void Widget::repaint()
{
    m_flags |= R64FX_WIDGET_WANTS_REPAINT;
    auto widget = parent();
    while(widget && !(widget->m_flags & R64FX_WIDGET_REPAINT_FLAGS))
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

    if(m_flags & R64FX_WIDGET_REPAINT_FLAGS)
    {
        p->adjustForWindowSize();

        d->num_rects = 0;
        d->got_rect = false;

        Widget::PaintEvent event(d);

        if(m_flags & R64FX_WIDGET_WANTS_REPAINT)
        {
            //Update whole window surface.
            paintEvent(&event);
            p->repaint();
        }
        else
        {
            //Update only certain parts of the window surface.
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

        set_bits(m_flags, false, R64FX_WIDGET_REPAINT_FLAGS);
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
    bool got_rect = d->got_rect;

    if(m_flags & R64FX_WIDGET_WANTS_REPAINT)
    {
        for(auto child : m_children)
        {
            child->m_flags |= R64FX_WIDGET_WANTS_REPAINT;
        }
    }

    for(auto child : m_children)
    {
        if(!(child->m_flags & R64FX_WIDGET_REPAINT_FLAGS))
            continue;

        /* Children should use painter in their local coord. system. */
        auto old_offset = p->offset();
        p->setOffset(old_offset + child->position() + contentOffset());

        if(child->m_flags & R64FX_WIDGET_WANTS_REPAINT)
        {
            child->paintEvent(event);

            /* Find portion of the window to update. */
            if(!got_rect)
            {
                Rect<int> rect(p->offset(), child->size());
                if(d->num_rects < max_rects)
                {
                    d->rects[d->num_rects] = rect;
                    d->num_rects++;
                }
                d->got_rect = true;
            }
        }
        else
        {
            child->paintChildren(event);
        }

        p->setOffset(old_offset);

        set_bits(child->m_flags, false, R64FX_WIDGET_REPAINT_FLAGS);
    }

    d->got_rect = got_rect;
}

}//namespace r64fx