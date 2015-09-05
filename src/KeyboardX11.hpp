#ifndef R64FX_KEYBOARD_X11_HPP
#define R64FX_KEYBOARD_X11_HPP

#include <X11/keysym.h>

namespace r64fx{

namespace Keyboard{
namespace Key{

constexpr int K0 = XK_0;
constexpr int K1 = XK_1;
constexpr int K2 = XK_2;
constexpr int K3 = XK_3;
constexpr int K4 = XK_4;
constexpr int K5 = XK_5;
constexpr int K6 = XK_6;
constexpr int K7 = XK_7;
constexpr int K8 = XK_8;
constexpr int K9 = XK_9;

constexpr int A = XK_a;
constexpr int B = XK_b;
constexpr int C = XK_c;
constexpr int D = XK_d;
constexpr int E = XK_e;
constexpr int F = XK_f;
constexpr int G = XK_g;
constexpr int H = XK_h;
constexpr int I = XK_i;
constexpr int J = XK_j;
constexpr int K = XK_k;
constexpr int L = XK_l;
constexpr int M = XK_m;
constexpr int N = XK_n;
constexpr int O = XK_o;
constexpr int P = XK_p;
constexpr int Q = XK_q;
constexpr int R = XK_r;
constexpr int S = XK_s;
constexpr int T = XK_t;
constexpr int U = XK_u;
constexpr int V = XK_v;
constexpr int W = XK_w;
constexpr int X = XK_x;
constexpr int Y = XK_y;
constexpr int Z = XK_z;

constexpr int Space = XK_space;

constexpr int Grave         = XK_grave;
constexpr int LeftBracket   = XK_bracketleft;
constexpr int RightBracket  = XK_bracketright;
constexpr int Minus         = XK_minus;
constexpr int Equals        = XK_equal;
constexpr int Comma         = XK_comma;
constexpr int Period        = XK_period;
constexpr int Slash         = XK_slash;
constexpr int Backslash     = XK_backslash;
constexpr int Semicolon     = XK_semicolon;
constexpr int Quote         = XK_quoteright;

constexpr int Backspace     = XK_BackSpace;
constexpr int Return        = XK_Return;
constexpr int Escape        = XK_Escape;
constexpr int Tab           = XK_Tab;
constexpr int Home          = XK_Home;
constexpr int End           = XK_End;
constexpr int PageUp        = XK_Page_Up;
constexpr int PageDown      = XK_Page_Down;
constexpr int Insert        = XK_Insert;
constexpr int Delete        = XK_Delete;

constexpr int Pause         = XK_Pause;
constexpr int PrintScreen   = XK_Sys_Req;

constexpr int CapsLock      = XK_Caps_Lock;
constexpr int NumLock       = XK_Num_Lock;
constexpr int ScrollLock    = XK_Scroll_Lock;

constexpr int LeftShift     = XK_Shift_L;
constexpr int RightShift    = XK_Shift_R;

constexpr int LeftCtrl      = XK_Control_L;
constexpr int RightCtrl     = XK_Control_R;

constexpr int LeftAlt       = XK_Alt_L;
constexpr int RightAlt      = XK_Alt_R;

constexpr int LeftGui       = XK_Meta_L;
constexpr int RightGui      = XK_Meta_R;

constexpr int ContextMenu   = XK_Menu;

constexpr int F1  = XK_F1;
constexpr int F2  = XK_F2;
constexpr int F3  = XK_F3;
constexpr int F4  = XK_F4;
constexpr int F5  = XK_F5;
constexpr int F6  = XK_F6;
constexpr int F7  = XK_F7;
constexpr int F8  = XK_F8;
constexpr int F9  = XK_F9;
constexpr int F10 = XK_F10;
constexpr int F11 = XK_F11;
constexpr int F12 = XK_F12;
constexpr int F13 = XK_F13;
constexpr int F14 = XK_F14;
constexpr int F15 = XK_F15;
constexpr int F16 = XK_F16;
constexpr int F17 = XK_F17;
constexpr int F18 = XK_F18;
constexpr int F19 = XK_F19;
constexpr int F20 = XK_F20;
constexpr int F21 = XK_F21;
constexpr int F22 = XK_F22;
constexpr int F23 = XK_F23;
constexpr int F24 = XK_F24;

constexpr int Up     = XK_Up;
constexpr int Down   = XK_Down;
constexpr int Left   = XK_Left;
constexpr int Right  = XK_Right;

constexpr int KP0  = XK_KP_Insert;
constexpr int KP1  = XK_KP_End;
constexpr int KP2  = XK_KP_Down;
constexpr int KP3  = XK_KP_Page_Down;
constexpr int KP4  = XK_KP_Left;
constexpr int KP5  = XK_KP_Begin;
constexpr int KP6  = XK_KP_Right;
constexpr int KP7  = XK_KP_Home;
constexpr int KP8  = XK_KP_Up;
constexpr int KP9  = XK_KP_Page_Up;
constexpr int KP_Enter      = XK_KP_Enter;
constexpr int KP_Plus       = XK_KP_Add;
constexpr int KP_Minus      = XK_KP_Subtract;
constexpr int KP_Multiply   = XK_KP_Multiply;
constexpr int KP_Divide     = XK_KP_Divide;
constexpr int KP_Period     = XK_KP_Delete;

}//namespace Key
}//namespace Keyboard

}//namespace r64fx


#endif//R64FX_KEYBOARD_X11_HPP