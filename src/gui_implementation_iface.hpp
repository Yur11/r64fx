#ifndef R64FX_GUI_IMPLEMENTATION_IFACE_HPP
#define R64FX_GUI_IMPLEMENTATION_IFACE_HPP


namespace r64fx{

class ProgramImplEventIface;
class WindowImplIface;

bool init();

void cleanup();
    
bool init_window_gl3(WindowImplIface* window);

bool init_window_normal(WindowImplIface* window);

void cleanup_window(WindowImplIface* window);

void show_window(WindowImplIface* window);

void hide_window(WindowImplIface* window);

void resize_window(WindowImplIface* window, int w, int h);

void turn_into_menu(WindowImplIface* window);

void process_some_events(ProgramImplEventIface* program);

/** @brief Get pressed mouse buttons. */
unsigned int pressed_mouse_buttons();

/** @brief Get pressed keyboard modifiers. */
unsigned int keyboard_modifiers();

};


#endif//R64FX_GUI_IMPLEMENTATION_IFACE_HPP