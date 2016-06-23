/* To be included in Widget.cpp */

namespace r64fx{

Widget* g_anouncer_clipboard       = nullptr;
Widget* g_anouncer_selection       = nullptr;
Widget* g_anouncer_drag_and_drop   = nullptr;

Widget* g_requestor_clipboard      = nullptr;
Widget* g_requestor_selection      = nullptr;
Widget* g_requestor_drag_and_drop  = nullptr;

Widget* g_requestor_anouncer_stub  = nullptr;

Widget* g_dnd_target = nullptr;
Widget* g_dnd_object = nullptr;
Widget* g_dnd_source = nullptr;


inline Widget* &anouncer(ClipboardMode mode)
{
    switch(mode)
    {
        case ClipboardMode::Clipboard:
            return g_anouncer_clipboard;
        case ClipboardMode::Selection:
            return g_anouncer_selection;
        case ClipboardMode::DragAndDrop:
            return g_anouncer_drag_and_drop;
        default:
            return g_requestor_anouncer_stub;
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
        case ClipboardMode::DragAndDrop:
            return g_requestor_drag_and_drop;
        default:
            return g_requestor_anouncer_stub;
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
        requestor(mode) = this;
        win->requestClipboardMetadata(mode);
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


void Widget::startDrag(Widget* dnd_object, Point<int> anchor)
{
    if(g_dnd_object)
        return;

    auto root_window = rootWindow();
    if(root_window)
    {
        g_dnd_object = dnd_object;
        g_dnd_source = this;
        if(!dnd_object->isWindow())
        {
            dnd_object->show(Window::WmType::DND, Window::Type::Image);
        }
        root_window->startDrag(dnd_object->window(), anchor.x(), anchor.y());
    }
}

}//namespace r64fx