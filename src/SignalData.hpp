#ifndef R64FX_SIGNAL_DATA_HPP
#define R64FX_SIGNAL_DATA_HPP

namespace r64fx{

class SignalData{
    void*          m_data        = nullptr;
    int            m_size        = 0;
    int            m_chan_count  = 0;

public:
    SignalData();

    SignalData(int size, int channel_count);

    ~SignalData();

    void alloc(int size, int channel_count);

    void free();

    int size() const;

    int channelCount() const;

    float* data(int chan = 0) const;
};

}//namespace r64fx

#endif//R64FX_SIGNAL_DATA_HPP