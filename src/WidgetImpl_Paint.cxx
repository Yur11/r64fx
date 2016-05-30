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


void WidgetImpl::clip()
{
    if(m_root_widget->m_flags & R64FX_WIDGET_CLIP_FLAGS)
    {
        m_parent_global_position = {0, 0};
        m_visible_rect = Rect<int>({0, 0}, m_root_widget->size());
        clipChildren(m_root_widget);
    }
    m_root_widget->m_flags &= ~R64FX_WIDGET_CLIP_FLAGS;
}


void WidgetImpl::clipChildren(Widget* parent)
{
    Point<int> offset = parent->contentOffset();

    for(auto child : *parent)
    {
        if(parent->m_flags & R64FX_WIDGET_WANTS_CLIPPING)
            child->m_flags |= R64FX_WIDGET_WANTS_CLIPPING;

        if(!(child->m_flags & R64FX_WIDGET_CLIP_FLAGS))
            continue;

        Point<int> old_parent_global_position = m_parent_global_position;
        Rect<int> old_visible_rect = m_visible_rect;

        Point<int> child_global_position = m_parent_global_position + offset + child->position();

        Rect<int> child_global_rect(child_global_position, child->size());
        Rect<int> rect = intersection(child_global_rect, m_visible_rect);

        if(child->m_flags & R64FX_WIDGET_WANTS_CLIPPING)
        {
            Widget::ClipEvent event(rect - child_global_position);
            child->clipEvent(&event);

            if(rect.width() > 0 && rect.height() > 0)
            {
                child->m_flags |= R64FX_WIDGET_IS_VISIBLE;

                m_parent_global_position = child_global_position;
                m_visible_rect = rect;
                clipChildren(child);
            }
            else
            {
                child->m_flags &= ~R64FX_WIDGET_IS_VISIBLE;
            }
        }
        else
        {
            m_parent_global_position = child_global_position;
            m_visible_rect = rect;
            clipChildren(child);
        }

        m_parent_global_position = old_parent_global_position;
        m_visible_rect = old_visible_rect;

        child->m_flags &= ~R64FX_WIDGET_CLIP_FLAGS;
    }
}


void Widget::clipEvent(ClipEvent* event)
{

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


void WidgetImpl::repaint()
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

        flags &= ~R64FX_WIDGET_REPAINT_FLAGS;
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
        if(!(child->m_flags & R64FX_WIDGET_REPAINT_FLAGS) || !child->isVisible())
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

        child_flags &= ~R64FX_WIDGET_REPAINT_FLAGS;
    }

    m_got_rect = old_got_rect;
}


}//namespace r64fx