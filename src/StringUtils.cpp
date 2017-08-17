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


std::string num2str(int num)
{
    if(num == 0)
        return "0";

    std::string str;
    char sign = 0;

    if(num < 0)
    {
        sign = '-';
        num *= -1;
    }

    while(num)
    {
        char ch = (num % 10) + 48;
        str = ch + str;
        num /= 10;
    }

    if(sign)
        str = sign + str;

    return str;
}


std::string full_path_to_uri(const std::string &full_path)
{
    std::string str = "file://";
    for(auto ch : full_path)
    {
        if(ch == ' ')
            str += "%20";
        else if(ch == '#')
            str += "%23";
        else
            str.push_back(ch);

    }
    return str;
}


std::string next_file_path_from_uri_list(std::string::iterator &begin_it, const std::string::iterator &end_it)
{
    enum class State{
        Initial,
        F, I, L, E, Colon, Slash, SlashSlash, SlashSlashSlash,
        Path,
        Percent, Percent2,
        CR, LF,
        Comment,
        Failed
    };

    State state = State::Initial;
    std::string result;

    auto it = begin_it;
    while(it != end_it)
    {
        char ch = *it;

        switch(state)
        {
            case State::Initial:
            {
                if(ch == 'f')
                {
                    state = State::F;
                    it++;
                }
                else if(ch == '#')
                {
                    state = State::Comment;
                    it++;
                }
                else
                    state = State::Failed;
                break;
            }

            case State::F:
            {
                if(ch == 'i')
                {
                    state = State::I;
                    it++;
                }
                else
                    state = State::Failed;
                break;
            }

            case State::I:
            {
                if(ch == 'l')
                {
                    state = State::L;
                    it++;
                }
                else
                    state = State::Failed;
                break;
            }

            case State::L:
            {
                if(ch == 'e')
                {
                    state = State::E;
                    it++;
                }
                else
                    state = State::Failed;
                break;
            }

            case State::E:
            {
                if(ch == ':')
                {
                    state = State::Colon;
                    it++;
                }
                else
                    state = State::Failed;
                break;
            }

            case State::Colon:
            {
                if(ch == '/')
                {
                    state = State::Slash;
                    it++;
                }
                else
                    state = State::Failed;
                break;
            }

            case State::Slash:
            {
                if(ch == '/')
                {
                    state = State::SlashSlash;
                    it++;
                }
                else
                    state = State::Failed;
                break;
            }

            case State::SlashSlash:
            {
                if(ch == '/')
                {
                    state = State::SlashSlashSlash;
                    it++;
                }
                else
                    state = State::Failed;
                break;
            }

            case State::SlashSlashSlash:
            {
                if(ch != '/' && ch != ':')
                {
                    result.push_back('/');
                    result.push_back(ch);
                    state = State::Path;
                    it++;
                }
                else
                    state = State::Failed;
                break;
            }

            case State::Path:
            {
                if(ch == '\r')
                    state = State::CR;
                else if(ch == '\n')
                    state = State::LF;
                else if(ch == '%')
                    state = State::Percent;
                else
                    result.push_back(ch);
                it++;
                break;
            }

            case State::Percent:
            {
                if(ch == '2')
                {
                    state = State::Percent2;
                    it++;
                }
                else
                    state = State::Path;
                break;
            }

            case State::Percent2:
            {
                if(ch == '0')
                {
                    result.push_back(' ');
                    it++;
                }
                else if(ch == '3')
                {
                    result.push_back('#');
                    it++;
                }
                state = State::Path;
                break;
            }

            case State::CR:
            {
                if(ch == '\n')
                {
                    state = State::LF;
                    it++;
                }
                else
                {
                    state = State::Failed;
                }
                break;
            }

            case State::LF:
            {
                state = State::Initial;
                break;
            }

            case State::Comment:
            {
                if(ch == '\n')
                    state = State::Initial;
                it++;
            }

            default:
                break;
        }

        if(state == State::LF)
            break;

        if(state == State::Failed)
        {
            result = "";
            break;
        }
    }

    begin_it = it;
    return result;
}

}//namespace r64fx
