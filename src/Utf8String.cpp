#include "Utf8String.hpp"

namespace r64fx{

std::string Utf8String::operator[](unsigned int index)
{
    int byte = bytes_up_to(index);
    int nbytes = next_utf8_char(byte) - byte;
    return std::string(stdstr, byte, nbytes);
}


int Utf8String::next_utf8_char(int index)
{
    char c = stdstr[index];
    if(!(c & 128))//ascii byte
    {
        index++;
    }
    else
    {
        //Count leading 1s.
        int leading_one_count = 1;//We know that we have at least one leading 1 by now.
        for(int i=6; i>=0; i--)
        {
            if(!(c & (1<<i))) break;
            leading_one_count++;
        }

        /* Invalid utf-8 */
        if(leading_one_count < 2)  return -1;
        if(index + leading_one_count > (int)stdstr.size()) return -1;

        
        index++;
        
        /* Check the validity of each of the continuation bytes. */
        int continuation_byte_count = leading_one_count-1;
        for(int i=0; i<continuation_byte_count; i++)
        {
            c = stdstr[index];
            if((c & (1<<7)) && ((~c) & (1<<6))) //must be 10xxxxxx
                index++;
            else               
                return -1;
        }
    }
    
    return index;
}


int Utf8String::size()
{   
    int result = 0;
    int i=0;
    while(i != (int)stdstr.size())
    {
        i = next_utf8_char(i);
        if(i == -1) return -1;
        result++;
    }
    
    return result;
}



int Utf8String::bytes_from_to(int first_byte, int ncharacters)
{
    int byte = first_byte;
    int n = 0;
    while(n++ < ncharacters)
    { 
        byte = next_utf8_char(byte); 
        if(byte == -1) return -1;
    }
    return byte;
}


void Utf8String::insert(int position, Utf8String other)
{
    int byte = bytes_up_to(position);
    stdstr.insert(byte, other.stdstr);
}


void Utf8String::insert(int position, int n, std::string ch)
{
    Utf8String tmp;
    while(n--) tmp.stdstr += ch;
    insert(position, tmp);
}

void Utf8String::erase(int position, int n)
{
    int begin_byte = bytes_up_to(position);
    int end_byte = bytes_from_to(begin_byte, n);
    stdstr.erase(begin_byte, end_byte - begin_byte);
}

}//namespace r64fx