#ifndef R64FX_JACK_AUDIO_ENGINE_H
#define R64FX_JACK_AUDIO_ENGINE_H

#include "AudioEngine.h"

namespace r64fx{

struct JackAudioEnginePrivate;
    
class JackAudioEngine : public AudioEngine{
    friend struct JackAudioEnginePrivate;
    JackAudioEnginePrivate* p;
    
    inline void setSampleRate(int new_rate) { sample_rate = new_rate; }
    
    inline void setBufferSize(int new_size) { buffer_size = new_size; }
    
public:
    JackAudioEngine(const char* client_name);
    ~JackAudioEngine();
    
    int createInputPort(const char* name);
    
    int createOutputPort(const char* name);
    
    virtual void activate();
    
    virtual void deactivate();
};
    
}//namespace r64fx

#endif//R64FX_JACK_AUDIO_ENGINE_H
