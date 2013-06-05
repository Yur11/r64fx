#include "Texture.h"
#include <png.h>
#include <GL/glu.h>
#include <iostream>

using namespace std;

namespace r64fx{
    

    
Texture::Texture(std::string path)
{
    FILE* file = fopen(path.c_str(), "rb");
    if(!file)
    {
        cerr << "Texture: Failed to read file '" << path << "' !\n";
        return;
    }
    
    char header[8];
    if(fread(header, 1, 8, file) < 8)
    {
        cerr << "Texture: Failed to read file header for '" << path << "' !\n";
        fclose(file);
        return;
    }
    
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if(!png_ptr)
    {
        cerr << "Texture: Failed to create png read structure!\n";
        fclose(file);
        return;
    }
    
    png_infop png_info = png_create_info_struct(png_ptr);
    if(!png_info)
    {
        cerr << "Texture: Failed to create png info structure!\n";
        fclose(file);
        return;
    }
    
    if(setjmp(png_jmpbuf(png_ptr)))
    {
        cerr << "Texture: Error during init_io!\n";
        fclose(file);
        return;
    }
    
    png_init_io(png_ptr, file);
    png_set_sig_bytes(png_ptr, 8);
    
    png_read_info(png_ptr, png_info);
    
    auto channel_count = png_get_channels(png_ptr, png_info);
    int mode;
    if(channel_count == 3)
    {
        mode = GL_RGB;
    }
    else if(channel_count == 4)
    {
        mode = GL_RGBA;
    }
    else
    {
        cerr << "Texture: Image format not supported!\n";
        fclose(file);
        return;
    }
    
    _width = png_get_image_width(png_ptr, png_info);
    _height = png_get_image_height(png_ptr, png_info);
    
    
    png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, png_info);
    
    png_byte* data = new png_byte[_height * png_get_rowbytes(png_ptr, png_info)];
    png_bytep* row_pointers = new png_bytep[_height];
    for(int i=0; i<_height; i++)
    {
        row_pointers[i] = data + i * png_get_rowbytes(png_ptr, png_info);
    }
    
    png_read_image(png_ptr, row_pointers);
    fclose(file);
    
    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

//     glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
//     glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    
    
    gluBuild2DMipmaps(GL_TEXTURE_2D, channel_count, _width, _height, mode, GL_UNSIGNED_BYTE, data);
    
    delete[] row_pointers;
    delete[] data;
}


Texture::~Texture()
{
    glDeleteTextures(1, &_texture);
}
    
}//namespace r64fx