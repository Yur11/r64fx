#include "Texture.h"
#include <shared_sources/read_png.h>
#include <vector>
#include <map>
#include <algorithm>
#include <stdlib.h>
#include <cmath>

#ifdef DEBUG
#include <assert.h>
#endif//DEBUG

using namespace std;

namespace r64fx{

    
static map<string, Texture*> common_textures;
    
    
Texture::Kind Texture1D::kind()
{
    return Texture::Kind::Tex1D;
}


Texture::Kind Texture2D::kind()
{
    return Texture::Kind::Tex2D;
}
    
    
Texture::~Texture()
{
    gl::DeleteTextures(1, &_gl_name);
}


void Texture::addCommonTexture(std::string name, Texture* tex)
{
    common_textures[name] = tex;
}


Texture* Texture::find(std::string name)
{
    auto it = common_textures.find(name);
    if(it == common_textures.end())
        return nullptr;
    else
        return it->second;
}


void Texture::freeCommonTextures()
{
    for(auto p : common_textures)
    {
        delete p.second;
    }
    
    common_textures.clear();
}


Texture1D::Texture1D(GLuint gl_name, int nchannels, int nlevels, GLsizei w)
: Texture(gl_name), _nchannels(nchannels), _nlevels(nlevels), _w(w)
{
}
    
    
Texture2D* Texture2D::loadBaseLevel(string path, GLenum internal_format, GLenum format, unsigned int flags)
{
    auto file = fopen(path.c_str(), "rb");
    if(!file)
    {
        cerr << "Texture2D failed to open file " << path << " !\n";
        return nullptr;
    }
    
    unsigned char* data; 
    int nchannels; 
    int width; 
    int height;
    if(!read_png(file, data, nchannels, width, height))
    {
        cerr << "Texture2D failed to read png file " << path << " !\n";
        return nullptr;
    }
    fclose(file);

    int nlevels = flags & Texture::AllocMipmaps ? log2(max(width, height)) + 1 : 1;
    
    GLuint name;
    gl::GenTextures(1, &name);
    gl::BindTexture(GL_TEXTURE_2D, name);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl::TexStorage2D(GL_TEXTURE_2D, nlevels, internal_format, width, height);
    gl::TexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
    
    free(data);
    
    return new Texture2D(name, nchannels, nlevels, width, height);
}


Texture2D* Texture2D::loadMipmaps(string dir_path, GLenum internal_format, GLenum format)
{
    if(dir_path.back() != '/')
        dir_path.push_back('/');
    
    auto tex = loadBaseLevel((dir_path + "0.png").c_str(), internal_format, format);
    if(!tex)
        return nullptr;
    
    int expected_width = tex->width(); 
    int expected_height = tex->height();
    
    for(int i=1; i<tex->nlevels(); i++)
    {
        if(expected_width > 1)
            expected_width >>= 1;
        
        if(expected_height)
            expected_height >>= 1;
        
        char buff[8];
        auto nchars = sprintf(buff, "%d", i);
        string pp = dir_path + string(buff, nchars) + ".png";
        
        auto file = fopen(pp.c_str(), "rb");
        if(!file)
        {
            cerr << "Texture2D failed to open file " << pp << "!\n";
            continue;
        }

        unsigned char* data; 
        int nchannels;
        int width;
        int height;
        
        if(!read_png(file, data, nchannels, width, height))
        {
            cerr << "Texture2D failed to read png file " << dir_path << " !\nignoring\n";
            continue;
        }
        fclose(file);
        
        if(width != expected_width || height != expected_height)
        {
            free(data);
            cerr << "Bad mipmap size " << width << 'x' << height << " for " << pp << "!\nignoring\n";
            continue;
        }
        
        if(nchannels != tex->nchannels())
        {
            free(data);
            cerr << "Bad mipmap format " << nchannels << " for " << pp << "!\nignoring\n";
            continue;
        }
        
        gl::TexSubImage2D(GL_TEXTURE_2D, i, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
        
        free(data);
    }
        
    return tex;
}
    
}//namespace r64fx