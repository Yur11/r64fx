#include <iostream>
#include <unistd.h>
#include <ctime>

#include "Program.hpp"
#include "Widget.hpp"
#include "Window.hpp"
#include "Mouse.hpp"
#include "Keyboard.hpp"
#include "ImageUtils.hpp"
#include "Painter.hpp"
#include "Font.hpp"
#include "Widget_ScrollArea.hpp"
#include "Widget_Control.hpp"
#include "Widget_Dummy.hpp"
#include "Widget_DirectoryItem.hpp"
#include "Widget_ItemBrowser.hpp"
#include "Widget_Text.hpp"
#include "Widget_AudioPlayer.hpp"
#include "KeyEvent.hpp"
#include "Timer.hpp"
#include "Thread.hpp"
#include "Player.hpp"
#include "sleep.hpp"


using namespace std;
using namespace r64fx;

Font* g_Font = nullptr;

ControlAnimation_Knob* g_anim_knob = nullptr;
ControlAnimation_ColouredButton* g_anim_colored_button = nullptr;
ControlAnimation_PlayPauseButton* g_anim_play_pause_button = nullptr;

class MyWidget : public Widget_ScrollArea{

public:
    MyWidget(Widget* parent = nullptr) : Widget_ScrollArea(parent)
    {
        setSize({640, 480});

        if(!g_anim_knob)
        {
            g_anim_knob = new ControlAnimation_Knob(48, 128, KnobType::Bipolar);
        }

        for(int i=0; i<8; i++)
        {
            auto c = new Widget_ValueControl(this);
            c->setPosition({100 + i * 80, 100});
            c->setMinValue(-1.0f);
            c->setMaxValue(+1.0f);
            c->setAnimation(g_anim_knob);
            c->setFont(g_Font);
            c->showsText(true);
            c->resizeAndRealign();
        }

        if(!g_anim_colored_button)
        {
            unsigned char c1[4] = {200, 127, 127, 0};
            unsigned char c2[4] = {200, 200, 127, 0};
            unsigned char c3[4] = {127, 200, 200, 0};
            unsigned char c4[4] = {127, 127, 200, 0};
            unsigned char c5[4] = {200, 127, 200, 0};
            unsigned char c6[4] = {200, 200, 200, 0};
            unsigned char c7[4] = {127, 200, 127, 0};
            unsigned char c8[4] = {127, 127, 127, 0};
            unsigned char* colors[8] = {c1, c2, c3, c4, c5, c6, c7, c8};

            g_anim_colored_button = new ControlAnimation_ColouredButton(48, colors, 8);
        }

        for(int y=0; y<8; y++)
        {
            for(int x=0; x<8; x++)
            {
                auto b1 = new Widget_ButtonControl(g_anim_colored_button, this);
                b1->setPosition({100 + x * 50, 200 + y * 50});
            }
        }

        if(!g_anim_play_pause_button)
        {
            g_anim_play_pause_button = new ControlAnimation_PlayPauseButton(48);
        }

        auto ppb = new Widget_ButtonControl(g_anim_play_pause_button, this);
        ppb->setPosition({550, 200});
    }

    ~MyWidget()
    {
        if(g_anim_knob)
        {
            delete g_anim_knob;
        }

        if(g_anim_colored_button)
        {
            delete g_anim_colored_button;
        }

        if(g_anim_play_pause_button)
        {
            delete g_anim_play_pause_button;
        }
    }

    virtual void paintEvent(PaintEvent* event)
    {
        auto p = event->painter();

        unsigned char grey[4]   = {127, 127, 127, 0};
        unsigned char black[4]  = {  0,   0,   0, 0};

        p->fillRect({0, 0, width(), height()}, grey);
        p->fillRect({offset(), {10, 10}}, black);
        Widget::paintEvent(event);
    }

    virtual void mousePressEvent(MousePressEvent* event)
    {
        Widget::mousePressEvent(event);

//         if(event->button() == MouseButton::Left())
//         {
//             grabMouse();
//         }
//         else if(event->button() == MouseButton::Right())
//         {
//
//         }
//         else
//         {
//
//         }
//         repaint();
    }

    virtual void mouseReleaseEvent(MouseReleaseEvent* event)
    {
        if(isMouseGrabber() && event->button() == MouseButton::Left())
        {
            ungrabMouse();
        }
    }

    virtual void mouseMoveEvent(MouseMoveEvent* event)
    {
        if(isMouseGrabber() && event->button() & MouseButton::Left())
        {
            setOffset(offset() + event->delta());
            recomputeChildrenVisibility();
            clip();
            repaint();
        }
    }

    virtual void keyPressEvent(KeyPressEvent* event)
    {
        if(event->key() == Keyboard::Key::Escape)
        {
            Program::quit();
        }

        repaint();
        Widget::keyPressEvent(event);
    }

    virtual void closeEvent()
    {
        Program::quit();
    }
};


class MyProgram : public Program{
    Player* m_player = nullptr;

public:
    MyProgram(int argc, char* argv[]) : Program(argc, argv) {}
    
private:
    virtual void setup()
    {
        g_Font = new Font("", 12, 72);

        auto db = new Widget_DirectoryItem("filesystem", "");

        auto ib = new Widget_ItemBrowser;
        ib->addItem(db);
        ib->rearrange();
        ib->setSize({400, 500});
        ib->show();
        db->expand();

        ib->onItemSelected([](Widget_ItemBrowser* browser, void*){
            auto selected_item = browser->selectedItem();
            if(selected_item)
            {
                cout << "item: " << selected_item->caption() << "\n";
            }
        });

        m_player = new Player;

        auto mw = new MyWidget;
        mw->show();

//         auto wt = new Widget_Text("", g_Font);
//         wt->setSize({640, 480});
//         wt->setTextWrap(TextWrap::Anywhere);
//         wt->show();

//         auto ap = new Widget_AudioPlayer;
//         ap->setSize({640, 480});
//         ap->show();
    }

    virtual void cleanup()
    {
        cout << "Cleanup!\n";

        if(g_Font)
            delete g_Font;

        if(m_player)
            delete m_player;
    }
};


int main(int argc, char* argv[])
{
    MyProgram prog(argc, argv);
    return prog.exec();

    return 0;
}
