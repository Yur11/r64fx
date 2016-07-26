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
#include "Widget_Dummy.hpp"
#include "Widget_DirectoryItem.hpp"
#include "Widget_ItemBrowser.hpp"
#include "Widget_Text.hpp"
#include "Widget_Knob.hpp"
#include "Widget_Button.hpp"
#include "Widget_Slider.hpp"
#include "KeyEvent.hpp"
#include "Timer.hpp"
#include "Thread.hpp"
#include "Player.hpp"
#include "sleep.hpp"


using namespace std;
using namespace r64fx;

namespace r64fx{
    Font* g_Font = nullptr;
    Font* g_LargeFont = nullptr;
}

class MyWidget : public Widget_ScrollArea{

public:
    MyWidget(Widget* parent = nullptr) : Widget_ScrollArea(parent)
    {
        setSize({640, 640});

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

            for(int y=0; y<8; y++)
            {
                for(int x=0; x<8; x++)
                {
                    auto b1 = new Widget_Button(ButtonAnimation::Colored({48, 48}, colors, 8), true, this);
                    b1->setPosition({100 + x * 50, 200 + y * 50});
                }
            }
        }

        auto k1 = new Widget_UnipolarKnob(this);
        k1->setPosition({100, 100});

        auto k2 = new Widget_BipolarKnob(this);
        k2->setPosition({200, 100});

        auto b1 = new Widget_Button(ButtonAnimation::PlayPause({48, 48}), true, this);
        b1->setPosition({300, 100});

        auto b2 = new Widget_Button(ButtonAnimation::Text({48, 48}, "CUE", g_LargeFont), true, this);
        b2->setPosition({350, 100});

        auto s1 = new Widget_Slider(150, Orientation::Vertical, this);
        s1->setPosition({50, 100});

        auto s2 = new Widget_Slider(150, Orientation::Horizontal, this);
        s2->setPosition({400, 50});
    }

    ~MyWidget()
    {

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

        g_LargeFont = new Font("", 18, 72);

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

        if(g_LargeFont)
            delete g_LargeFont;

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
