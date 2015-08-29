#include "gui_implementation_iface.hpp"

#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

using namespace std;

namespace r64fx{

namespace Keyboard{
namespace Key{

int K0 = XK_0;
int K1 = XK_1;
int K2 = XK_2;
int K3 = XK_3;
int K4 = XK_4;
int K5 = XK_5;
int K6 = XK_6;
int K7 = XK_7;
int K8 = XK_8;
int K9 = XK_9;

int A = XK_a;
int B = XK_b;
int C = XK_c;
int D = XK_d;
int E = XK_e;
int F = XK_f;
int G = XK_g;
int H = XK_h;
int I = XK_i;
int J = XK_j;
int K = XK_k;
int L = XK_l;
int M = XK_m;
int N = XK_n;
int O = XK_o;
int P = XK_p;
int Q = XK_q;
int R = XK_r;
int S = XK_s;
int T = XK_t;
int U = XK_u;
int V = XK_v;
int W = XK_w;
int X = XK_x;
int Y = XK_y;
int Z = XK_z;

int Space = XK_space;

int Grave        = XK_grave;
int LeftBracket  = XK_bracketleft;
int RightBracket = XK_bracketright;
int Minus        = XK_minus;
int Equals       = XK_equal;
int Comma        = XK_comma;
int Period       = XK_period;
int Slash        = XK_slash;
int Backslash    = XK_backslash;

int Backspace    = XK_BackSpace;
int Return       = XK_Return;
int Escape       = XK_Escape;
int Tab          = XK_Tab;
int Home         = XK_Home;
int End          = XK_End;
int PageUp       = XK_Page_Up;
int PageDown     = XK_Page_Down;
int Insert       = XK_Insert;
int Delete       = XK_Delete;

int Pause        = XK_Break;
int PrintScreen  = XK_Sys_Req;

int CapsLock     = XK_Caps_Lock;
int NumLock      = XK_Num_Lock;
int ScrollLock   = XK_Scroll_Lock;

int LeftShift    = XK_Shift_L;
int RightShift   = XK_Shift_R;

int LeftCtrl     = XK_Control_L;
int RightCtrl    = XK_Control_R;

int LeftAlt      = XK_Alt_L;
int RightAlt     = XK_Alt_R;

int LeftGui      = XK_Meta_L;
int RightGui     = XK_Meta_R;

int Menu         = XK_Menu;
int ContextMenu  = -1;

int F1   = XK_F1;
int F2   = XK_F2;
int F3   = XK_F3;
int F4   = XK_F4;
int F5   = XK_F5;
int F6   = XK_F6;
int F7   = XK_F7;
int F8   = XK_F8;
int F9   = XK_F9;
int F10  = XK_F10;
int F11  = XK_F11;
int F12  = XK_F12;
int F13  = XK_F13;
int F14  = XK_F14;
int F15  = XK_F15;
int F16  = XK_F16;
int F17  = XK_F17;
int F18  = XK_F18;
int F19  = XK_F19;
int F20  = XK_F20;
int F21  = XK_F21;
int F22  = XK_F22;
int F23  = XK_F23;
int F24  = XK_F24;

int Up       = XK_Up;
int Down     = XK_Down;
int Left     = XK_Left;
int Right    = XK_Right;

int KP0           = XK_KP_0;
int KP1           = XK_KP_1;
int KP2           = XK_KP_2;
int KP3           = XK_KP_3;
int KP4           = XK_KP_4;
int KP5           = XK_KP_5;
int KP6           = XK_KP_6;
int KP7           = XK_KP_7;
int KP8           = XK_KP_8;
int KP9           = XK_KP_9;
int KP_Enter      = XK_KP_Enter;
int KP_Plus       = XK_KP_Add;
int KP_Minus      = XK_KP_Subtract;
int KP_Multiply   = XK_KP_Multiply;
int KP_Divide     = XK_KP_Divide;
int KP_Period     = XK_KP_Separator;

}//namespace Key
}//namespace Keyboard


namespace{

Display*   display;
int        screen;
Atom       WM_PROTOCOLS;
Atom       WM_DELETE_WINDOW;

struct WindowData{
    Window window;
    XWindowAttributes attrs;
    XImage* image = nullptr;
    XGCValues xgc_values;
    GC gc;
};

}//namespace


namespace Impl{

bool init()
{
    display = XOpenDisplay(nullptr);
    if(!display)
    {
        cerr << "Failed to open display!\n";
    }

    screen = DefaultScreen(display);

    WM_PROTOCOLS       = XInternAtom(display, "WM_PROTOCOLS", true);
    WM_DELETE_WINDOW   = XInternAtom(display, "WM_DELETE_WINDOW", true);
}


void cleanup()
{
    XCloseDisplay(display);
}


WindowHandle_t init_window_gl3(WindowData_t* wd)
{
    return 0;
}


WindowHandle_t init_window_normal(WindowData_t* wd)
{
    wd->window = XCreateSimpleWindow(
        display,
        RootWindow(display, screen),
        0, 0, 640, 480, 0,
        BlackPixel(display, screen), WhitePixel(display, screen)
    );

    XSetWMProtocols(display, wd->window, &WM_DELETE_WINDOW, 1);
    XSelectInput(display, wd->window, KeyPressMask);
    XGetWindowAttributes(display, wd->window, &wd->attrs);

    wd->gc = XCreateGC(display, wd->window, GCGraphicsExposures, &(wd->xgc_values));
}


void free_window(WindowHandle_t wh)
{

}


void show_window(WindowHandle_t wh)
{

}


void hide_window(WindowHandle_t wh)
{

}


void resize_window(WindowHandle_t wh, int w, int h)
{

}


void update_window_surface(WindowHandle_t wh)
{

}


void set_window_title(WindowHandle_t wh, const char* title)
{

}


const char* window_title(WindowHandle_t wh)
{

}


void turn_into_menu(WindowHandle_t window)
{

}


void process_some_events(Events* events)
{

}


/** @brief Get pressed mouse buttons. */
unsigned int pressed_mouse_buttons()
{
    return 0;
}


/** @brief Get pressed keyboard modifiers. */
unsigned int keyboard_modifiers()
{
    return 0;
}


WindowSurface get_window_surface(WindowHandle_t wh)
{

}


void* pixels(WindowSurface surface)
{

}


void get_surface_size(int &w, int &h, WindowSurface surface)
{

}


int bytes_per_pixel(WindowSurface surface)
{

}


void get_channel_indices(WindowSurface surface, int &r, int &g, int &b, int &a)
{

}

}

}//namespace r64fx