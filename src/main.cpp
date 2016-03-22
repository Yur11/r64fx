#include <iostream>
#include <unistd.h>

#include "Program.hpp"
#include "Widget.hpp"
#include "Window.hpp"
#include "Mouse.hpp"
#include "Keyboard.hpp"
#include "ImageUtils.hpp"
#include "Painter.hpp"
#include "Font.hpp"
#include "Widget_Button.hpp"
#include "Widget_Container.hpp"
#include "Widget_Control.hpp"
#include "Widget_Dummy.hpp"
#include "Widget_Text.hpp"
#include "Widget_View.hpp"
#include "Widget_ScrollBar.hpp"
#include "ImageAnimation.hpp"
#include "KeyEvent.hpp"
#include "SoundDriver.hpp"
#include "Timer.hpp"
#include "NodeGraph.hpp"

using namespace std;
using namespace r64fx;

class MyWidget : public Widget_View{
    Image m_Image;
    ImageAnimation* m_animation = nullptr;
    float m_angle = 0.0f;

public:
    MyWidget(Widget* parent = nullptr) : Widget_View(parent)
    {
        auto wc1 = new Widget_Control(ControlType::UnipolarRadius, 50, this);
        wc1->setPosition({100, 100});

        auto wc2 = new Widget_Control(ControlType::UnipolarRadius,  50, this);
        wc2->setPosition({160, 160});

        wc1->onValueChanged([](Widget_Control* control, void* data){
            auto wc2 = (Widget_Control*) data;
            wc2->setValue(control->value());
            wc2->update();
        }, wc2);

        wc2->onValueChanged([](Widget_Control* control, void* data){
            auto wc1 = (Widget_Control*) data;
            wc1->setValue(control->value());
            wc1->update();
        }, wc1);

        unsigned char red[4]    = {255, 0, 0, 0};
        unsigned char green[4]  = {0, 255, 0, 0};
        unsigned char blue[4]   = {0, 0, 255, 0};
        unsigned char* colors[3] = {red, green, blue};
        m_animation = new_button_animation(20, 20, colors, 3);

        auto wb = new Widget_Button(m_animation, this);
        wb->setPosition({100, 200});
        wb->onClick([](Widget_Button* button, void*){
            cout << "click " << button->state() << "\n";
            button->pickNextState();
            button->update();
        });
    }

    ~MyWidget()
    {
        if(m_animation)
            delete m_animation;
    }

    virtual void reconfigureEvent(ReconfigureEvent* event)
    {
        unsigned char fg[4] = { 0,   0,   0,   0 };
        unsigned char bg[4] = { 127, 180, 255, 0 };

        m_Image.load(width(), height(), 4);
        fill(&m_Image, bg);
        auto r = intersection(
            Rect<int>(50, 50, 10, 10) + offset(),
            Rect<int>(0, 0, m_Image.width(), m_Image.height())
        );
        if(r.width() > 0 && r.height() > 0)
        {
            fill(&m_Image, fg, r);
        }

        auto painter = event->painter();
        painter->putImage(&m_Image);

        Widget_View::reconfigureEvent(event);
    }

    virtual void mousePressEvent(MousePressEvent* event)
    {
        Widget_View::mousePressEvent(event);
    }

    virtual void mouseMoveEvent(MouseMoveEvent* event)
    {
        if(event->button() & MouseButton::Left())
        {
            setOffset(offset() + event->delta());
            update();
        }
        else
        {
            Widget_View::mouseMoveEvent(event);
        }
    }

    virtual void keyPressEvent(KeyPressEvent* event)
    {
        if(event->key() == Keyboard::Key::Up)
        {
//             m_angle += 0.05;
//             if(m_angle < 0.0f)
//                 m_angle += 2.0f * M_PI;
//             else if(m_angle > 2.0f * M_PI)
//                 m_angle -= 2.0f * M_PI;
//             setOffsetY(offsetY() - step);
        }
        else if(event->key() == Keyboard::Key::Down)
        {
//             m_angle -= 0.05f;
//             if(m_angle < 0.0f)
//                 m_angle += 2.0f * M_PI;
//             else if(m_angle > 2.0f * M_PI)
//                 m_angle -= 2.0f * M_PI;
//             setOffsetY(offsetY() + step);
        }
        else if(event->key() == Keyboard::Key::Left)
        {
//             setOffsetX(offsetX() - step);
        }
        else if(event->key() == Keyboard::Key::Right)
        {
//             setOffsetX(offsetX() + step);
        }
        update();

        Widget::keyPressEvent(event);
    }

    virtual void closeEvent()
    {
        Program::quit();
    }
};


class MyProgram : public Program{
    Font*   m_Font = nullptr;
    Widget_Container* m_container = nullptr;

    Widget_Control* m_wc1 = nullptr;

    SoundDriver* m_driver = nullptr;
    SoundDriverIOPort_AudioOutput*  m_audio_out = nullptr;
    SoundDriverIOPort_MidiInput*    m_midi_in = nullptr;

    float  m_frequency = 440.0f;
    float  m_value = 0.0f;
    float* m_buffer = nullptr;

    Timer m_timer1;
    Timer m_timer2;

    NodeGraph* m_graph = nullptr;

public:
    MyProgram(int argc, char* argv[]) : Program(argc, argv) {}
    
private:
    virtual void setup()
    {
        m_Font = new Font("", 20, 72);

        m_container = new Widget_Container;
        {
            auto subcontainer = new Widget_Container(m_container);
            auto mw = new MyWidget(subcontainer);
            mw->setSize({300, 300});
            auto sb = new Widget_ScrollBar_Horizontal(subcontainer);
            sb->setWidth(300);
            sb->setRatio(0.5f);
            subcontainer->alignVertically();
        }

        auto subcontainer = new Widget_Container(m_container);
        auto wc1 = new Widget_Control(ControlType::UnipolarRadius, 50, subcontainer);
        m_wc1 = wc1;
        auto wc2 = new Widget_Control(ControlType::UnipolarRadius, 50, subcontainer);
        auto wc3 = new Widget_Control(ControlType::UnipolarSector, 50, subcontainer);
        auto wc4 = new Widget_Control(ControlType::BipolarSector,  50, subcontainer);
        auto wc5 = new Widget_Control(ControlType::BipolarRadius,  50, subcontainer);
        subcontainer->setSpacing(5);
        subcontainer->alignVertically();

        auto wt = new Widget_Text("", m_Font, m_container);
        wt->setWidth(300);
        wt->setHeight(300);
        wt->setPadding(5);
        wt->setTextWrap(TextWrap::Anywhere);
        wt->setTextAlignment(TextAlignment::Left);

        m_container->setPadding(20);
        m_container->setSpacing(5);
        m_container->alignHorizontally();
        m_container->show();

        m_driver = SoundDriver::newInstance();
        m_graph = new NodeGraph(m_driver);
        if(m_driver && m_graph)
        {
            m_driver->enable();
            m_graph->enable();
        }
        else
        {
            cerr << "No driver!\n";
        }
    }


    void onTimer1()
    {
        if(m_midi_in)
        {
            MidiEvent event;
            while(m_midi_in->readEvents(&event, 1))
            {
                auto msg = event.message();
                switch(msg.type())
                {
                    case MidiMessage::Type::NoteOff:
                    {
                        cout << "NoteOff:       "
                             << msg.channel() << ", "
                             << msg.noteNumber() << ", "
                             << msg.velocity() << "\n";
                        break;
                    }

                    case MidiMessage::Type::NoteOn:
                    {
                        cout << "NoteOn:        "
                             << msg.channel() << ", "
                             << msg.noteNumber() << ", "
                             << msg.velocity() << "\n";
                        break;
                    }

                    case MidiMessage::Type::ControlChange:
                    {
                        cout << "ControlChange: "
                             << msg.channel() << ", "
                             << msg.controllerNumber() << ","
                             << msg.controllerValue() << "\n";

                        if(msg.controllerNumber() == 1 && m_wc1)
                        {
                            float value = float(msg.controllerValue()) / 127.0f;
                            m_wc1->setValue(value);
                            m_wc1->update();
                            m_frequency = 440.0f * value;
                        }

                        break;
                    }

                    default:
                    {
                        cout << "other\n";
                        break;
                    }
                }
            }
        }

//         long time = 0;
//         if(m_audio_out && m_driver->readTime(&time))
//         {
//             float delta = m_frequency / float(m_driver->sampleRate());
//             for(int i=0; i<m_driver->bufferSize(); i++)
//             {
//                 m_buffer[i] = sin(m_value * 2.0 * M_PI);
//
//                 m_value += delta;
//                 if(m_value >= 1.0f)
//                     m_value -= 1.0f;
//             }
//
//             int nsamples = m_audio_out->writeSamples(m_buffer, m_driver->bufferSize());
//             if(nsamples < m_driver->bufferSize())
//             {
// //                 cerr << "Wrote only " << nsamples << " samples\n";
//             }
//         }
    }

    
    virtual void cleanup()
    {
        cout << "Cleanup!\n";
        if(m_Font)
            delete m_Font;

        if(m_container)
            delete m_container;

        if(m_graph)
            delete m_graph;

        if(m_driver)
            SoundDriver::deleteInstance(m_driver);
    }
};


int main(int argc, char* argv[])
{
    MyProgram prog(argc, argv);
    return prog.exec();
}
