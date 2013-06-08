/* SDL2 Implementation of Window */

#include <iostream>
#include <vector>

#ifdef DEBUG
#include <assert.h>
#endif//DEBUG

#include "../View.h"

using namespace std;

namespace r64fx{
   
vector<Window*> all_windows;
Window* focused_window = nullptr;
Window* last_focused_window = nullptr;
bool tracking_mouse = false;
bool should_quit = false;
unsigned int pressed_mouse_buttons = 0;

    
Window::Window(int width, int height, const char* title)
{
    _window = SDL_CreateWindow(title, 0, 0, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    if(!_window)
    {
        cerr << "Failed to create SDL Window!\n";
        abort();
    }
    
    _gl_context =   SDL_GL_CreateContext(_window);
    if(!_gl_context)
    {
        cerr << "Failed to create GL context!\n";
        abort();
    }
        
    all_windows.push_back(this);
    
    trackMouse(true);
}


Window::~Window()
{
    for(int i=0; i<(int)all_windows.size(); i++)
    {
        if(all_windows[i] == this)
        {
            all_windows.erase( all_windows.begin() + i );
            break;
        }
    }
}


void Window::render()
{
#ifdef DEBUG
    if(!view())
    {
        cerr << "View is null!\n";
        abort();
    }
#endif//DEBUG

    view()->render();
    
    render_overlay_menus();
}


void Window::makeCurrent()
{
    SDL_GL_MakeCurrent(_window, _gl_context);
}


void Window::swapBuffers()
{
    SDL_GL_SwapWindow(_window);
}

Size<int> Window::size()
{
    Size<int> s;
    SDL_GetWindowSize(_window, &s.w, &s.h);
    return s;
}

bool Window::init()
{
    return SDL_Init(SDL_INIT_VIDEO) == 0;
}
    

void Window::cleanup()
{
    SDL_Quit();
}
    
    
void Window::trackMouse(bool on)
{
    tracking_mouse = on;
}


bool Window::shouldQuit()
{
    return should_quit;
}


        
void Window::processEvents()
{    
    static SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT:
            {
                should_quit = true;
                break;
            }
            
            /* Keyboard */
            case SDL_KEYDOWN:
            {
                if(focused_window) 
                    focused_window->initKeyPressEvent(
                        event.key.keysym.scancode, 
                        pressed_mouse_buttons, 
                        event.key.keysym.mod
                    );
                break;
            }
            
            case SDL_KEYUP:
            {
                if(focused_window) 
                    focused_window->initKeyReleaseEvent(
                        event.key.keysym.scancode, 
                        pressed_mouse_buttons, 
                        event.key.keysym.mod
                    );
                break;
            }
            
            case SDL_TEXTEDITING:
            {
                break;
            }
            
            case SDL_TEXTINPUT:
            {
                if(last_focused_window)
                {
                    //Unicode input broken.
                    std::string chr;
                    for(int i=0; i<SDL_TEXTINPUTEVENT_TEXT_SIZE && event.text.text[i]; i++)
                    {
                        chr.push_back(event.text.text[i]);
                    }
                    focused_window->initTextInputEvent(chr);
                }
                break;
            }
            
            /* Mouse */
            case SDL_MOUSEMOTION:
            {
                if(last_focused_window)
                {
                    last_focused_window->initMouseMoveEvent(event.motion.x, last_focused_window->height() - event.motion.y, pressed_mouse_buttons);
                }
                break;
            }
            
            case SDL_MOUSEBUTTONDOWN:
            {
                if(last_focused_window)
                {
                    if(event.button.button == SDL_BUTTON_LEFT)
                        pressed_mouse_buttons |= Mouse::Button::Left;
                    else if(event.button.button == SDL_BUTTON_MIDDLE)
                        pressed_mouse_buttons |= Mouse::Button::Middle;
                    else if(event.button.button == SDL_BUTTON_RIGHT)
                        pressed_mouse_buttons |= Mouse::Button::Right;
                    last_focused_window->initMousePressEvent(event.button.x, last_focused_window->height() - event.button.y, pressed_mouse_buttons);
                }
                break;
            }
            
            case SDL_MOUSEBUTTONUP:
            {
                if(last_focused_window)
                {
                    if(event.button.button == SDL_BUTTON_LEFT)
                        pressed_mouse_buttons &= ~Mouse::Button::Left;
                    else if(event.button.button == ~SDL_BUTTON_MIDDLE)
                        pressed_mouse_buttons &= ~Mouse::Button::Middle;
                    else if(event.button.button == SDL_BUTTON_RIGHT)
                        pressed_mouse_buttons &= ~Mouse::Button::Right;
                    last_focused_window->initMouseReleaseEvent(event.button.x, last_focused_window->height() - event.button.y, pressed_mouse_buttons);
                }
                break;
            }
            
            case SDL_MOUSEWHEEL:
            {
                if(last_focused_window)
                {
                    last_focused_window->initMouseWheelEvent(event.wheel.x, event.wheel.y, pressed_mouse_buttons);
                }
                break;
            }
            
            /* Clipboard */
            case SDL_CLIPBOARDUPDATE:
            {
                break;
            }
            
            /* Drag and drop */
            case SDL_DROPFILE:
            {
                cout << "dropfile\n";
                break;
            }
            
            /* Window */
            case SDL_WINDOWEVENT:
            {
                if(event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
                {
                    auto w = SDL_GetWindowFromID(event.window.windowID);
                    for(int i=0; i<(int)all_windows.size(); i++)
                    {
                        if(all_windows[i]->sdl_window() == w)
                        {
                            focused_window = all_windows[i];
                            last_focused_window = focused_window;
                            break;
                        }
                    }
                }
                else if(event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
                {
                    focused_window = nullptr;
                }
                else if(event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    if(last_focused_window)
                    {
                        int w = event.window.data1;
                        int h = event.window.data2;
                        
                        glViewport(0, 0, w, h);
                        glMatrixMode(GL_PROJECTION);
                        glLoadIdentity();
                        
                        /* (0, 0) is in the bottom left corner. */
                        glOrtho(0, w, 0, h, -1, 1);
                        
                        glMatrixMode(GL_MODELVIEW);
                        glLoadIdentity();
                        
                        last_focused_window->view()->resize(0, h, w, 0);
                    }
                }
                break;
            }
            
            
            case SDL_SYSWMEVENT:
            {
                break;
            }
        }
    }
}

    
}//namespace r64fx
