#include "AudioData.hpp"
#include <sndfile.h>
#include <cmath>

#include <iostream>
#include <assert.h>

using namespace std;

namespace r64fx{
    
AudioData::AudioData(long unsigned int size, float* data, float nchannels, float samplerate)
{
    if(size == 0)
        return;
    
    m_size = size;
    
    if(data == nullptr)
        m_d = new float[size];
    else
        m_d = data;
    
    m_channels = nchannels;
    m_samplerate = samplerate;
    m_samplerate_rcp = 1.0 / float(m_samplerate);
}


AudioData::AudioData(const char* path)
{
    SF_INFO info;
    info.format = 0;
    SNDFILE* sndfile = sf_open(path, SFM_READ, &info);
    if(!sndfile)
    {
        cerr << "Failed to read audio file \"" << path << "\"!\n";
        cerr << sf_strerror(nullptr) << "\n";
        return;
    }

    m_channels = info.channels;
    m_samplerate = info.samplerate;
    m_samplerate_rcp = 1.0 / float(m_samplerate);

    m_size = info.frames * m_channels;
    
    m_d = new float[m_size];
    assert(sf_readf_float(sndfile, m_d, info.frames) == (int)m_size);
    
    sf_close(sndfile);
}


AudioData::~AudioData()
{
    if(m_a)
        delete[] m_a;
    
    if(m_b)
        delete[] m_b;
    
    if(m_c)
        delete[] m_c;
    
    if(m_d)
        delete[] m_d;
}


void AudioData::calculateLinear()
{
    m_c = new float[size()];
    
    cout << fixed;
    float dx = m_samplerate_rcp;
    for(int i=0; i<(int)size()-1; i++)
    {
        float dy = m_d[i+1] - m_d[i];
        m_c[i] = dy / dx;
    }
    m_c[size()-1] = float(m_d[0] - m_d[size()-1]) / dx;
}


float AudioData::readNearest(float t)
{
    t *= float(m_samplerate);
    int n = int(t);
    return m_d[n];
}


float AudioData::readLinear(float t)
{
    t *= float(m_samplerate);
    int n = int(t);
    float x = t - float(n);
    float d = m_d[n];
    float c = m_c[n];
    return c*x*m_samplerate_rcp + d;
}


void calculate_peak_summary(float* input, int input_size, float* output, int output_size)
{
    int chunk_size = (input_size / output_size);
    output_size /= 2;

    for(int i=0; i<output_size; i++)
    {
        float min = +1.0f;
        float max = -1.0f;
        for(int j=0; j<chunk_size; j++)
        {
            int n = i*chunk_size + j;
            float val = input[n];

            if(val >= 1.0f)
            {
                val = 1.0f;
                break;
            }
            else if(val <= -1.0f)
            {
                val = -1.0f;
                break;
            }

            if(val > max)
                max = val;
            if(val < min)
                min = val;
        }
        output[i*2    ] = min * 0.5f + 0.5;
        output[i*2 + 1] = max * 0.5f + 0.5;
    }
}
    
}//namespace r64fx