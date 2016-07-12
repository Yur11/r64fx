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

ControlAnimation* g_anim_knob = nullptr;
ControlAnimation* g_anim_button = nullptr;

class MyWidget : public Widget_ScrollArea{

public:
    MyWidget(Widget* parent = nullptr) : Widget_ScrollArea(parent)
    {
        setSize({640, 480});

        if(!g_anim_knob)
        {
            g_anim_knob = new ControlAnimation_Knob(48);
        }

        auto c1 = new Widget_Control(g_anim_knob, this);
        c1->setPosition({100, 100});

        Image button_img(48, 48, 1);
        fill(&button_img, (unsigned char)255);
        unsigned char red    [4] = {50, 50, 50, 0};
        unsigned char green  [4] = {200, 200, 200, 0};
        unsigned char* colors[2] = {red, green};

        if(!g_anim_button)
        {
            g_anim_button = new ControlAnimation_Button(&button_img, colors, 2);
        }

        auto b1 = new Widget_Control(g_anim_button, this);
        b1->setPosition({200, 100});
    }

    ~MyWidget()
    {
        if(g_anim_knob)
        {
            delete g_anim_knob;
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
        g_Font = new Font("", 16, 72);

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
//
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
