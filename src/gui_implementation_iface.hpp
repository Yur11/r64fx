#ifndef R64FX_GUI_IMPLEMENTATION_IFACE_HPP
#define R64FX_GUI_IMPLEMENTATION_IFACE_HPP

namespace r64fx{

class Program;
class Window;
    
bool init_window_gl3(Window* window);

bool init_window_normal(Window* window);

void cleanup_window(Window* window);

void show_window(Window* window);

void hide_window(Window* window);

void resize_window(Window* window, int w, int h);

void process_some_events(Program* program);

/** @brief Get pressed mouse buttons. */
unsigned int pressed_mouse_buttons();

/** @brief Get pressed keyboard modifiers. */
unsigned int keyboard_modifiers();

};


#endif//R64FX_GUI_IMPLEMENTATION_IFACE_HPP