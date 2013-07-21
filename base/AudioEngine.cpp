#include "AudioEngine.h"
#include <pthread.h>

using namespace std;

namespace r64fx{
    

AudioEngine::AudioEngine()
{
}


AudioEngine::~AudioEngine()
{
}


void AudioEngine::createInputBuffer()
{
    input_buffers.push_back( new float[bufferSize()] );
}


void AudioEngine::createOutputBuffer()
{
    input_buffers.push_back( new float[bufferSize()] );
}


void AudioEngine::dropInputBuffers()
{
    auto &buffers = input_buffers;
    while(buffers.empty())
    {
        delete buffers.back();
        buffers.pop_back();
    }
}

    
void AudioEngine::dropOutputBuffers()
{
    auto &buffers = output_buffers;
    while(buffers.empty())
    {
        delete buffers.back();
        buffers.pop_back();
    }
}


float* AudioEngine::inputBuffer(int n)
{
    return input_buffers[n];
}

    
float* AudioEngine::outputBuffer(int n)
{
    return output_buffers[n];
}


float* AudioEngine::inputBuffer(int n) const
{
    return input_buffers[n];
}


int AudioEngine::inputBufferCount() const
{
    return input_buffers.size();
}


float* AudioEngine::outputBuffer(int n) const
{
    return output_buffers[n];
}


int AudioEngine::outputBufferCount() const
{
    return output_buffers.size();
}
    
}//namespace r64fx
