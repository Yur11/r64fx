#ifndef R64FX_OPTIONS_HPP
#define R64FX_OPTIONS_HPP

#include <string>

namespace r64fx{

struct Options{
    int parse(int argc, char** argv);

    float ui_scale = 1.0f;
    inline int UiScaleUp(int n) const { return n * ui_scale; }
};

#ifndef R64FX_OPTIONS_IMPL
extern
#endif//R64FX_OPTIONS_IMPL
Options g_options;

}//namespace r64fx

#endif//R64FX_OPTIONS_HPP
