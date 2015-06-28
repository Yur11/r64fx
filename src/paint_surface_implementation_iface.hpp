#ifndef R64FX_PAINT_SURFACE_IMPLEMENTATION_IFACE_HPP
#define R64FX_PAINT_SURFACE_IMPLEMENTATION_IFACE_HPP

namespace r64fx{

class WindowImplIface;

namespace Impl{

typedef void* Surface;

Surface get_window_surface(WindowImplIface* window);

void* pixels(Surface surface);

void get_surface_size(int &w, int &h, Surface surface);

int bytes_per_pixel(Surface surface);

void get_channel_indices(Surface surface, int &r, int &g, int &b, int &a);

}//namespace Impl

}//namespace r64fx

#endif//R64FX_PAINT_SURFACE_IMPLEMENTATION_IFACE_HPP