#ifndef R64FX_AUDIO_ENGINE_H
#define R64FX_AUDIO_ENGINE_H

#include <vector>


namespace r64fx{
   
struct AudioEnginePrivate;
    
/** @brief Base class for audio engines. 
 
    AudioEngine does all the audio backend management.
    Make a subclass for each specific audio backend: Jack, PortAudio etc...
 */
class AudioEngine{
    std::vector<float*> input_buffers;
    std::vector<float*> output_buffers;
    
protected:
    int sample_rate;
    int buffer_size;
    bool is_valid = false;//Must be set by the backend after all inializaton is done.
        
    AudioEngine();
    ~AudioEngine();
    
    void createInputBuffer();
    
    void createOutputBuffer();
    
    void dropInputBuffers();
    
    void dropOutputBuffers();
    
    float* inputBuffer(int n);
    
    float* outputBuffer(int n);
    
public:    
    int sampleRate() const { return sample_rate; }
    
    int bufferSize() const { return buffer_size; }
    
    bool isValid() const { return is_valid; }
    
    /** @brief Get the n'th input buffer. */
    float* inputBuffer(int n) const;
    
    int inputBufferCount() const;
    
    /** @brief Get the n'th output_buffer */
    float* outputBuffer(int n) const;
    
    int outputBufferCount() const;
    
    virtual void activate() = 0;
    
    virtual void deactivate() = 0;
};
    
}//namespace r64fx

#endif//R64FX_AUDIO_ENGINE_H