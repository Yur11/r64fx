#include "Mouse.h"
#include "shared_sources/read_png.h"

#ifdef DEBUG
#include <assert.h>
#endif//DEBUG

#include <iostream>

#ifdef USE_SDL2
#include "SDL2/SDL_surface.h"
#endif//USE_SDL2

using namespace std;

namespace r64fx{

extern string data_prefix;
    
#ifdef USE_SDL2

SDL_Cursor* Mouse::smile_cursor;

SDL_Cursor* create_cursor_from_png(string file_name)
{
    auto file = fopen(file_name.c_str(), "rb");
    if(!file)
    {
        cerr << "Failed to read " << file_name << "\n";
        return nullptr;
    }
    
    unsigned char* pixels;
    int nchannels, w, h;
    if(!read_png(file, pixels, nchannels, w, h))
    {
        cerr << "Failed to read png file: " << file_name << "\n";
        return nullptr;
    }
    
#ifdef DEBUG
    assert(nchannels == 4);
#endif//DEBUG
    
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(pixels, w, h, nchannels*8, w*nchannels, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
#ifdef DEBUG
    assert(surface != nullptr);
#endif//DEBUG
    
    SDL_Cursor* cursor = SDL_CreateColorCursor(surface, w/2, h/2);
    
    fclose(file);

    return cursor;
}

    
void Mouse::init()
{
    smile_cursor = create_cursor_from_png(data_prefix + "textures/alpha_smile.png");
#ifdef DEBUG
    assert(smile_cursor != nullptr);
#endif//DEBUG
}


void Mouse::cleanup()
{

}


void Mouse::useDefaultCursor()
{
    auto cursor = SDL_GetDefaultCursor();
#ifdef DEBUG
    assert(cursor != nullptr);
#endif//DEBUG
    SDL_SetCursor(cursor);
}

#endif//USE_SDL2
    
}//namespace r64fx

