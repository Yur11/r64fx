#include <iostream>
#include <unistd.h>

#include "Program.hpp"
#include "Widget.hpp"
#include "Window.hpp"
#include "KeyEvent.hpp"
#include "Keyboard.hpp"
#include "MouseEvent.hpp"
#include "ReconfigureEvent.hpp"
#include "Image.hpp"
#include "Painter.hpp"
#include "AudioData.hpp"

using namespace std;
using namespace r64fx;


class MyWidget : public Widget{
    Image* m_Image = nullptr;
    float* data = nullptr;
    int data_size = 256;

public:
    MyWidget(Widget* parent = nullptr) : Widget(parent)
    {
        m_Image = new Image(20, 20, 4);
        {
            unsigned char px[4] = { 255, 255, 255, 255 };
            m_Image->fill(px);
        }
        {
            unsigned char px[4] = { 255, 0, 0, 0 };
            m_Image->setPixel(9,  9,  px);
            m_Image->setPixel(10, 9,  px);
            m_Image->setPixel(9,  10, px);
            m_Image->setPixel(10, 10, px);
        }

        {
            AudioData ad("./data/drum_loop_mono.wav");

            data_size = ad.size() / 128;
            data = new float[data_size];
            ad.calculateLinear();
            calculate_peak_summary(ad.constantData(), ad.size(), data, data_size);

//             for(int i=0; i<data_size; i++)
//             {
//                 cout << data[i] << "\n";
//             }
        }

    }

    ~MyWidget()
    {
        if(m_Image)
        {
            delete m_Image;
        }

        if(data)
        {
            delete[] data;
        }
    }

protected:
    virtual void reconfigureEvent(ReconfigureEvent* event)
    {
//         if(event->sizeChanged())
//         {
//             auto size = event->newSize();
// //             cout << "MyWidget::reconfigureEvent(" << size.width() << "x" << size.height() << ")\n";
//         }

        auto p = event->painter();
        p->clear();
        p->fillRect({10, 10, 100, 100},   {1.0f, 0.0f, 0.0f});
        p->fillRect({210, 210, 100, 100}, {0.0f, 1.0f, 0.0f});
        p->fillRect({410, 410, 100, 100}, {0.0f, 0.0f, 1.0f});
//         p->putImage(130, 200, m_Image);
//         p->putImage(250, 50, m_Image);
//         p->putImage(180, 250, m_Image);
//         p->putPlot(Rect<int>(100, 100, 100, data_size/2), data, data_size, Orientation::Vertical);
//         p->putPlot(Rect<int>(250, 200, data_size/2, 100), data, data_size, Orientation::Horizontal);
//         p->prepare();
        p->repaint();

//         p->debugDraw();
    }
};


class MyProgram : public Program{
    MyWidget* m_Widget = nullptr;
    Painter* m_painter = nullptr;
    Point<int> m_point = {10, 10};

    
public:
    MyProgram(int argc, char* argv[]) : Program(argc, argv) {}
    
private:
    virtual void setup()
    {
        m_Widget = new MyWidget;
        m_Widget->resize(800, 600);
        m_Widget->show();
    }

//     virtual void mousePressEvent(Window* window, MouseEvent* event)
//     {
//     }
//
//     virtual void mouseReleaseEvent(Window* window, MouseEvent* event)
//     {
//     }
//
//     virtual void mouseMoveEvent(Window* window, MouseEvent* event)
//     {
//     }
    

    virtual void keyPressEvent(Window* window, KeyEvent* event)
    {
        Program::keyPressEvent( window, event );

        cout << Keyboard::Key::toString(event->key()) << "\n";

        if(event->key() == Keyboard::Key::Escape) 
        {
            quit();
        }
        else if(event->key() == Keyboard::Key::J)
        {
            m_Widget->resize(400, 200);
            m_Widget->setWindowTitle("A");
        }
        else if(event->key() == Keyboard::Key::K)
        {
            m_Widget->resize(200, 400);
            m_Widget->setWindowTitle("B");
        }
        else if(event->key() == Keyboard::Key::T)
        {
            cout << "Title: " << m_Widget->windowTitle() << "\n";
        }

    }
    

    virtual void closeEvent(Window* window)
    {
        if(window->widget() == m_Widget)
        {
            quit();
        }
    }


    virtual void cleanup()
    {
        cout << "Cleanup!\n";
        delete m_Widget;
    }
};


int main(int argc, char* argv[])
{
    MyProgram prog(argc, argv);
    return prog.exec();
}
