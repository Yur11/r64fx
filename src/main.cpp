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
#include "SignalGraph.hpp"
#include "Timer.hpp"
#include "Thread.hpp"
#include "sleep.hpp"
#include "SignalNode_AudioIO.hpp"
#include "SignalNode_Oscillator.hpp"
#include "SignalNode_Player.hpp"
#include "SoundFile.hpp"
#include "SignalData.hpp"


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

    Thread m_graph_thread;
    bool m_graph_running = true;

    SoundFile m_sound_file;
    SignalData m_signal_data;

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
        if(m_driver)
        {
            m_driver->enable();

            m_sound_file.open("../amen_break.wav", SoundFile::Mode::Read);
            if(m_sound_file.isGood())
            {
                m_signal_data.load(m_sound_file.frameCount(), m_sound_file.componentCount());
                m_sound_file.readFrames(m_signal_data.data(), m_signal_data.frameCount());
                m_signal_data.setSampleRate(m_sound_file.sampleRate());
                cout << "fc: " << m_signal_data.frameCount() << "\n";
                cout << "cc: " << m_signal_data.componentCount() << "\n";
                cout << "od: " << m_signal_data.ownsData() << "\n";
                cout << "sr: " << m_signal_data.sampleRate() << "\n";
                m_sound_file.close();
            }

            m_graph_thread.run([](void* arg) -> void*{
                auto self = (MyProgram*)arg;
                return self->processGraph();
            }, this);
        }
        else
        {
            cerr << "No driver!\n";
        }
    }


    void* processGraph()
    {
        SignalGraph graph(m_driver);

        SignalNodeClass_AudioInput   audio_input_class   (&graph);
        SignalNodeClass_AudioOutput  audio_output_class  (&graph);
        SignalNodeClass_Oscillator   oscillator_class    (&graph);
        SignalNodeClass_Player       player_class        (&graph);

        auto output = audio_output_class.newNode("audio_output", 2);
        auto input  = audio_input_class.newNode("audio_input", 2);
        auto osc    = oscillator_class.newNode();
        auto player = player_class.newNode(&m_signal_data);

        oscillator_class.frequency()->buffer()[osc->slotOffset()] = 440.0f;

        for(int i=0; i<player->slotCount(); i++)
        {
            float end_point = float(m_signal_data.frameCount()) / float(m_signal_data.sampleRate());
            player_class.playEnd()->buffer()[player->slotOffset() + i] = end_point;
            cout << "end_point: " << end_point << "\n";
        }

        auto connection = graph.newConnection(
            output, audio_output_class.sink(), player, player_class.out()
        );

//         int i = 0;
        int n = 0;

        while(m_graph_running)
        {
            if(graph.process())
            {
//                 cout << i++ << " -> " << n << "\n";
                n = 0;
            }
            else
            {
                n++;
                sleep_microseconds(100);
            }
        }

        cout << "Graph Thread exit!\n";
        return nullptr;
    }

    
    virtual void cleanup()
    {
        cout << "Cleanup!\n";
        m_graph_running = false;
        m_graph_thread.join();

        if(m_Font)
            delete m_Font;

        if(m_container)
            delete m_container;

        if(m_driver)
            SoundDriver::deleteInstance(m_driver);

        m_signal_data.free();
    }
};


int main(int argc, char* argv[])
{
    MyProgram prog(argc, argv);
    return prog.exec();
}
