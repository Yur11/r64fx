#ifndef R64FX_PROGRAM_HPP
#define R64FX_PROGRAM_HPP

namespace r64fx{

class Widget;
class Window;
class MousePressEvent;
class MouseReleaseEvent;
class MouseMoveEvent;
class KeyEvent;

class Program{
    friend class Widget;
    bool m_should_be_running = true;
    
public:
    Program(int argc, char* argv[]);
    
    virtual ~Program();
    
    /** @brief Program main loop. */
    virtual int exec();
    
    virtual void quit();
    
    static Program* instance();
    
protected:
    virtual void reconfigure(Window* window);

    virtual void mousePressEvent(Window* window, MousePressEvent* event);

    virtual void mouseReleaseEvent(Window* window, MouseReleaseEvent* event);

    virtual void mouseMoveEvent(Window* window, MouseMoveEvent* event);

    virtual void keyPressEvent(Window* window, KeyEvent* event);

    virtual void keyReleaseEvent(Window* window, KeyEvent* event);

    virtual void closeEvent(Window* window);

    virtual void setup();
    
    virtual void cleanup();

private:
    void performUpdates(Window* window);
    
};
    
}//namespace r64fx

#endif//R64FX_PROGRAM_HPP