#include "Texture.h"
#include "shared_sources/read_png.h"
#include <GL/glu.h>
#include <vector>
#ifdef DEBUG
#include <assert.h>
#endif//DEBUG

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
        cerr << "Texture: Failed to find and read texture file \"" << name << "\" !\n";
        return;
    }
    
    unsigned char* data;
    int width, height, nchannels;
    if(!read_png(file, data, nchannels, width, height))
    {
        cerr << "Texture: Error reading png file: \"" << name << "\" !\n";
        fclose(file);
        return;
    }
    
    cout << width << "x" << height << ": " << nchannels << "\n";
    
    fclose(file);
    
#ifdef DEBUG
    assert(nchannels == 3 || nchannels == 4);
#endif//DEBUG
    load_to_vram(width, height, nchannels, (nchannels == 3 ? GL_RGB : GL_RGBA), data);
    
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
        
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, mode, GL_UNSIGNED_BYTE, bytes);
    glDisable(GL_TEXTURE_2D);
    
    _width = width;
    _height = height;
}


Texture::Texture(vector<unsigned char> resource_bytes)
{
    int channel_count = resource_bytes[0];
    
    union{
        int num;
        unsigned char bytes[4];
    } cast;
    
    for(int i=0; i<4; i++)
        cast.bytes[i] = resource_bytes[i+1];
    int width = cast.num;
    
    for(int i=0; i<4; i++)
        cast.bytes[i] = resource_bytes[i+5];
    int height = cast.num;
    
    load_to_vram(width, height, channel_count, (channel_count == 3 ? GL_RGB : GL_RGBA), resource_bytes.data() + 9);
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