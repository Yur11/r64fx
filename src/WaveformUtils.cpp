#include "WaveformUtils.hpp"
#include <limits>
#include <algorithm>

namespace r64fx{

template<typename T> T normalize(float num)
{
    return T(float(std::numeric_limits<T>::max()) * num);
}

template<> float normalize(float num)
{
    return num;
}

    
template<typename T> void gen_waveform(
    float*  data, 
    int     begin_frame,
    int     end_frame,
    int     component_count, 
    int     component,
    float   gain,
    
    int pixel_count,
    T* out
)
{
    int range = end_frame - begin_frame;
    int frames_per_pixel = range / pixel_count;
    int base = begin_frame * component_count;
    
    for(int p=0; p<pixel_count; p++)
    {
        float min_value = 0;
        float max_value = 0;
        for(int f=0; f<frames_per_pixel; f++)
        {
            int ff = p * frames_per_pixel + f;
            float value = data[base + ff * component_count + component];
            if(value > max_value)
                max_value = value;
            if(value < min_value)
                min_value = value;
        }
        
        min_value *= gain;
        max_value *= gain;
        min_value = std::max(min_value, -1.0f);
        max_value = std::min(max_value, 1.0f);
        min_value = min_value * 0.5f + 0.5f;
        max_value = max_value * 0.5f + 0.5f;
        
        out[p*2]     = normalize<T>(min_value);
        out[p*2 + 1] = normalize<T>(max_value);
    }
}
    
    
void gen_waveform(
    float*  data, 
    int     begin_frame,
    int     end_frame,
    int     component_count, 
    int     component,
    float   gain,
    
    int pixel_count,
    unsigned char* out
)
{
    return gen_waveform<unsigned char>(data, begin_frame, end_frame, component_count, component, gain, pixel_count, out);
}


void gen_waveform(
    float*  data, 
    int     begin_frame,
    int     end_frame,
    int     component_count, 
    int     component,
    float   gain,
    
    int pixel_count,
    unsigned short* out
)
{
    return gen_waveform<unsigned short>(data, begin_frame, end_frame, component_count, component, gain, pixel_count, out);
}


void gen_waveform(
    float*  data, 
    int     begin_frame,
    int     end_frame,
    int     component_count, 
    int     component,
    float   gain,
    
    int pixel_count,
    unsigned int* out
)
{
    return gen_waveform<unsigned int>(data, begin_frame, end_frame, component_count, component, gain, pixel_count, out);
}

void gen_waveform(
    float*  data, 
    int     begin_frame,
    int     end_frame,
    int     component_count, 
    int     component,
    float   gain,
    
    int pixel_count,
    float* out
)
{
    return gen_waveform<float>(data, begin_frame, end_frame, component_count, component, gain, pixel_count, out);
}

}//namespace r64fx
