#include "AudioData.h"
#include <sndfile.h>

#ifdef DEBUG
#include <assert.h>
#include <iostream>
using namespace std;
#endif//DEBUG

namespace r64fx{
    
AudioData::AudioData(long unsigned int size, float* data, float nchannels, float samplerate)
{
    if(size == 0)
        return;
    
    _size = size;
    
    if(data == nullptr)
        _d = new float[size];
    else
        _d = data;
    
    _channels = nchannels;
    _samplerate = samplerate;
    _samplerate_rcp = 1.0 / float(_samplerate);
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

//     cout << info.channels << " channels\n";
//     cout << info.samplerate << " HZ\n";
//     cout << info.frames << " frames\n";
    
    assert(info.samplerate == 48000);
    assert(info.channels == 1);
    
    _channels = info.channels;
    _samplerate = info.samplerate;
    _samplerate_rcp = 1.0 / float(_samplerate);
    
    _size = info.frames * _channels;
    
    _d = new float[_size];
    assert(sf_readf_float(sndfile, _d, info.frames) == (int)_size);
    
    sf_close(sndfile);
}


AudioData::~AudioData()
{
    if(_a)
        delete[] _a;
    
    if(_b)
        delete[] _b;
    
    if(_c)
        delete[] _c;
    
    if(_d)
        delete[] _d;
}


void AudioData::calculateLinear()
{
    _c = new float[size()];
    
    cout << fixed;
    float dx = _samplerate_rcp;
    for(int i=0; i<(int)size()-1; i++)
    {
        float dy = _d[i+1] - _d[i];
        _c[i] = dy / dx;
//         cout << "dy: " << dy << "\n";
//         cout << "dx: " << dx << "\n";
//         cout << _c[i] << "\n";
    }
    _c[size()-1] = float(_d[0] - _d[size()-1]) / dx;
}


float AudioData::readNearest(float t)
{
    t *= float(_samplerate);
    int n = int(t);
    return _d[n];
}


float AudioData::readLinear(float t)
{
    t *= float(_samplerate);
    int n = int(t);
    float x = t - float(n);
    float d = _d[n];
    float c = _c[n];
    return c*x*_samplerate_rcp + d; 
}
    
}//namespace r64fx