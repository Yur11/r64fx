#ifndef R64FX_PROGRAM_HPP
#define R64FX_PROGRAM_HPP

namespace r64fx{

class Window;
class MouseEvent;
class KeyEvent;

class Program{
    int m_argc = 0;
    char** m_argv = nullptr;
    
    bool m_should_be_running = true;
    
public:
    Program(int argc, char* argv[]);
    
    virtual ~Program();
    
    virtual int exec();
    
    virtual void quit();
    
    virtual Window* createWindow(int w=800, int h=600, const char* title = "");
    
    virtual void destroyWindow(Window* window);
    
    virtual void mousePressEvent(Window* window, MouseEvent* event);
    
    virtual void mouseReleaseEvent(Window* window, MouseEvent* event);
    
    virtual void mouseMoveEvent(Window* window, MouseEvent* event);
    
    virtual void keyPressEvent(Window* window, KeyEvent* event);
    
    virtual void keyReleaseEvent(Window* window, KeyEvent* event);
    
    virtual void closeEvent(Window* window);
    
protected:
    virtual void setup();
    
    virtual void cleanup();
};
    
}//namespace r64fx

#endif//R64FX_PROGRAM_HPP