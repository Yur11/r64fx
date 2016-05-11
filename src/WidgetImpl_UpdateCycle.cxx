/* To be included in Widget.cpp */

namespace r64fx{

/* Maximum number of individual rectangles
* that can be repainted during a singe update cycle. */
constexpr int max_rects = 16;

/* Collection of data attached to the window.
    * We should be able to cast back and forth
    * between WindowWidgetData and Window::UpdateEvent. */
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


Painter* Widget::UpdateEvent::painter()
{
    auto d = (WindowWidgetData*) m;
    return d->painter;
}


Rect<int> Widget::UpdateEvent::visibleRect()
{
    auto d = (WindowWidgetData*) m;
    return d->visible_rect;
}


void Widget::update()
{
    m_flags |= R64FX_WIDGET_WANTS_UPDATE;
    auto widget = parent();
    while(widget && !(widget->m_flags & R64FX_WIDGET_UPDATE_FLAGS))
    {
        widget->m_flags |= R64FX_CHILD_WANTS_UPDATE;
        widget = widget->parent();
    }
}


void Widget::performUpdates()
{
    if(!isWindow())
        return;

    window()->makeCurrent();

    auto d        = (WindowWidgetData*) window()->data();
    auto p  = d->painter;

    if(m_flags & R64FX_WIDGET_UPDATE_FLAGS)
    {
        p->adjustForWindowSize();
        d->num_rects = 0;
        d->visible_rect = {0, 0, width(), height()};
        d->got_rect = false;
        d->global_position = {0, 0};

        Widget::UpdateEvent event(d);

        if(m_flags & R64FX_WIDGET_WANTS_UPDATE)
        {
            updateEvent(&event);
            p->repaint();
        }
        else
        {
            updateChildren(&event);
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


void Widget::updateEvent(Widget::UpdateEvent* event)
{
    updateChildren(event);
}


void Widget::updateChildren(Widget::UpdateEvent* event)
{
    auto d = (WindowWidgetData*) event->m;
    auto p = d->painter;
    auto visible_rect = d->visible_rect;

    if(m_flags & R64FX_WIDGET_WANTS_UPDATE)
    {
        for(auto child : m_children)
        {
            child->m_flags |= R64FX_WIDGET_WANTS_UPDATE;
        }
    }

    for(auto child : m_children)
    {
        if(!(child->m_flags & R64FX_WIDGET_UPDATE_FLAGS))
            continue;

        auto global_position = d->global_position;
        auto child_global_position = global_position + child->position();

        Rect<int> child_global_rect(child_global_position, child->size());
        Rect<int> child_visible_rect = intersection(visible_rect, child_global_rect);

        d->global_position = child_global_position;
        p->setOffset(child_global_position);

        child->updateEvent(event);

        p->setOffset(global_position);
        d->global_position = global_position;

        set_bits(child->m_flags, false, R64FX_WIDGET_UPDATE_FLAGS);
    }

//     auto d = (WindowWidgetData*) event;
//     auto parent_visible_rect = d->visible_rect;
//     bool got_rect = d->got_rect;
//
//     Point<int> content_offset = contentOffset();
//
//     if(m_flags & R64FX_WIDGET_WANTS_UPDATE)
//     {
//         /* Calculate a rectangle to process on the window surface
//            if we don't aleady have one for one of the parents. */
//         if(!d->got_rect)
//         {
//             if(d->num_rects < max_rects)
//             {
//                 d->rects[d->num_rects] = intersection(
//                     Rect<int>(d->painter->offset(), size()), d->painter->clipRect()
//                 );
//                 d->num_rects++;
//                 d->got_rect = true;
//             }
//         }
//
//         /* Calculate children visibility. */
//         for(auto child : m_children)
//         {
//             auto visible_rect = intersection(child->rect(), parent_visible_rect);
//             if(visible_rect.width() > 0 && visible_rect.height() > 0)
//             {
//                 child->m_flags |= R64FX_WIDGET_IS_VISIBLE;
//                 child->m_flags |= R64FX_WIDGET_WANTS_UPDATE;
// //                 cout << "child: " << child << " -> 1\n";
//             }
//             else
//             {
//                 child->m_flags &= ~R64FX_WIDGET_IS_VISIBLE;
// //                 cout << "child: " << child << " -> 0\n";
//             }
// //             cout << "    " << visible_rect << " : " << child->rect() << " :: " << parent_visible_rect << "\n";
//         }
//     }
//
//     /* Recursively process children. */
//     for(auto child : m_children)
//     {
//         if(child->isVisible() && (child->m_flags & R64FX_WIDGET_UPDATE_FLAGS))
//         {
//             auto old_offset = d->painter->offset();
//
//             d->painter->setOffset(
//                 old_offset + child->position() + content_offset
//             );
//
//             auto visible_rect = intersection(child->rect(), parent_visible_rect);
//             d->visible_rect = {0, 0, visible_rect.width(), visible_rect.height()};
//
//             if((child->m_flags & R64FX_WIDGET_WANTS_UPDATE))
//             {
//                 child->updateEvent((UpdateEvent*)d);
//             }
//             else
//             {
//                 child->updateChildren((UpdateEvent*)d);
//             }
//
//             d->painter->setOffset(old_offset);
//         }
//     }
//
//     d->got_rect = got_rect;
//     d->visible_rect = parent_visible_rect;
//     set_bits(m_flags, false, R64FX_WIDGET_UPDATE_FLAGS);
}

}//namespace r64fx