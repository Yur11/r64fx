#include "Widget_AudioPlayer.hpp"
#include "Painter.hpp"
#include "Mouse.hpp"
#include "Keyboard.hpp"
#include "KeyboardModifiers.hpp"
#include "KeyEvent.hpp"
#include "Clipboard.hpp"
#include "ClipboardEvent.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

namespace{
    std::string extract_file_path_from_uri_list(const std::string &uri_list)
    {
        static const std::string prefix = "file:///";

        if(uri_list.size() <= prefix.size())
            return "";

        for(int i=0; i<int(prefix.size()); i++)
        {
            if(uri_list[i] != prefix[i])
                return "";
        }

        return std::string(uri_list, prefix.size(), uri_list.size() - prefix.size());
    }

}//namespace

Widget_AudioPlayer::Widget_AudioPlayer(Widget* parent)
: Widget(parent)
{

}


void Widget_AudioPlayer::paintEvent(PaintEvent* event)
{
    auto p = event->painter();

    unsigned char bg[4] = {127, 63, 31, 0};
    unsigned char fg[4] = {199, 99, 49, 0};

    p->fillRect({0, 0, width(), height()}, bg);

    if(height() >= 40)
    {
        p->fillRect({0, height() - 40, width(), 2}, fg);
        if(width() >= 120)
        {
            p->fillRect({40, 0, 2, height() - 40}, fg);
        }
    }
}


void Widget_AudioPlayer::mousePressEvent(MousePressEvent* event)
{
    setFocus();
    repaint();
}


void Widget_AudioPlayer::keyPressEvent(KeyPressEvent* event)
{
    if(Keyboard::CtrlDown())
    {
        if(event->key() == Keyboard::Key::V)
        {
            requestClipboardMetadata(ClipboardMode::Clipboard);
        }
    }
}


void Widget_AudioPlayer::clipboardDataRecieveEvent(ClipboardDataRecieveEvent* event)
{
    if(event->mode() != ClipboardMode::Clipboard)
        return;

    if(event->data() == nullptr && event->size() <= 0)
        return;

    if(event->type() == "text/uri-list")
    {
        string uri_list((const char*)event->data(), event->size());
        string file_path = extract_file_path_from_uri_list(uri_list);
        cout << "open file: " << file_path << "\n";
    }
}


void Widget_AudioPlayer::clipboardDataTransmitEvent(ClipboardDataTransmitEvent* event)
{

}


void Widget_AudioPlayer::clipboardMetadataRecieveEvent(ClipboardMetadataRecieveEvent* event)
{
    if(event->mode() != ClipboardMode::Clipboard)
        return;

    if(event->has("text/uri-list"))
    {
        requestClipboardData("text/uri-list", event->mode());
    }
}

}//namespace r64fx