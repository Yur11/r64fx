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
#include "Widget_DirectoryTree.hpp"
#include "Widget_ItemBrowser.hpp"
#include "KeyEvent.hpp"
#include "Timer.hpp"
#include "Thread.hpp"
#include "sleep.hpp"


using namespace std;
using namespace r64fx;

Font* g_Font = nullptr;

class MyWidget : public Widget_ScrollArea{

public:
    MyWidget(Widget* parent = nullptr) : Widget_ScrollArea(parent)
    {
//         srand(time(0));
//
//         auto dummy = new Widget_Dummy(this);
//         dummy->setSize({350, 350});
//         dummy->setPosition({50, 50});
//
//         {
//             auto subdummy = new Widget_Dummy(dummy);
//             subdummy->setSize({100, 100});
//             subdummy->setPosition({50, 50});
//         }
//
//         {
//             auto subdummy = new Widget_Dummy(dummy);
//             subdummy->setSize({100, 100});
//             subdummy->setPosition({200, 50});
//         }
//
//         {
//             auto subdummy = new Widget_Dummy(dummy);
//             subdummy->setSize({100, 100});
//             subdummy->setPosition({50, 200});
//         }
//
//         {
//             auto subdummy = new Widget_Dummy(dummy);
//             subdummy->setSize({100, 100});
//             subdummy->setPosition({200, 200});
//         }
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
        Widget_ScrollArea::paintEvent(event);
    }

    virtual void mousePressEvent(MousePressEvent* event)
    {
        if(event->button() == MouseButton::Right())
        {

        }
        else
        {
            repaint();
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

public:
    MyProgram(int argc, char* argv[]) : Program(argc, argv) {}
    
private:
    virtual void setup()
    {
        g_Font = new Font("", 20, 72);

//         auto mw = new MyWidget();
//         mw->setSize({600, 600});
//         mw->show();

        auto db = new Widget_DirectoryTree("samples", "/home/yurii/Sound/Samples/freesound/");
        db->populate();

        auto ib = new Widget_ItemBrowser;
        ib->addItem(db);
        ib->rearrange();
        ib->show();

        ib->onItemSelected([](Widget_ItemBrowser* browser, void*){
            cout << "item: " << browser->selectedItem()->text() << "\n";
        });
    }

    virtual void cleanup()
    {
        cout << "Cleanup!\n";

        if(g_Font)
            delete g_Font;
    }
};


int main(int argc, char* argv[])
{
    MyProgram prog(argc, argv);
    return prog.exec();

    return 0;
}
