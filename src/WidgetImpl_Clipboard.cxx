/* To be included in Widget.cpp */


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