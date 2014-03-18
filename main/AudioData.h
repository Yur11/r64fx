#ifndef R64FX_MAIN_AUDIO_DATA_H
#define R64FX_MAIN_AUDIO_DATA_H

namespace r64fx{
    
//
class AudioData{
    float* _a = nullptr; //cubic
    float* _b = nullptr; //square
    float* _c = nullptr; //linear
    float* _d = nullptr; //constant
    //Think a*(x**3) + b*(x**2) + c*x + d
    
    long unsigned int _size = 0;
    
    unsigned int _channels = 0;
    
    unsigned int _samplerate = 0;
    
    float _samplerate_rcp = 0.0;
    
public:
    AudioData(long unsigned int size = 0, float* data = nullptr, float nchannels = 1, float samplerate = 48000);
    
    /** @brief Load audio data from file. */
    AudioData(const char* path);
    
    virtual ~AudioData();
    
    /** @brief The number of floating point samples in one of the buffers. */
    inline long unsigned int size() const { return _size; }
    
    inline float* constantData() const { return _d; }
    
    inline float* linearData() const { return _c; }
    
    inline float* squareData() const { return _b; }
    
    inline float* cubicData() const { return _a; }

    inline unsigned int nchannels() const { return _channels; }
    
    inline unsigned int samplerate() const { return _samplerate; }
    
    /** @brief Calculate linear interpolation slope coeff. from the constantData() for tightly packed single channel data. */
    void calculateLinear();
    
    /** @brief Calculate linear interpolation slope coeff. from the constantData() for interleaved multichannel data. */
    void calculateLinearInterleaved();
    
    float readNearest(float t);
    
    float readLinear(float t);
    
    void calculateSummary(float* buffer, int nsamples);
    
    void calculateBipolarSummary(float* buffer, int nsamples);
};
    
}//namespace r64fx

#endif//R64FX_MAIN_AUDIO_DATA_H