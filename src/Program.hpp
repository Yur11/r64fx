#ifndef R64FX_PROGRAM_HPP
#define R64FX_PROGRAM_HPP

namespace r64fx{

class Widget;
class Window;
class MouseEvent;
class KeyEvent;
class ResizeEvent;

class Program{
    bool m_should_be_running = true;
    
public:
    Program(int argc, char* argv[]);
    
    virtual ~Program();
    
    /** @brief Program main loop. */
    virtual int exec();
    
    virtual void quit();
    
    static Program* instance();
    
protected:
    virtual void mousePressEvent(Widget* widget, MouseEvent* event);

    virtual void mouseReleaseEvent(Widget* widget, MouseEvent* event);

    virtual void mouseMoveEvent(Widget* widget, MouseEvent* event);

    virtual void keyPressEvent(Widget* widget, KeyEvent* event);

    virtual void keyReleaseEvent(Widget* widget, KeyEvent* event);

    virtual void resizeEvent(Widget* widget, ResizeEvent* event);

    virtual void closeEvent(Widget* widget);

    virtual void setup();
    
    virtual void cleanup();
};
    
}//namespace r64fx

#endif//R64FX_PROGRAM_HPP