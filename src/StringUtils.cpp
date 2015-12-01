#include "StringUtils.hpp"

namespace r64fx{

    
bool is_ascii(unsigned char byte)
{
    return (byte & 128) == 0;
}


bool is_utf8_secodary(unsigned char byte)
{
    return (byte & (128 + 64)) == 128;
}
    
    
int utf8_byte_count(unsigned char first_byte)
{
    int i=5;
    while(i)
    {
        unsigned char mask = (1<<i);
        if((first_byte & mask)==0)
            return 7 - i;
        i--;
    }
    
    return 0;
}
    

int next_utf8(const std::string &str, int pos)
{
    if(pos >= (int)str.size())
        return -1;
        
    if(is_ascii(str[pos]))
        return 1;
    
    if(is_utf8_secodary(str[pos]))
        return -2;
    
    int byte_count = utf8_byte_count(str[pos]);
    if(byte_count > 0)
    {
        for(int i=1; i<byte_count; i++)
        {
            if(!is_utf8_secodary(str[pos+i]))
                return -3;
        }
        return byte_count;
    }

    return -4;
}


int to_utf32(const std::string &str, int pos, int size)
{
    int result = 0;

    if(size == 1)
    {
        result = (int(str[pos]) & 127);
    }
    else if(size > 1)
    {
        result = (int(str[pos]) & ((1<<(7-size)) - 1));
        
        for(int i=1; i<size; i++)
        {
            result <<= 6;
            result |= (int(str[pos+i]) & 63);
        }
    }
    
    return result;
}


namespace
{
    const std::string hex = "0123456789ABCDEF";
}

std::string hexstr(unsigned char byte)
{
    std::string str;
    str.push_back(hex[byte >> 4]);
    str.push_back(hex[byte & 15]);
    return str;
}

std::string hexstr(unsigned short word)
{
    return hexstr((unsigned char)(word >> 8)) + hexstr((unsigned char)(word & 255));
}

std::string hexstr(unsigned int dword)
{
    return hexstr((unsigned short)(dword >> 16)) + hexstr((unsigned short)(dword & 65535));
}

std::string hexstr(int dword)
{
    std::string str;
    if(dword < 0)
        str.push_back('-');
    unsigned int num = dword & 0x7FFFFFFF;
    str += hexstr(num);
    return str;
}

}//namespace r64fx