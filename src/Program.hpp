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
    
    virtual Window* createWindow(Widget* widget);
    
    virtual void destroyWindow(Window* window);
    
    virtual void mousePressEvent(Window* window, MouseEvent* event);
    
    virtual void mouseReleaseEvent(Window* window, MouseEvent* event);
    
    virtual void mouseMoveEvent(Window* window, MouseEvent* event);
    
    virtual void keyPressEvent(Window* window, KeyEvent* event);
    
    virtual void keyReleaseEvent(Window* window, KeyEvent* event);
    
    virtual void closeEvent(Window* window);
    
    virtual void resizeEvent(Window* window, ResizeEvent* event);
    
    static Program* instance();
    
protected:
    virtual void setup();
    
    virtual void cleanup();
};
    
}//namespace r64fx

#endif//R64FX_PROGRAM_HPP