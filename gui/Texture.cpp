#include "Texture.h"
#include <png.h>
#include <GL/glu.h>
#include <iostream>
#include <vector>

using namespace std;

namespace r64fx{
   
    
vector<Texture> all_textures;
std::vector<std::string>* texture_data_paths;
    
Texture::Texture(std::string name)
{    
    FILE* file = fopen(name.c_str(), "rb");//Try absolute path first.
    if(!file && texture_data_paths)
    {
        /* Check all known data directories. */
        for(auto it=texture_data_paths->begin(); it!=texture_data_paths->end(); it++)
        {
            auto &path = *it;
            file = fopen((path + name).c_str(), "rb");
            if(file)
                break;
        }
    }
    
    if(!file)
    {
        cerr << "Texture: Failed to find and read texture file '" << name << "' !\n";
        return;
    }
    
    char header[8];
    if(fread(header, 1, 8, file) < 8)
    {
        cerr << "Texture: Failed to read file header for '" << name << "' !\n";
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
    
    auto width = png_get_image_width(png_ptr, png_info);
    auto height = png_get_image_height(png_ptr, png_info);
    
    
    png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, png_info);
    
    png_byte* data = new png_byte[height * png_get_rowbytes(png_ptr, png_info)];
    png_bytep* row_pointers = new png_bytep[height];
    for(int i=0; i<(int)height; i++)
    {
        row_pointers[i] = data + i * png_get_rowbytes(png_ptr, png_info);
    }
    
    png_read_image(png_ptr, row_pointers);
    fclose(file);
    
    load_to_vram(width, height, channel_count, mode, data);
    
    delete[] row_pointers;
    delete[] data;
    
    all_textures.push_back(*this);
}


void Texture::load_to_vram(int width, int height, int channel_count, int mode, unsigned char* bytes)
{
    glEnable(GL_TEXTURE_2D);
    
    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
    glDisable(GL_TEXTURE_2D);
    
    _width = width;
    _height = height;
}


Texture::~Texture()
{
}



Texture default_texture;
Texture transparent_16x16;


void Texture::init(std::vector<std::string>* data_paths)
{
    texture_data_paths = data_paths;
    
    /** Create default texture. */
    const int width = 32;
    const int height = 32;
    const int mode = GL_RGBA;
    unsigned char bytes[width * height * 4];
    
    for(int y=0; y<height; y++)
    {
        for(int x=0; x<width; x++)
        {
            unsigned color;
            if(y < height / 2)
            {
                if(x < width / 2)
                {
                    color = 0;
                }
                else
                {
                    color = 255;
                }
            }
            else
            {
                if(x < width / 2)
                {
                    color = 255;
                }
                else
                {
                    color = 0;
                }
            }
            
            for(int ch=0; ch<3; ch++)
            {
                bytes[width * y * 4 + x * 4 + ch] = color;
            }
            
            bytes[width * y * 4 + x * 4 + 3] = 255;
        }
    }
    
    default_texture = Texture(width, height, 4, mode, bytes);


    /* Create transparent 16x16 texture. */
    for(int y=0; y<16; y++)
    {
        for(int x=0; x<16; x++)
        {
            for(int ch=0; ch<3; ch++)
            {
                bytes[16 * y * 4 + x * 4 + ch] = 0;
            }

            bytes[16 * y * 4 + x * 4 + 3] = 0;
        }
    }

    transparent_16x16 = Texture(16, 16, 4, GL_RGBA, bytes);
}


Texture Texture::defaultTexture()
{
    return default_texture;
}


Texture Texture::badTexture()
{
    return Texture();
}


Texture Texture::transparent16x16()
{
    return transparent_16x16;
}


void Texture::cleanup()
{
    all_textures.clear();
}
    
}//namespace r64fx