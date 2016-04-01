#ifndef R64FX_SIGNAL_DATA_HPP
#define R64FX_SIGNAL_DATA_HPP

namespace r64fx{

class SignalData{
    float* m_data            = nullptr;
    int    m_frame_count     = 0;
    int    m_component_count = 0;
    int    m_flags           = 0;
    int    m_sample_rate     = 0.0f;

public:
    SignalData(int frame_count = 0, int channel_count = 1, float* data = nullptr, bool copy_data = false);

    ~SignalData();

    void load(int frame_count = 0, int channel_count = 1, float* data = nullptr, bool copy_data = false);

    void free();

    bool ownsData() const;

    float* data() const;

    int frameCount() const;

    int componentCount() const;

    float* frame(int i) const;

    void setSampleRate(int rate);

    int sampleRate() const;
};

}//namespace r64fx

#endif//R64FX_SIGNAL_DATA_HPP