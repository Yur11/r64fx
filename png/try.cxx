#include <iostream>
#include "png.h"
#include "Image.h"
#include <cmath>

using namespace std;
using namespace r64fx;


float distance_between(float x1, float y1, float x2, float y2)
{
    float dx = x1 - x2;
    float dy = y1 - y2;
    return sqrt(dx*dx + dy*dy);
}

float angle_of(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    return atan(dy / dx);
}

int main(int argc, char* argv[])
{    
    if(argc < 2)
    {
        cerr << "Give me a file name!\n";
        return -1;
    }

    FILE* file = fopen(argv[1], "w+");
    if(!file)
    {
        cerr << "Failed to create file!\n";
        return -2;
    }
    
    Image<Pixel<unsigned char, 4>> img(100, 100);
    for(unsigned int y=0; y<img.height(); y++)
    {
        for(unsigned int x=0; x<img.width(); x++)
        {
            auto d = distance_between(50, 50, x, y);
            unsigned char val = (angle_of(50, 50, x, y) + M_PI) * 255 / (M_PI*4);
            
            if(d > 40 || d < 20)
                val = 0;
            
            img(x, y)[0] = val;
            img(x, y)[1] = val;
            img(x, y)[2] = val;
            img(x, y)[3] = 255;
        }
    }
    
//     for(unsigned int c=0; c<img.componentCount(); c++)
//     {
//         for(unsigned int y=1; y<img.height()-1; y++)
//         {
//             for(unsigned int x=1; x<img.width()-1; x++)
//             {
//                 double val = 0.0;
//                 val += img(x-1, y-1)[c];
//                 val += img(x-1, y  )[c];
//                 val += img(x-1, y+1)[c];
//                 val += img(x,   y-1)[c];
//                 val += img(x,   y  )[c];
//                 val += img(x,   y+1)[c];
//                 val += img(x+1, y-1)[c];
//                 val += img(x+1, y  )[c];
//                 val += img(x+1, y+1)[c];
//                 val /= 9;
//                 img(x, y)[c] = val;
//             }
//         }
//     }
    
    write_png(file, (unsigned char*)img.data(), img.componentCount(), img.width(), img.height());
    
    img.free();
    fclose(file);
    
    return 0;
}