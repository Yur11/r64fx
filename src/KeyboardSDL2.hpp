#ifndef R64FX_KEYBOARD_SDL2_HPP
#define R64FX_KEYBOARD_SDL2_HPP

#include <SDL2/SDL_keycode.h>

namespace r64fx{

namespace Keyboard{
namespace Key{

constexpr int K0 = SDL_SCANCODE_0;
constexpr int K1 = SDL_SCANCODE_1;
constexpr int K2 = SDL_SCANCODE_2;
constexpr int K3 = SDL_SCANCODE_3;
constexpr int K4 = SDL_SCANCODE_4;
constexpr int K5 = SDL_SCANCODE_5;
constexpr int K6 = SDL_SCANCODE_6;
constexpr int K7 = SDL_SCANCODE_7;
constexpr int K8 = SDL_SCANCODE_8;
constexpr int K9 = SDL_SCANCODE_9;

constexpr int A = SDL_SCANCODE_A;
constexpr int B = SDL_SCANCODE_B;
constexpr int C = SDL_SCANCODE_C;
constexpr int D = SDL_SCANCODE_D;
constexpr int E = SDL_SCANCODE_E;
constexpr int F = SDL_SCANCODE_F;
constexpr int G = SDL_SCANCODE_G;
constexpr int H = SDL_SCANCODE_H;
constexpr int I = SDL_SCANCODE_I;
constexpr int J = SDL_SCANCODE_J;
constexpr int K = SDL_SCANCODE_K;
constexpr int L = SDL_SCANCODE_L;
constexpr int M = SDL_SCANCODE_M;
constexpr int N = SDL_SCANCODE_N;
constexpr int O = SDL_SCANCODE_O;
constexpr int P = SDL_SCANCODE_P;
constexpr int Q = SDL_SCANCODE_Q;
constexpr int R = SDL_SCANCODE_R;
constexpr int S = SDL_SCANCODE_S;
constexpr int T = SDL_SCANCODE_T;
constexpr int U = SDL_SCANCODE_U;
constexpr int V = SDL_SCANCODE_V;
constexpr int W = SDL_SCANCODE_W;
constexpr int X = SDL_SCANCODE_X;
constexpr int Y = SDL_SCANCODE_Y;
constexpr int Z = SDL_SCANCODE_Z;

constexpr int Space = SDL_SCANCODE_SPACE;

constexpr int Grave         = SDL_SCANCODE_GRAVE;
constexpr int LeftBracket   = SDL_SCANCODE_LEFTBRACKET;
constexpr int RightBracket  = SDL_SCANCODE_RIGHTBRACKET;
constexpr int Minus         = SDL_SCANCODE_MINUS;
constexpr int Equals        = SDL_SCANCODE_EQUALS;
constexpr int Comma         = SDL_SCANCODE_COMMA;
constexpr int Period        = SDL_SCANCODE_PERIOD;
constexpr int Slash         = SDL_SCANCODE_SLASH;
constexpr int Backslash     = SDL_SCANCODE_BACKSLASH;

constexpr int Backspace     = SDL_SCANCODE_BACKSPACE;
constexpr int Return        = SDL_SCANCODE_RETURN;
constexpr int Escape        = SDL_SCANCODE_ESCAPE;
constexpr int Tab           = SDL_SCANCODE_TAB;
constexpr int Home          = SDL_SCANCODE_HOME;
constexpr int End           = SDL_SCANCODE_END;
constexpr int PageUp        = SDL_SCANCODE_PAGEUP;
constexpr int PageDown      = SDL_SCANCODE_PAGEDOWN;
constexpr int Insert        = SDL_SCANCODE_INSERT;
constexpr int Delete        = SDL_SCANCODE_DELETE;

constexpr int Pause         = SDL_SCANCODE_PAUSE;
constexpr int PrintScreen   = SDL_SCANCODE_PRINTSCREEN;

constexpr int CapsLock      = SDL_SCANCODE_CAPSLOCK;
constexpr int NumLock       = SDL_SCANCODE_NUMLOCKCLEAR;
constexpr int ScrollLock    = SDL_SCANCODE_SCROLLLOCK;

constexpr int LeftShift     = SDL_SCANCODE_LSHIFT;
constexpr int RightShift    = SDL_SCANCODE_RSHIFT;

constexpr int LeftCtrl      = SDL_SCANCODE_LCTRL;
constexpr int RightCtrl     = SDL_SCANCODE_RCTRL;

constexpr int LeftAlt       = SDL_SCANCODE_LALT;
constexpr int RightAlt      = SDL_SCANCODE_RALT;

constexpr int LeftGui       = SDL_SCANCODE_LGUI;
constexpr int RightGui      = SDL_SCANCODE_RGUI;

constexpr int ContextMenu   = SDL_SCANCODE_APPLICATION;

constexpr int F1  = SDL_SCANCODE_F1;
constexpr int F2  = SDL_SCANCODE_F2;
constexpr int F3  = SDL_SCANCODE_F3;
constexpr int F4  = SDL_SCANCODE_F4;
constexpr int F5  = SDL_SCANCODE_F5;
constexpr int F6  = SDL_SCANCODE_F6;
constexpr int F7  = SDL_SCANCODE_F7;
constexpr int F8  = SDL_SCANCODE_F8;
constexpr int F9  = SDL_SCANCODE_F9;
constexpr int F10 = SDL_SCANCODE_F10;
constexpr int F11 = SDL_SCANCODE_F11;
constexpr int F12 = SDL_SCANCODE_F12;
constexpr int F13 = SDL_SCANCODE_F13;
constexpr int F14 = SDL_SCANCODE_F14;
constexpr int F15 = SDL_SCANCODE_F15;
constexpr int F16 = SDL_SCANCODE_F16;
constexpr int F17 = SDL_SCANCODE_F17;
constexpr int F18 = SDL_SCANCODE_F18;
constexpr int F19 = SDL_SCANCODE_F19;
constexpr int F20 = SDL_SCANCODE_F20;
constexpr int F21 = SDL_SCANCODE_F21;
constexpr int F22 = SDL_SCANCODE_F22;
constexpr int F23 = SDL_SCANCODE_F23;
constexpr int F24 = SDL_SCANCODE_F24;

constexpr int Up     = SDL_SCANCODE_UP;
constexpr int Down   = SDL_SCANCODE_DOWN;
constexpr int Left   = SDL_SCANCODE_LEFT;
constexpr int Right  = SDL_SCANCODE_RIGHT;

constexpr int KP0  = SDL_SCANCODE_KP_0;
constexpr int KP1  = SDL_SCANCODE_KP_1;
constexpr int KP2  = SDL_SCANCODE_KP_2;
constexpr int KP3  = SDL_SCANCODE_KP_3;
constexpr int KP4  = SDL_SCANCODE_KP_4;
constexpr int KP5  = SDL_SCANCODE_KP_5;
constexpr int KP6  = SDL_SCANCODE_KP_6;
constexpr int KP7  = SDL_SCANCODE_KP_7;
constexpr int KP8  = SDL_SCANCODE_KP_8;
constexpr int KP9  = SDL_SCANCODE_KP_9;
constexpr int KP_Enter      = SDL_SCANCODE_KP_ENTER;
constexpr int KP_Plus       = SDL_SCANCODE_KP_PLUS;
constexpr int KP_Minus      = SDL_SCANCODE_KP_MINUS;
constexpr int KP_Multiply   = SDL_SCANCODE_KP_MULTIPLY;
constexpr int KP_Divide     = SDL_SCANCODE_KP_DIVIDE;
constexpr int KP_Period     = SDL_SCANCODE_KP_PERIOD;

}//namespace Key
}//namespace Keyboard

}//namespace r64fx

#endif//R64FX_KEYBOARD_SDL2_HPP