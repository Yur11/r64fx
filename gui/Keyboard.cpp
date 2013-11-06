#include "Keyboard.h"

#ifdef USE_SDL2

#include <map>

using namespace std;

namespace r64fx{

const unsigned int Keyboard::Key::Backspace;
const unsigned int Keyboard::Key::Tab;
const unsigned int Keyboard::Key::Clear;
const unsigned int Keyboard::Key::Return;
const unsigned int Keyboard::Key::Pause;
const unsigned int Keyboard::Key::Escape;
const unsigned int Keyboard::Key::Space;
const unsigned int Keyboard::Key::Comma;
const unsigned int Keyboard::Key::Minus;
const unsigned int Keyboard::Key::Period;
const unsigned int Keyboard::Key::Slash;
const unsigned int Keyboard::Key::K0;
const unsigned int Keyboard::Key::K1;
const unsigned int Keyboard::Key::K2;
const unsigned int Keyboard::Key::K3;
const unsigned int Keyboard::Key::K4;
const unsigned int Keyboard::Key::K5;
const unsigned int Keyboard::Key::K6;
const unsigned int Keyboard::Key::K7;
const unsigned int Keyboard::Key::K8;
const unsigned int Keyboard::Key::K9;
const unsigned int Keyboard::Key::Semicolon;
const unsigned int Keyboard::Key::Equals;
const unsigned int Keyboard::Key::LeftBracket;
const unsigned int Keyboard::Key::Backslash;
const unsigned int Keyboard::Key::RightBracket;
const unsigned int Keyboard::Key::A;
const unsigned int Keyboard::Key::B;
const unsigned int Keyboard::Key::C;
const unsigned int Keyboard::Key::D;
const unsigned int Keyboard::Key::E;
const unsigned int Keyboard::Key::F;
const unsigned int Keyboard::Key::G;
const unsigned int Keyboard::Key::H;
const unsigned int Keyboard::Key::I;
const unsigned int Keyboard::Key::J;
const unsigned int Keyboard::Key::K;
const unsigned int Keyboard::Key::L;
const unsigned int Keyboard::Key::M;
const unsigned int Keyboard::Key::N;
const unsigned int Keyboard::Key::O;
const unsigned int Keyboard::Key::P;
const unsigned int Keyboard::Key::Q;
const unsigned int Keyboard::Key::R;
const unsigned int Keyboard::Key::S;
const unsigned int Keyboard::Key::T;
const unsigned int Keyboard::Key::U;
const unsigned int Keyboard::Key::V;
const unsigned int Keyboard::Key::W;
const unsigned int Keyboard::Key::X;
const unsigned int Keyboard::Key::Y;
const unsigned int Keyboard::Key::Z;
const unsigned int Keyboard::Key::Delete;
const unsigned int Keyboard::Key::KP0;
const unsigned int Keyboard::Key::KP1;
const unsigned int Keyboard::Key::KP2;
const unsigned int Keyboard::Key::KP3;
const unsigned int Keyboard::Key::KP4;
const unsigned int Keyboard::Key::KP5;
const unsigned int Keyboard::Key::KP6;
const unsigned int Keyboard::Key::KP7;
const unsigned int Keyboard::Key::KP8;
const unsigned int Keyboard::Key::KP9;
const unsigned int Keyboard::Key::KP_Period;
const unsigned int Keyboard::Key::KP_Divide;
const unsigned int Keyboard::Key::KP_Multiply;
const unsigned int Keyboard::Key::KP_Minus;
const unsigned int Keyboard::Key::KP_Plus;
const unsigned int Keyboard::Key::KP_Enter;
const unsigned int Keyboard::Key::KP_Equals;
const unsigned int Keyboard::Key::Up;
const unsigned int Keyboard::Key::Down;
const unsigned int Keyboard::Key::Right;
const unsigned int Keyboard::Key::Left;
const unsigned int Keyboard::Key::Insert;
const unsigned int Keyboard::Key::Home;
const unsigned int Keyboard::Key::End;
const unsigned int Keyboard::Key::PageUp;
const unsigned int Keyboard::Key::PageDown;
const unsigned int Keyboard::Key::F1;
const unsigned int Keyboard::Key::F2;
const unsigned int Keyboard::Key::F3;
const unsigned int Keyboard::Key::F4;
const unsigned int Keyboard::Key::F5;
const unsigned int Keyboard::Key::F6;
const unsigned int Keyboard::Key::F7;
const unsigned int Keyboard::Key::F8;
const unsigned int Keyboard::Key::F9;
const unsigned int Keyboard::Key::F10;
const unsigned int Keyboard::Key::F11;
const unsigned int Keyboard::Key::F12;
const unsigned int Keyboard::Key::F13;
const unsigned int Keyboard::Key::F14;
const unsigned int Keyboard::Key::F15;
const unsigned int Keyboard::Key::NumLock;
const unsigned int Keyboard::Key::CapsLock;
const unsigned int Keyboard::Key::ScrollLock;
const unsigned int Keyboard::Key::Rshift;
const unsigned int Keyboard::Key::Lshift;
const unsigned int Keyboard::Key::RightCtrl;
const unsigned int Keyboard::Key::LeftCtrl;
const unsigned int Keyboard::Key::RightAlt;
const unsigned int Keyboard::Key::LeftAlt;
const unsigned int Keyboard::Key::RightMeta;
const unsigned int Keyboard::Key::LeftMeta;
const unsigned int Keyboard::Key::Mode;
const unsigned int Keyboard::Key::Help;
const unsigned int Keyboard::Key::Print;
const unsigned int Keyboard::Key::Sysreq;
const unsigned int Keyboard::Key::Menu;
const unsigned int Keyboard::Key::Power;
    

const unsigned int Keyboard::Modifier::None;
const unsigned int Keyboard::Modifier::LeftShift;
const unsigned int Keyboard::Modifier::RightShift;
const unsigned int Keyboard::Modifier::LeftCtrl;
const unsigned int Keyboard::Modifier::RightCtrl;
const unsigned int Keyboard::Modifier::LeftAlt;
const unsigned int Keyboard::Modifier::RightAlt;
const unsigned int Keyboard::Modifier::LeftGui;
const unsigned int Keyboard::Modifier::RightGui;
const unsigned int Keyboard::Modifier::Numlock;

map<unsigned int, const char*> key2str;

void Keyboard::init()
{
    key2str[Keyboard::Key::Backspace] = "Backspace";
    key2str[Keyboard::Key::Tab] = "Tab";
    key2str[Keyboard::Key::Clear] = "Clear";
    key2str[Keyboard::Key::Return] = "Return";
    key2str[Keyboard::Key::Pause] = "Pause";
    key2str[Keyboard::Key::Escape] = "Escape";
    key2str[Keyboard::Key::Space] = "Space";
    key2str[Keyboard::Key::Comma] = ",";
    key2str[Keyboard::Key::Minus] = "-";
    key2str[Keyboard::Key::Period] = ".";
    key2str[Keyboard::Key::Slash] = "/";
    key2str[Keyboard::Key::K0] = "0";
    key2str[Keyboard::Key::K1] = "1";
    key2str[Keyboard::Key::K2] = "2";
    key2str[Keyboard::Key::K3] = "3";
    key2str[Keyboard::Key::K4] = "4";
    key2str[Keyboard::Key::K5] = "5";
    key2str[Keyboard::Key::K6] = "6";
    key2str[Keyboard::Key::K7] = "7";
    key2str[Keyboard::Key::K8] = "8";
    key2str[Keyboard::Key::K9] = "9";
    key2str[Keyboard::Key::Semicolon] = ";";
    key2str[Keyboard::Key::Equals] = "=";
    key2str[Keyboard::Key::LeftBracket] = "(";
    key2str[Keyboard::Key::Backslash] = "\\";
    key2str[Keyboard::Key::RightBracket] = ")";
    key2str[Keyboard::Key::A] = "A";
    key2str[Keyboard::Key::B] = "B";
    key2str[Keyboard::Key::C] = "C";
    key2str[Keyboard::Key::D] = "D";
    key2str[Keyboard::Key::E] = "E";
    key2str[Keyboard::Key::F] = "F";
    key2str[Keyboard::Key::G] = "G";
    key2str[Keyboard::Key::H] = "H";
    key2str[Keyboard::Key::I] = "I";
    key2str[Keyboard::Key::J] = "J";
    key2str[Keyboard::Key::K] = "K";
    key2str[Keyboard::Key::L] = "L";
    key2str[Keyboard::Key::M] = "M";
    key2str[Keyboard::Key::N] = "N";
    key2str[Keyboard::Key::O] = "O";
    key2str[Keyboard::Key::P] = "P";
    key2str[Keyboard::Key::Q] = "Q";
    key2str[Keyboard::Key::R] = "R";
    key2str[Keyboard::Key::S] = "S";
    key2str[Keyboard::Key::T] = "T";
    key2str[Keyboard::Key::U] = "U";
    key2str[Keyboard::Key::V] = "V";
    key2str[Keyboard::Key::W] = "W";
    key2str[Keyboard::Key::X] = "X";
    key2str[Keyboard::Key::Y] = "Y";
    key2str[Keyboard::Key::Z] = "Z";
    key2str[Keyboard::Key::Delete] = "Delete";
    key2str[Keyboard::Key::KP0] = "Keypad 0";
    key2str[Keyboard::Key::KP1] = "Keypad 1";
    key2str[Keyboard::Key::KP2] = "Keypad 2";
    key2str[Keyboard::Key::KP3] = "Keypad 3";
    key2str[Keyboard::Key::KP4] = "Keypad 4";
    key2str[Keyboard::Key::KP5] = "Keypad 5";
    key2str[Keyboard::Key::KP6] = "Keypad 6";
    key2str[Keyboard::Key::KP7] = "Keypad 7";
    key2str[Keyboard::Key::KP8] = "Keypad 8";
    key2str[Keyboard::Key::KP9] = "Keypad 9";
    key2str[Keyboard::Key::KP_Period] = "Keypad .";
    key2str[Keyboard::Key::KP_Divide] = "Keypad /";
    key2str[Keyboard::Key::KP_Multiply] = "Keypad *";
    key2str[Keyboard::Key::KP_Minus] = "Keypad -";
    key2str[Keyboard::Key::KP_Plus] = "Keypad +";
    key2str[Keyboard::Key::KP_Enter] = "Enter";
    key2str[Keyboard::Key::KP_Equals] = "Keypad =";
    key2str[Keyboard::Key::Up] = "Up";
    key2str[Keyboard::Key::Down] = "Down";
    key2str[Keyboard::Key::Right] = "Right";
    key2str[Keyboard::Key::Left] = "Left";
    key2str[Keyboard::Key::Insert] = "Insert";
    key2str[Keyboard::Key::Home] = "Home";
    key2str[Keyboard::Key::End] = "End";
    key2str[Keyboard::Key::PageUp] = "PageUp";
    key2str[Keyboard::Key::PageDown] = "PageDown";
    key2str[Keyboard::Key::F1] = "F1";
    key2str[Keyboard::Key::F2] = "F2";
    key2str[Keyboard::Key::F3] = "F3";
    key2str[Keyboard::Key::F4] = "F4";
    key2str[Keyboard::Key::F5] = "F5";
    key2str[Keyboard::Key::F6] = "F6";
    key2str[Keyboard::Key::F7] = "F7";
    key2str[Keyboard::Key::F8] = "F8";
    key2str[Keyboard::Key::F9] = "F9";
    key2str[Keyboard::Key::F10] = "F10";
    key2str[Keyboard::Key::F11] = "F11";
    key2str[Keyboard::Key::F12] = "F12";
    key2str[Keyboard::Key::F13] = "F13";
    key2str[Keyboard::Key::F14] = "F14";
    key2str[Keyboard::Key::F15] = "F15";
    key2str[Keyboard::Key::NumLock] = "NumLock";
    key2str[Keyboard::Key::CapsLock] = "CapsLock";
    key2str[Keyboard::Key::ScrollLock] = "ScrollLock";
    key2str[Keyboard::Key::Rshift] = "Right Shift";
    key2str[Keyboard::Key::Lshift] = "Left Shift";
    key2str[Keyboard::Key::RightCtrl] = "Right Ctrl";
    key2str[Keyboard::Key::LeftCtrl] = "Left Ctrl";
    key2str[Keyboard::Key::RightAlt] = "Right Alt";
    key2str[Keyboard::Key::LeftAlt] = "Left Alt";
    key2str[Keyboard::Key::RightMeta] = "Right Meta";
    key2str[Keyboard::Key::LeftMeta] = "Left Meta";
    key2str[Keyboard::Key::Mode] = "Mode";
    key2str[Keyboard::Key::Help] = "Help";
    key2str[Keyboard::Key::Print] = "Print";
    key2str[Keyboard::Key::Sysreq] = "Sysreq";
    key2str[Keyboard::Key::Menu] = "Menu";
    key2str[Keyboard::Key::Power] = "Power";
}


const char* Keyboard::Key::str(unsigned int key)
{
    auto it = key2str.find(key);
    if(it == key2str.end()) return nullptr;
    return it->second;
}


}//namespace r64fx

#endif//USE_SDL2