#ifndef R64FX_MAIN_AUDIO_DATA_H
#define R64FX_MAIN_AUDIO_DATA_H

namespace r64fx{
    
//
class AudioData{
    float* m_a = nullptr; //cubic
    float* m_b = nullptr; //square
    float* m_c = nullptr; //linear
    float* m_d = nullptr; //constant
    //Think a*(x**3) + b*(x**2) + c*x + d
    
    long unsigned int m_size = 0;
    
    unsigned int m_channels = 0;
    
    unsigned int m_samplerate = 0;
    
    float m_samplerate_rcp = 0.0;
    
public:
    AudioData(long unsigned int size = 0, float* data = nullptr, float nchannels = 1, float samplerate = 48000);
    
    /** @brief Load audio data from file. */
    AudioData(const char* path);
    
    ~AudioData();
    
    /** @brief The number of floating point samples in one of the buffers. */
    inline long unsigned int size() const { return m_size; }
    
    inline float* constantData() const { return m_d; }
    
    inline float* linearData() const { return m_c; }
    
    inline float* squareData() const { return m_b; }
    
    inline float* cubicData() const { return m_a; }

    inline unsigned int nchannels() const { return m_channels; }
    
    inline unsigned int samplerate() const { return m_samplerate; }
    
    /** @brief Calculate linear interpolation slope coeff. from the constantData() for tightly packed single channel data. */
    void calculateLinear();
    
    /** @brief Calculate linear interpolation slope coeff. from the constantData() for interleaved multichannel data. */
    void calculateLinearInterleaved();
    
    float readNearest(float t);
    
    float readLinear(float t);
};

void calculate_peak_summary(float* input, int input_size, float* output, int output_size);
    
}//namespace r64fx

#endif//R64FX_MAIN_AUDIO_DATA_H