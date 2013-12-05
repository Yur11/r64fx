#include <iostream>
#include "shared_sources/read_png.h"

using namespace std;


int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        cerr << argv[0] << ": Give me an input file!\n";
        return 1;
    }
    
    auto file = fopen(argv[1], "r");
    if(!file)
    {
        cerr << argv[0] << ": Failed to open file \"" << argv[1] << "\" !\n";
        return 2;
    }
    
    
    unsigned char* data = nullptr;
    int nchannels = 0;
    int width = 0;
    int height = 0;
    if(!r64fx::read_png(file, data, nchannels, width, height))
    {
        cerr << "png2numlist: Error reading file \"" << argv[0] << "\" !\n";
        return 3;
    }
    
    cout << hex;
    
    cout << "/* Channel count. */\n";
    if(nchannels == 3)
        cout << 3;
    else if(nchannels == 4)
        cout << 4;
    else
    {
        cerr << "png2numlist: Supporting only 3 or 4 color channels\n!Got " << nchannels << " channels!\n";
        return 4;
        delete[] data;
        fclose(file);
    }
    cout << ",";
    
    union{
        int num;
        unsigned char bytes[4];
    } cast;
    
    cout << "\n/* Image width :: int */\n";
    cast.num = width;
    for(int i=0; i<4; i++)
        cout << "0x" << (unsigned int) cast.bytes[i] << ", ";
    
    cout << "\n/* Image height :: int */\n";
    cast.num = height;
    for(int i=0; i<4; i++)
        cout << "0x" << (unsigned int) cast.bytes[i] << ", ";
    
    cout << "\n/* Data. */\n";
    int n = 1;
    cout << "0x" << (unsigned int) data[0];
    for(int i=1; i<width*height*nchannels; i++)
    {
        n++;
        cout << ", ";
        if(n == 16)
        {
            n = 1;
            cout << "\n";
        }
        cout << "0x" << (unsigned int) data[i];
    }
    
    delete[] data;
    fclose(file);
    
    return 0;
}