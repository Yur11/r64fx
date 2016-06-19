#include "Widget_AudioPlayer.hpp"
#include "Painter.hpp"
#include "Mouse.hpp"
#include "Keyboard.hpp"
#include "KeyboardModifiers.hpp"
#include "KeyEvent.hpp"
#include "Clipboard.hpp"
#include "ClipboardEvent.hpp"
#include "SoundFile.hpp"
#include "ImageUtils.hpp"
#include "StringUtils.hpp"

#include <cstring>
#include <iostream>
using namespace std;

namespace r64fx{


Widget_AudioPlayer::Widget_AudioPlayer(Widget* parent)
: Widget(parent)
{

}


Widget_AudioPlayer::~Widget_AudioPlayer()
{
    if(m_waveform)
        delete m_waveform;
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
    }

    if(m_waveform)
    {
        unsigned char blue[3] = {0, 0, 255};
        p->drawWaveform({0, height() - 40, width(), 40}, blue, m_waveform);
    }
}


void Widget_AudioPlayer::resizeEvent(ResizeEvent* event)
{
    cout << "resize: " << event->size() << "\n";
    clip();
    repaint();
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
        auto it = uri_list.begin();
        for(;;)
        {
            auto file_path = next_file_path_from_uri_list(it, uri_list.end());
            if(file_path.empty())
                break;

            cout << file_path << "\n";
            continue;

            SoundFile file(file_path, SoundFile::Mode::Read);
            if(file.isGood())
            {
                cout << file.componentCount() << " " << file.frameCount() << " " << file.sampleRate() << "\n";

                int waveform_size = width();
                if(waveform_size > 0)
                {
                    if(!m_waveform)
                        m_waveform = new float[waveform_size * 2];

                    int chunk_size = file.frameCount() / waveform_size;
                    int buffer_size = chunk_size * file.componentCount();

                    float* buffer = new float[buffer_size];

                    for(int i=0; i<waveform_size; i++)
                    {
                        for(int j=0; j<buffer_size; j++)
                        {
                            buffer[j] = 0.0f;
                        }
                        file.readFrames(buffer, chunk_size);

                        float min = 0;
                        float max = 0;

                        for(int j=0; j<buffer_size; j+=file.componentCount())
                        {
                            float val = buffer[j];
                            if(val < 0)
                            {
                                if(val < min)
                                {
                                    min = val;
                                }
                            }
                            else
                            {
                                if(val > max)
                                {
                                    max = val;
                                }
                            }
                        }

                        m_waveform[i*2] = min;
                        m_waveform[i*2 + 1] = max;
                    }
                    delete buffer;
                }
            }
            else
            {
                cerr << "Failed to open: " << file_path << "\n";
            }
        }
    }

    repaint();
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