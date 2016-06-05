/* To be included in Widget.cpp */

namespace r64fx{

Widget* g_anouncer_clipboard       = nullptr;
Widget* g_anouncer_selection       = nullptr;
Widget* g_anouncer_drag_and_drop   = nullptr;

Widget* g_requestor_clipboard      = nullptr;
Widget* g_requestor_selection      = nullptr;
Widget* g_requestor_drag_and_drop  = nullptr;

Widget* g_dnd_target = nullptr;
Widget* g_dnd_object = nullptr;


inline Widget* &anouncer(ClipboardMode mode)
{
    switch(mode)
    {
        case ClipboardMode::Clipboard:
            return g_anouncer_clipboard;
        case ClipboardMode::Selection:
            return g_anouncer_selection;
        default:
            return g_anouncer_drag_and_drop;
    }
}


inline Widget* &requestor(ClipboardMode mode)
{
    switch(mode)
    {
        case ClipboardMode::Clipboard:
            return g_requestor_clipboard;
        case ClipboardMode::Selection:
            return g_requestor_selection;
        default:
            return g_requestor_drag_and_drop;
    }
}


void Widget::anounceClipboardData(const ClipboardMetadata &metadata, ClipboardMode mode)
{
    if(mode == ClipboardMode::Bad)
        return;

    auto win = rootWindow();
    if(win)
    {
        win->anounceClipboardData(metadata, mode);
        anouncer(mode) = this;
    }
}


void Widget::requestClipboardMetadata(ClipboardMode mode)
{
    if(mode == ClipboardMode::Bad)
        return;

    auto win = rootWindow();
    if(win)
    {
        win->requestClipboardMetadata(mode);
        requestor(mode) = this;
    }
}


void Widget::requestClipboardData(ClipboardDataType type, ClipboardMode mode)
{
    if(mode == ClipboardMode::Bad)
        return;

    auto win = rootWindow();
    if(win)
    {
        win->requestClipboardData(type, mode);
        requestor(mode) = this;
    }
}


void Widget::startDrag(Widget* drag_object, Point<int> anchor)
{
    if(g_dnd_object)
        return;

    auto root_window = rootWindow();
    if(root_window)
    {
        g_dnd_object = drag_object;
        if(!drag_object->isWindow())
        {
            drag_object->show(Window::WmType::DND, Window::Type::Image);
        }
        root_window->startDrag(drag_object->window(), anchor.x(), anchor.y());
    }
}

}//namespace r64fx