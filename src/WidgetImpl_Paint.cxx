/* To be included in Widget.cpp */

namespace r64fx{

WidgetImpl* Widget::PaintEvent::impl() const
{
    return m_impl;
}


Painter* Widget::PaintEvent::painter() const
{
    return m_impl->painter();
}


void Widget::clip()
{
    m_flags |= R64FX_WIDGET_WANTS_CLIPPING;
    auto widget = parent();
    while(widget && !(widget->m_flags & R64FX_WIDGET_CLIP_FLAGS))
    {
        widget->m_flags |= R64FX_CHILD_WANTS_CLIPPING;
        widget = widget->parent();
    }
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


void WidgetImpl::initPaintCycle()
{
    m_window->makeCurrent();

    auto &flags = m_root_widget->m_flags;

    if(flags & R64FX_WIDGET_REPAINT_FLAGS)
    {
        m_painter->adjustForWindowSize();

        m_num_rects = 0;
        m_got_rect = false;

        if(flags & R64FX_WIDGET_WANTS_REPAINT)
        {
            //Paint whole window surface.
            Widget::PaintEvent event(this);
            m_root_widget->paintEvent(&event);
            m_painter->repaint();
        }
        else
        {
            //Paint only certain parts of the window surface.
            paintChildren(m_root_widget);
            if(m_num_rects > 0)
            {
                for(int i=0; i<m_num_rects; i++)
                {
                    auto rect = m_rects[i];
                    rect = intersection(rect, {0, 0, m_window->width(), m_window->height()});
                }

                m_painter->repaint(
                    m_rects,
                    m_num_rects
                );
            }
        }

        set_bits(flags, false, R64FX_WIDGET_REPAINT_FLAGS);
    }
}


void WidgetImpl::paintChildren(Widget* parent)
{
    bool old_got_rect = m_got_rect;

    auto &parent_flags = parent->m_flags;

    if(parent_flags & R64FX_WIDGET_WANTS_REPAINT)
    {
        for(auto child : *parent)
        {
            child->m_flags |= R64FX_WIDGET_WANTS_REPAINT;
        }
    }

    for(auto child : *parent)
    {
        if(!(child->m_flags & R64FX_WIDGET_REPAINT_FLAGS))
            continue;

        auto &child_flags = child->m_flags;

        /* Children should use painter in their local coord. system. */
        auto old_offset = m_painter->offset();
        m_painter->setOffset(old_offset + child->position() + parent->contentOffset());

        if(child_flags & R64FX_WIDGET_WANTS_REPAINT)
        {
            Widget::PaintEvent event(this);
            child->paintEvent(&event);

            /* Find portion of the window to update. */
            if(!m_got_rect)
            {
                //Painter offset is equal to the window coord. of the child widget.
                Rect<int> rect(m_painter->offset(), child->size());
                if(m_num_rects < max_rects)
                {
                    m_rects[m_num_rects] = rect;
                    m_num_rects++;
                }
                m_got_rect = true;
            }
        }
        else
        {
            paintChildren(child);
        }

        m_painter->setOffset(old_offset);

        set_bits(child_flags, false, R64FX_WIDGET_REPAINT_FLAGS);
    }

    m_got_rect = old_got_rect;
}


void WidgetImpl::clipChildren(Widget* parent)
{

}

}//namespace r64fx