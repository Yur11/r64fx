#include "FrequencyPlot.hpp"
#include "ImageUtils.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

FrequencyPlot::FrequencyPlot(int w, int h)
{
    update(w, h);
}


FrequencyPlot::FrequencyPlot()
{

}


FrequencyPlot::~FrequencyPlot()
{

}


void FrequencyPlot::update(int w, int h)
{
    m_image.load(w, h, 1);
    m_rect = {10, 10, (w | 1) - 40, (h | 1) - 20};
    m_width_rcp = 1.0f / float(m_rect.width());

    fill(&m_image, Color(0));

    for(int x=0; x<m_rect.width(); x++)
    {
        if(!(x & 1))
        {
            int xx = x + m_rect.x();
            m_image(xx, m_rect.y())[0] = m_image(xx, m_rect.bottom() - 1)[0] = 255;
        }
    }

    float maxfreqlog = log10(maxFreq());
    m_min_freq_log = log10(minFreq());
    m_freq_range_log = maxfreqlog - m_min_freq_log;
    m_freq_range_log_rcp = 1.0f / m_freq_range_log;

    int n = 1;
    float f = 1.0f;
    float df = 1.0f;
    for(;;)
    {
        if(f >= minFreq())
        {
            if(f >= maxFreq())
                break;

            int x = (log10(f) - m_min_freq_log) * m_freq_range_log_rcp * m_rect.width();
            paintLineAt(x + m_rect.x(), n == 1 ? 127 : 63);
        }

        n++;
        f += df;
        if(n == 10)
        {
            n = 1;
            df *= 10;
        }
    }

    paintLineAt(m_rect.x(), 255);
    paintLineAt(m_rect.right()-1, 255);
}


float FrequencyPlot::freqAt(int x) const
{
    if(x < m_rect.x() || x >= m_rect.right())
        return 0.0f;
    return pow(10, float(x - m_rect.x()) * m_width_rcp * m_freq_range_log) + minFreq();
}


void FrequencyPlot::paintLineAt(int x, unsigned char value)
{
    for(int y=2; y<m_rect.height()-2; y++)
    {
        if(!(y & 1))
        {
            int yy = y + m_rect.y();
            m_image(x, yy)[0] = value;
        }
    }
}

}//namespace r64fx
