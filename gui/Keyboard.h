#ifndef R64FX_GUI_KEYBOARD_H
#define R64FX_GUI_KEYBOARD_H

#ifdef USE_SDL2
#include <SDL2/SDL.h>

namespace r64fx{

struct Keyboard{
    struct Key{
        static const unsigned int Backspace = SDL_SCANCODE_BACKSPACE;
        static const unsigned int Tab = SDL_SCANCODE_TAB;
        static const unsigned int Clear = SDL_SCANCODE_CLEAR;
        static const unsigned int Return = SDL_SCANCODE_RETURN;
        static const unsigned int Pause = SDL_SCANCODE_PAUSE;
        static const unsigned int Escape = SDL_SCANCODE_ESCAPE;
        static const unsigned int Space = SDL_SCANCODE_SPACE;
        static const unsigned int Comma = SDL_SCANCODE_COMMA;
        static const unsigned int Minus = SDL_SCANCODE_MINUS;
        static const unsigned int Period = SDL_SCANCODE_PERIOD;
        static const unsigned int Slash = SDL_SCANCODE_SLASH;
        static const unsigned int K0 = SDL_SCANCODE_0;
        static const unsigned int K1 = SDL_SCANCODE_1;
        static const unsigned int K2 = SDL_SCANCODE_2;
        static const unsigned int K3 = SDL_SCANCODE_3;
        static const unsigned int K4 = SDL_SCANCODE_4;
        static const unsigned int K5 = SDL_SCANCODE_5;
        static const unsigned int K6 = SDL_SCANCODE_6;
        static const unsigned int K7 = SDL_SCANCODE_7;
        static const unsigned int K8 = SDL_SCANCODE_8;
        static const unsigned int K9 = SDL_SCANCODE_9;
        static const unsigned int Semicolon = SDL_SCANCODE_SEMICOLON;
        static const unsigned int Equals = SDL_SCANCODE_EQUALS;
        static const unsigned int LeftBracket = SDL_SCANCODE_LEFTBRACKET;
        static const unsigned int Backslash = SDL_SCANCODE_BACKSLASH;
        static const unsigned int RightBracket = SDL_SCANCODE_RIGHTBRACKET;
        static const unsigned int A = SDL_SCANCODE_A;
        static const unsigned int B = SDL_SCANCODE_B;
        static const unsigned int C = SDL_SCANCODE_C;
        static const unsigned int D = SDL_SCANCODE_D;
        static const unsigned int E = SDL_SCANCODE_E;
        static const unsigned int F = SDL_SCANCODE_F;
        static const unsigned int G = SDL_SCANCODE_G;
        static const unsigned int H = SDL_SCANCODE_H;
        static const unsigned int I = SDL_SCANCODE_I;
        static const unsigned int J = SDL_SCANCODE_J;
        static const unsigned int K = SDL_SCANCODE_K;
        static const unsigned int L = SDL_SCANCODE_L;
        static const unsigned int M = SDL_SCANCODE_M;
        static const unsigned int N = SDL_SCANCODE_N;
        static const unsigned int O = SDL_SCANCODE_O;
        static const unsigned int P = SDL_SCANCODE_P;
        static const unsigned int Q = SDL_SCANCODE_Q;
        static const unsigned int R = SDL_SCANCODE_R;
        static const unsigned int S = SDL_SCANCODE_S;
        static const unsigned int T = SDL_SCANCODE_T;
        static const unsigned int U = SDL_SCANCODE_U;
        static const unsigned int V = SDL_SCANCODE_V;
        static const unsigned int W = SDL_SCANCODE_W;
        static const unsigned int X = SDL_SCANCODE_X;
        static const unsigned int Y = SDL_SCANCODE_Y;
        static const unsigned int Z = SDL_SCANCODE_Z;
        static const unsigned int Delete = SDL_SCANCODE_DELETE;
        static const unsigned int KP0 = SDL_SCANCODE_KP_0;
        static const unsigned int KP1 = SDL_SCANCODE_KP_1;
        static const unsigned int KP2 = SDL_SCANCODE_KP_2;
        static const unsigned int KP3 = SDL_SCANCODE_KP_3;
        static const unsigned int KP4 = SDL_SCANCODE_KP_4;
        static const unsigned int KP5 = SDL_SCANCODE_KP_5;
        static const unsigned int KP6 = SDL_SCANCODE_KP_6;
        static const unsigned int KP7 = SDL_SCANCODE_KP_7;
        static const unsigned int KP8 = SDL_SCANCODE_KP_8;
        static const unsigned int KP9 = SDL_SCANCODE_KP_9;
        static const unsigned int KP_Period = SDL_SCANCODE_KP_PERIOD;
        static const unsigned int KP_Divide = SDL_SCANCODE_KP_DIVIDE;
        static const unsigned int KP_Multiply = SDL_SCANCODE_KP_MULTIPLY;
        static const unsigned int KP_Minus = SDL_SCANCODE_KP_MINUS;
        static const unsigned int KP_Plus = SDL_SCANCODE_KP_PLUS;
        static const unsigned int KP_Enter = SDL_SCANCODE_KP_ENTER;
        static const unsigned int KP_Equals = SDL_SCANCODE_KP_EQUALS;
        static const unsigned int Up = SDL_SCANCODE_UP;
        static const unsigned int Down = SDL_SCANCODE_DOWN;
        static const unsigned int Right = SDL_SCANCODE_RIGHT;
        static const unsigned int Left = SDL_SCANCODE_LEFT;
        static const unsigned int Insert = SDL_SCANCODE_INSERT;
        static const unsigned int Home = SDL_SCANCODE_HOME;
        static const unsigned int End = SDL_SCANCODE_END;
        static const unsigned int PageUp = SDL_SCANCODE_PAGEUP;
        static const unsigned int PageDown = SDL_SCANCODE_PAGEDOWN;
        static const unsigned int F1 = SDL_SCANCODE_F1;
        static const unsigned int F2 = SDL_SCANCODE_F2;
        static const unsigned int F3 = SDL_SCANCODE_F3;
        static const unsigned int F4 = SDL_SCANCODE_F4;
        static const unsigned int F5 = SDL_SCANCODE_F5;
        static const unsigned int F6 = SDL_SCANCODE_F6;
        static const unsigned int F7 = SDL_SCANCODE_F7;
        static const unsigned int F8 = SDL_SCANCODE_F8;
        static const unsigned int F9 = SDL_SCANCODE_F9;
        static const unsigned int F10 = SDL_SCANCODE_F10;
        static const unsigned int F11 = SDL_SCANCODE_F11;
        static const unsigned int F12 = SDL_SCANCODE_F12;
        static const unsigned int F13 = SDL_SCANCODE_F13;
        static const unsigned int F14 = SDL_SCANCODE_F14;
        static const unsigned int F15 = SDL_SCANCODE_F15;
        static const unsigned int NumLock = SDL_SCANCODE_NUMLOCKCLEAR;
        static const unsigned int CapsLock = SDL_SCANCODE_CAPSLOCK;
        static const unsigned int ScrollLock = SDL_SCANCODE_SCROLLLOCK;
        static const unsigned int Rshift = SDL_SCANCODE_RSHIFT;
        static const unsigned int Lshift = SDL_SCANCODE_LSHIFT;
        static const unsigned int RightCtrl = SDL_SCANCODE_RCTRL;
        static const unsigned int LeftCtrl = SDL_SCANCODE_LCTRL;
        static const unsigned int RightAlt = SDL_SCANCODE_RALT;
        static const unsigned int LeftAlt = SDL_SCANCODE_LALT;
        static const unsigned int RightMeta = SDL_SCANCODE_RGUI;
        static const unsigned int LeftMeta = SDL_SCANCODE_LGUI;
        static const unsigned int Mode = SDL_SCANCODE_MODE;
        static const unsigned int Help = SDL_SCANCODE_HELP;
        static const unsigned int Print = SDL_SCANCODE_PRINTSCREEN;
        static const unsigned int Sysreq = SDL_SCANCODE_SYSREQ;
        static const unsigned int Menu = SDL_SCANCODE_MENU;
        static const unsigned int Power = SDL_SCANCODE_POWER;
        
        static const char* str(unsigned int);
    };
    
    struct Modifier{
        static const unsigned int None = KMOD_NONE;
        static const unsigned int LeftShift  = KMOD_LSHIFT;
        static const unsigned int RightShift = KMOD_RSHIFT;
        static const unsigned int Shift = KMOD_SHIFT;
        static const unsigned int LeftCtrl = KMOD_LCTRL;
        static const unsigned int RightCtrl = KMOD_RCTRL;
        static const unsigned int Ctrl = KMOD_CTRL;
        static const unsigned int LeftAlt = KMOD_LALT;
        static const unsigned int RightAlt = KMOD_RALT;
        static const unsigned int Alt = KMOD_ALT;
        static const unsigned int LeftGui = KMOD_LGUI;
        static const unsigned int RightGui = KMOD_RGUI;
        static const unsigned int Gui = KMOD_GUI;
        static const unsigned int Numlock = KMOD_NUM;
    };
    
    static void init();
    
    inline static unsigned int modifiers() { return SDL_GetModState(); }
    
    inline static void beginTextInput()
    {
        SDL_StartTextInput();
    }
    
    inline static void endTextInput()
    {
        SDL_StopTextInput();
    }
};

}//namespace r64fx

#else
#error 'No valid Keyboard implementation available!'
#endif//USE_SDL2

#endif//R64FX_GUI_KEYBOARD_H