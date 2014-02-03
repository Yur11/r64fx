#include "SDL2Window.h"

#include <iostream>
#include <vector>

#ifdef DEBUG
#include <assert.h>
#endif//DEBUG

#include "View.h"

using namespace std;

namespace r64fx{
   
// Window* focused_window = nullptr;
// Window* last_focused_window = nullptr;
bool tracking_mouse = false;
bool should_quit = false;
unsigned int pressed_mouse_buttons = 0;
// vector<SDL2Window*> all_sdl2_windows;

    
SDL2Window::SDL2Window(RenderingContextId_t id, int width, int height, const char* title)
: RenderingContext(id)
{
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); 
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
    
    _window = SDL_CreateWindow(
        title, 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
        width, height, 
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
    );
    
    if(!_window)
    {
        cerr << "Failed to create SDL Window!\n";
        abort();
    }

    _gl_context = SDL_GL_CreateContext(_window);
    if(!_gl_context)
    {
        cerr << "Failed to create GL context!\n";
        abort();
    }
    
//     all_sdl2_windows.push_back(this);

    SDL_SetWindowData(_window, "window", this);
    
    updateGeometry();
}


SDL2Window* SDL2Window::create(int width, int height, const char* title)
{
    auto id = RenderingContext::getFreeId();
    if(id == BadRenderingContextId)
        return nullptr;
    else 
    {
        auto window = new SDL2Window(id, width, height, title);
        window->makeCurrent();
        window->setup();
        return window;
    }
}


SDL2Window::~SDL2Window()
{
    RenderingContext::cleanup();
    SDL_GL_DeleteContext(_gl_context);
    SDL_DestroyWindow(_window);
    
    
//     for(auto it=all_sdl2_windows.begin(); it!=all_sdl2_windows.end(); it++)
//     {
//         if(*it == this)
//         {
//             all_sdl2_windows.erase(it);
//             break;
//         }
//     }
}


void SDL2Window::swapBuffers()
{
    SDL_GL_SwapWindow(_window);
}


void SDL2Window::makeCurrent()
{
    SDL_GL_MakeCurrent(_window, _gl_context);
}


void SDL2Window::render()
{
    makeCurrent();
    Window::render(RenderingContext::id());
    swapBuffers();
}


Size<int> SDL2Window::size()
{
    Size<int> s;
    SDL_GetWindowSize(_window, &s.w, &s.h);
    return s;
}


void SDL2Window::updateGeometry()
{
     /* Hack. Send these events to the window for it to resize properly. */
    SDL_Event event;
    event.type = SDL_WINDOWEVENT;
    
    event.window.event = SDL_WINDOWEVENT_FOCUS_GAINED;
    event.window.windowID = SDL_GetWindowID(_window);
    SDL_PushEvent(&event);
    
    event.window.event = SDL_WINDOWEVENT_RESIZED;
    event.window.data1 = width();
    event.window.data2 = height();
    SDL_PushEvent(&event);
}


void SDL2Window::warpMouse(int x, int y)
{
    SDL_WarpMouseInWindow(_window, x, height() - y);
}


bool SDL2Window::init()
{
    RenderingContext::init();
    return SDL_Init(SDL_INIT_VIDEO) == 0;
}
    

void SDL2Window::cleanup()
{
    SDL_Quit();
}
    
    
void SDL2Window::trackMouse(bool on)
{
    tracking_mouse = on;
}


bool SDL2Window::shouldQuit()
{
    return should_quit;
}


/** @brief Get an SDL2Window that recieved the event or an alternative value if the window was nullptr. */
SDL2Window* get_event_window_or_alternative(unsigned int window_id, SDL2Window* alternative = nullptr)
{
    auto window = (SDL2Window*) SDL_GetWindowData(SDL_GetWindowFromID(window_id), "window");
    return window ? window : alternative;
}

        
void SDL2Window::processEvents()
{    
    static SDL_Event event;
    static SDL2Window* window = nullptr;
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
                int x = 0; 
                int y = 0;
                
                SDL_GetMouseState(&x, &y);
                
                window = get_event_window_or_alternative(event.key.windowID, window);
#ifdef DEBUG
                assert(window != nullptr);
#endif//DEBUG
                window->makeCurrent();
                
                window->initKeyPressEvent(
                    x, window->height() - y,
                    event.key.keysym.scancode, 
                    pressed_mouse_buttons, 
                    event.key.keysym.mod
                );
                break;
            }
            
            case SDL_KEYUP:
            {
                int x = 0; 
                int y = 0;
                
                SDL_GetMouseState(&x, &y);
                
                window = get_event_window_or_alternative(event.key.windowID, window);
#ifdef DEBUG
                assert(window != nullptr);
#endif//DEBUG
                window->makeCurrent();
                
                window->initKeyReleaseEvent(
                    x, window->height() - y,
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
                window = get_event_window_or_alternative(event.text.windowID, window);
#ifdef DEBUG
                assert(window != nullptr);
#endif//DEBUG
                window->makeCurrent();
                
                std::string chr;
                for(int i=0; i<SDL_TEXTINPUTEVENT_TEXT_SIZE && event.text.text[i]; i++)
                {
                    chr.push_back(event.text.text[i]);
                }
                window->initTextInputEvent(chr);
                break;
            }
            
            /* Mouse */
            case SDL_MOUSEMOTION:
            {
                window = get_event_window_or_alternative(event.motion.windowID, window);
#ifdef DEBUG
                assert(window != nullptr);
#endif//DEBUG
                window->makeCurrent();
                
                window->initMouseMoveEvent(event.motion.x, window->height() - event.motion.y, pressed_mouse_buttons, SDL_GetModState());
                break;
            }
            
            case SDL_MOUSEBUTTONDOWN:
            {
                window = get_event_window_or_alternative(event.button.windowID, window);                
#ifdef DEBUG
                assert(window != nullptr);
#endif//DEBUG
                window->makeCurrent();
                
                cout << "click: " << window->id() << "\n";
                
                if(event.button.button == SDL_BUTTON_LEFT)
                    pressed_mouse_buttons |= Mouse::Button::Left;
                else if(event.button.button == SDL_BUTTON_MIDDLE)
                    pressed_mouse_buttons |= Mouse::Button::Middle;
                else if(event.button.button == SDL_BUTTON_RIGHT)
                    pressed_mouse_buttons |= Mouse::Button::Right;
                window->initMousePressEvent(event.button.x, window->height() - event.button.y, pressed_mouse_buttons, SDL_GetModState());
                break;
            }
            
            case SDL_MOUSEBUTTONUP:
            {
                window = get_event_window_or_alternative(event.button.windowID, window);
#ifdef DEBUG
                assert(window != nullptr);
#endif//DEBUG
                window->makeCurrent();
                
                if(event.button.button == SDL_BUTTON_LEFT)
                    pressed_mouse_buttons &= ~Mouse::Button::Left;
                else if(event.button.button == ~SDL_BUTTON_MIDDLE)
                    pressed_mouse_buttons &= ~Mouse::Button::Middle;
                else if(event.button.button == SDL_BUTTON_RIGHT)
                    pressed_mouse_buttons &= ~Mouse::Button::Right;
                window->initMouseReleaseEvent(event.button.x, window->height() - event.button.y, pressed_mouse_buttons, SDL_GetModState());
                break;
            }
            
            case SDL_MOUSEWHEEL:
            {
                int x = 0; 
                int y = 0;
            
                SDL_GetMouseState(&x, &y);

                window = get_event_window_or_alternative(event.wheel.windowID, window);
#ifdef DEBUG
                assert(window != nullptr);
#endif//DEBUG
                window->makeCurrent();
                
                window->initMouseWheelEvent(x, window->height() - y, event.wheel.x, event.wheel.y, pressed_mouse_buttons, SDL_GetModState());
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
                window = get_event_window_or_alternative(event.window.windowID, window);
                
                if(event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
                {
//                     auto w = SDL_GetWindowFromID(event.window.windowID);
//                     for(int i=0; i<(int)all_sdl2_windows.size(); i++)
//                     {
//                         if(all_sdl2_windows[i]->sdl_window() == w)
//                         {
//                             focused_window = all_sdl2_windows[i];
//                             last_focused_window = focused_window;
//                             break;
//                         }
//                     }
                }
                else if(event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
                {
                }
                else if(event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    window->makeCurrent();
                    
                    int w = event.window.data1;
                    int h = event.window.data2;
                    
                    glViewport(0, 0, w, h);
                    glMatrixMode(GL_PROJECTION);
                    glLoadIdentity();
                    
                    /* (0, 0) is in the bottom left corner. */
                    glOrtho(0, w, 0, h, -1, 1);
                    
                    glMatrixMode(GL_MODELVIEW);
                    glLoadIdentity();
                    
                    window->view()->resize(0, h, w, 0);
                }
                else if(event.window.event == SDL_WINDOWEVENT_CLOSE)
                {
                    window->makeCurrent();
                    delete window;
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
