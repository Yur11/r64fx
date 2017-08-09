#ifndef R64FX_FREQUENCY_PLOT_HPP
#define R64FX_FREQUENCY_PLOT_HPP

#include "Image.hpp"
#include "GeometryUtils.hpp"


namespace r64fx{

class FrequencyPlot{
    Image m_image;
    float m_min_freq            = 1.0f;
    float m_max_freq            = 1.0f;
    float m_min_freq_log        = 1.0f;
    float m_freq_range_log      = 1.0f;
    float m_freq_range_log_rcp  = 1.0f;
    Rect<int> m_rect;

public:
    FrequencyPlot(int w, int h);

    FrequencyPlot();

    ~FrequencyPlot();

    inline Image* image()
    {
        return &m_image;
    }

    inline bool isGood() const
    {
        return m_image.isGood();
    }

    inline void setMinFreq(float freq)
    {
        m_min_freq = freq;
    }

    inline float minFreq() const
    {
        return m_min_freq;
    }

    inline void setMaxFreq(float freq)
    {
        m_max_freq = freq;
    }

    inline float maxFreq() const
    {
        return m_max_freq;
    }

    inline Rect<int> rect() const
    {
        return m_rect;
    }

    void update(int w, int h);

private:
    void paintLineAt(int x, unsigned char value);
};

}//namespace r64fx

#endif//R64FX_FREQUENCY_PLOT_HPP
