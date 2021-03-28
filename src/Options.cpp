#define R64FX_OPTIONS_IMPL

#include "Options.hpp"

#include <iostream>
#include <string_view>

using namespace std;

namespace r64fx{

void show_help(char* argv0)
{
    cout << "Usage: " << argv0 << " [options]\n\n" <<
        "Options:\n"
        "    -h | --help      : Show this help message and exit.\n"
        "\n"
        "    --scale <float>  : UI scale\n"
        "\n"
    ;
}


int option_needs_value(string_view option)
{
    cerr << "Option '" << option << "' needs a value!\n";
    return -1;
}

int bad_option_argument(string_view opt, string_view val)
{
    cerr << "Bad argument '" << val  << "' for '" << opt << "' option!\n";
    return -1;
}


template<typename T> bool str2(string_view str, T &val)
{
    static_assert("Unsupported value type!");
    return false;
}

template<> bool str2<float>(string_view str, float &num)
{
    auto it = str.begin();

    float whole = 0.0f;
    while(it != str.end())
    {
        if(*it == '.')
            { it++; break; }

        if(*it < '0' || *it > '9')
            return false;

        whole *= 10.0f;
        whole += int(*it - '0');
        it++;
    }

    float frac = 0.0f, coeff = 1.0f;
    while(it != str.end())
    {
        if(*it < '0' || *it > '9')
            return false;

        coeff *= 0.1f;
        frac += int(*it - '0') * coeff;
        it++;
    }

    num = whole + frac;
    return true;
}


int Options::parse(int argc, char** argv)
{
    char* argv0 = argv[0];
    char** end = argv + argc;
    argv++;

    for(;;)
    {
        char* arg = *argv;
        if(argv >= end)
            break;

        if(arg[0] == '-')
        {
            if(arg[1] == '-')
            {
                if(arg[2] == '\0')
                    return 0; // Stop parsing after --

                string_view opt(arg+2);

                if(opt == "help")
                {
                    show_help(argv0);
                    return 1;
                }
                else if(opt == "scale")
                {
                    argv++;
                    if(argv == end || argv[0][0] == '-')
                        return option_needs_value(opt);
                    if(!str2<float>(argv[0], ui_scale) || ui_scale < 1.0f)
                        return bad_option_argument(opt, argv[0]);
                }
                else if(opt == "demo")
                {
                    argv++;
                    if(argv == end || argv[0][0] == '-')
                        return option_needs_value(opt);
                    demo = argv[0];
                }
                else
                {
                    cerr << "Bad option --" << opt << " !\n";
                    return -1;
                }
            }
            else
            {
                for(char* ch = arg+1; *ch != '\0'; ch++)
                {
                    if(*ch == 'h')
                    {
                        show_help(argv0);
                        return 1;
                    }
                    else
                    {
                        cerr << "Bad option -" << *ch << " !";
                        return -1;
                    }
                }
            }
        }

        argv++;
    }

    return 0;
}

}//namespace r64fx

#undef R64FX_OPTIONS_IMPL
