#ifndef R64FX_WAVEFORM_UTILS_HPP
#define R64FX_WAVEFORM_UTILS_HPP

namespace r64fx{

void gen_waveform(
    float*  data,
    int     begin_frame,
    int     end_frame,
    int     component_count,
    int     component,
    float   gain,

    int pixel_count,
    unsigned char* out
);


void gen_waveform(
    float*  data,
    int     begin_frame,
    int     end_frame,
    int     component_count,
    int     component,
    float   gain,

    int pixel_count,
    unsigned short* out
);


void gen_waveform(
    float*  data,
    int     begin_frame,
    int     end_frame,
    int     component_count,
    int     component,
    float   gain,

    int pixel_count,
    unsigned int* out
);

void gen_waveform(
    float*  data,
    int     begin_frame,
    int     end_frame,
    int     component_count,
    int     component,
    float   gain,

    int pixel_count,
    float* out
);

}//namespace r64fx

#endif//R64FX_WAVEFORM_UTILS_HPP
