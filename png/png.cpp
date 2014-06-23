#include "png.h"
#include <iostream>

using namespace std;

namespace r64fx{
    
bool read_png(FILE* file, unsigned char* &data, int &nchannels, int &width, int &height)
{
    using namespace std;
    
    char header[8];
    if(fread(header, 1, 8, file) < 8)
    {
        cerr << "read_png: Failed to read file header!\n";
        return false;
    }
    
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if(!png_ptr)
    {
        cerr << "read_png: Failed to create png read structure!\n";
        return false;
    }
    
    png_infop png_info = png_create_info_struct(png_ptr);
    if(!png_info)
    {
        cerr << "read_png: Failed to create png info structure!\n";
        return false;
    }
    
    if(setjmp(png_jmpbuf(png_ptr)))
    {
        cerr << "read_png: Error during init_io!\n";
        return false;
    }
    
    png_init_io(png_ptr, file);
    png_set_sig_bytes(png_ptr, 8);
    
    png_read_info(png_ptr, png_info);
    
    nchannels = png_get_channels(png_ptr, png_info);
    
    width = png_get_image_width(png_ptr, png_info);
    height = png_get_image_height(png_ptr, png_info);
    
    
    png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, png_info);
    
    data = new png_byte[height * png_get_rowbytes(png_ptr, png_info)];
    png_bytep* row_pointers = new png_bytep[height];
    for(int i=0; i<(int)height; i++)
    {
        row_pointers[i] = data + i * png_get_rowbytes(png_ptr, png_info);
    }
    
    png_read_image(png_ptr, row_pointers);
    
    delete[] row_pointers;
    
    return true;
}


bool write_png(FILE* file, unsigned char* data, int nchannels, int width, int height)
{
    if(nchannels < 1 || nchannels > 4)
    {
        cerr << "write_png: Bad channel count: " << nchannels << " !\n";
        return false;
    }
    
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png_ptr)
    {
        cerr << "write_png: Failed to create png write structure!\n";
        return false;
    }
    
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr)
    {
        cerr << "write_png: Failed to create png info structure!\n";
        return false;
    }
    
    if(setjmp(png_jmpbuf(png_ptr)))
    {
        cerr << "write_png: Error during init_io!\n";
        return false;
    }
    
    png_init_io(png_ptr, file);
    
    if(setjmp(png_jmpbuf(png_ptr)))
    {
        cerr << "write_png: Error during writing header!\n";
        return false;
    }

    static int color_type[5] = {
        0,
        PNG_COLOR_TYPE_GRAY,
        PNG_COLOR_TYPE_GRAY_ALPHA,
        PNG_COLOR_TYPE_RGB,
        PNG_COLOR_TYPE_RGB_ALPHA
    };
        
    png_set_IHDR(
        png_ptr, info_ptr, width, height,
        8, color_type[nchannels], PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE
    );
    
    png_write_info(png_ptr, info_ptr);

    if(setjmp(png_jmpbuf(png_ptr)))
    {
        cerr << "write_png: Error during writing bytes!\n";
        return false;
    }
    
    
    if(setjmp(png_jmpbuf(png_ptr)))
    {
        cerr << "write_png: Error during end of write!\n";
        return false;
    }
    
    for(int i=0; i<height; i++)
    {
        png_write_row(png_ptr, data + i * width * nchannels);
    }

    png_write_end(png_ptr, NULL);
    
    
    return true;
}
    
}//namespace r64fx
