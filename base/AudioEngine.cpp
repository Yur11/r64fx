#include "AudioEngine.h"
#include <vector>
#include <pthread.h>

using namespace std;

namespace r64fx{

struct AudioEnginePrivate{
    vector<float*> input_buffers;
    vector<float*> output_buffers;
    
};
    

AudioEngine::AudioEngine()
{
    p = new AudioEnginePrivate;
}


AudioEngine::~AudioEngine()
{
    delete p;
}


void AudioEngine::createInputBuffer()
{
    p->input_buffers.push_back( new float[bufferSize()] );
}


void AudioEngine::createOutputBuffer()
{
    p->input_buffers.push_back( new float[bufferSize()] );
}


void AudioEngine::dropInputBuffers()
{
    auto &buffers = p->input_buffers;
    while(buffers.empty())
    {
        delete buffers.back();
        buffers.pop_back();
    }
}

    
void AudioEngine::dropOutputBuffers()
{
    auto &buffers = p->output_buffers;
    while(buffers.empty())
    {
        delete buffers.back();
        buffers.pop_back();
    }
}


float* AudioEngine::inputBuffer(int n)
{
    return p->input_buffers[n];
}

    
float* AudioEngine::outputBuffer(int n)
{
    return p->output_buffers[n];
}


float* AudioEngine::inputBuffer(int n) const
{
    return p->input_buffers[n];
}


int AudioEngine::inputBufferCount() const
{
    return p->input_buffers.size();
}


float* AudioEngine::outputBuffer(int n) const
{
    return p->output_buffers[n];
}


int AudioEngine::outputBufferCount() const
{
    return p->output_buffers.size();
}
    
}//namespace r64fx
