#include "Texture.h"
#include "shared_sources/read_png.h"
#include <GL/glu.h>
#include <vector>
#ifdef DEBUG
#include <assert.h>
#endif//DEBUG

using namespace std;

namespace r64fx{
  
extern string data_prefix;

Texture default_texture;

    
Texture::Texture(std::string name)
{
    (*this) = Texture(fopen((data_prefix + name).c_str(), "r"), true);
    if(!isGood())
    {
        cerr << "Problems reading file \"" << name << "\" !\n";
    }
}


Texture::Texture(FILE* fd, bool close_fd)
{
    if(!fd)
    {
        cerr << "Texture: Bad file descriptor !\n";
        return;
    }
    
    unsigned char* data;
    int width, height, nchannels;
    if(!read_png(fd, data, nchannels, width, height))
    {
        cerr << "Texture: Error reading png file!\n";
        if(close_fd)
            fclose(fd);
        return;
    }
    
    cout << width << "x" << height << ": " << nchannels << "\n";
    
    if(close_fd)
        fclose(fd);
    
#ifdef DEBUG
    assert(nchannels == 3 || nchannels == 4);
#endif//DEBUG
    load_to_vram(width, height, nchannels, (nchannels == 3 ? GL_RGB : GL_RGBA), data);
    
    delete[] data;
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


void Texture::free() 
{
    glDeleteTextures(1, &_texture); 
    _width = _height = 0; 
}


void Texture::init()
{
    const int width = 32;
    const int height = 32;

    auto data = new unsigned char[width*height*3];
    for(int x=0; x<width; x++)
    {
        for(int y=0; y<height; y++)
        {
            for(int c=0; c<3; c++)
            {
                data[y*width + x + c] = 0;
            }
        }
    }
    
    for(int x=0; x<width/2; x++)
    {
        for(int y=0; y<height/2; y++)
        {
            for(int c=0; c<3; c++)
            {
                data[y*width + x + c] = 255;
            }
        }
    }
    
    for(int x=width/2; x<width; x++)
    {
        for(int y=height/2; y<height; y++)
        {
            for(int c=0; c<3; c++)
            {
                data[y*width + x + c] = 255;
            }
        }
    }
    
    default_texture = Texture(width, height, 3, GL_RGB, data);
    
    delete[] data;
}


Texture Texture::defaultTexture()
{
    return default_texture;
}


Texture Texture::badTexture()
{
    return Texture();
}

void Texture::cleanup()
{
    if(default_texture.isGood())
        default_texture.free();
}
    
}//namespace r64fx