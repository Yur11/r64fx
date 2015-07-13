#ifndef R64FX_GUI_IMPLEMENTATION_IFACE_HPP
#define R64FX_GUI_IMPLEMENTATION_IFACE_HPP

namespace r64fx{

namespace Impl{

typedef void* WindowHandle_t;
typedef void WindowData_t;
typedef void* Surface;

bool init();

void cleanup();
    
WindowHandle_t init_window_gl3(WindowData_t* wd);

WindowHandle_t init_window_normal(WindowData_t* wd);

void free_window(WindowHandle_t wh);

void show_window(WindowHandle_t wh);

void hide_window(WindowHandle_t wh);

void resize_window(WindowHandle_t wh, int w, int h);

void update_window_surface(WindowHandle_t wh);

void set_window_title(WindowHandle_t wh, const char* title);

const char* window_title(WindowHandle_t wh);

void turn_into_menu(WindowHandle_t window);

struct Events{
    void (*mouse_press)   (WindowData_t* wd, float x, float y, unsigned int buttons);
    void (*mouse_release) (WindowData_t* wd, float x, float y, unsigned int buttons);
    void (*mouse_move)    (WindowData_t* wd, float x, float y, unsigned int buttons);
    void (*key_press)     (WindowData_t* wd, int key);
    void (*key_release)   (WindowData_t* wd, int key);
    void (*resize)        (WindowData_t* wd, int w, int h);
    void (*close)         (WindowData_t* wd);
};

void process_some_events(Events* events);

/** @brief Get pressed mouse buttons. */
unsigned int pressed_mouse_buttons();

/** @brief Get pressed keyboard modifiers. */
unsigned int keyboard_modifiers();

Surface get_window_surface(WindowHandle_t* wh);

void* pixels(Surface surface);

void get_surface_size(int &w, int &h, Surface surface);

int bytes_per_pixel(Surface surface);

void get_channel_indices(Surface surface, int &r, int &g, int &b, int &a);

}//namespace Impl

}//namespace r64x


#endif//R64FX_GUI_IMPLEMENTATION_IFACE_HPP