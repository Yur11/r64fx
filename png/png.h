#ifndef R64FX_PNG_PNG_H
#define R64FX_PNG_PNG_H

#include <png.h>

namespace r64fx{
   
bool read_png(FILE* file, unsigned char* &data, int &nchannels, int &width, int &height);

bool write_png(FILE* file, unsigned char* data, int nchannels, int width, int height);
    
}//namespace r64fx

#endif//R64FX_PNG_PNG_H