#include "JackAudioEngine.h"
#include <jack/jack.h>
#include <iostream>
#include <vector>
#include <cstring>

#ifdef DEBUG
#include <string>
#include <assert.h>
#endif//DEBUG

using namespace std;

namespace r64fx{
    
int on_audio_io(jack_nframes_t nframes, void* arg);
int on_sample_rate_change(jack_nframes_t new_rate, void* arg);
int on_buffer_size_change(jack_nframes_t new_size, void *arg);
void on_shutdown(void* arg);
void on_error(const char* str);
    
#ifdef DEBUG
int on_debug_audio_player_io(jack_nframes_t nframes, void* arg);
#endif//DEBUG

struct JackAudioEnginePrivate{
    JackAudioEngine* public_inteface;
    jack_client_t* client;
    vector<jack_port_t*> input_ports;
    vector<jack_port_t*> output_ports;

#ifdef DEBUG
    vector<AudioData> debug_player_data_channels;
    vector<int> debug_player_indices;
#endif//DEBUG
    
    inline bool init(JackAudioEngine* engine, const char* client_name)
    {
        public_inteface = engine;
        
        jack_set_error_function(on_error);
        
        client = jack_client_open(client_name, JackNullOption, nullptr);
        if(!client)
        {
            cerr << "Failed to create jack client!\n";
            return false;
        }
        
        jack_set_process_callback(client, on_audio_io, this);
        jack_set_sample_rate_callback(client, on_sample_rate_change, this);
        jack_on_shutdown(client, on_shutdown, this);
        
        return true;
    }
    
    inline void do_audio_io(unsigned int sample_count)
    {
        /* Sync with processing threads here! */
        
        /* Do IO. */
        for(int i=0; i<(int)input_ports.size(); i++)
        {
           float* port = (float*) jack_port_get_buffer(input_ports[i], sample_count);
           memcpy(input_buffer(i), port, sizeof(float) * sample_count);
        }
        
        for(int i=0; i<(int)output_ports.size(); i++)
        {
           float* port = (float*) jack_port_get_buffer(output_ports[i], sample_count);
           memcpy(port, output_buffer(i), sizeof(float) * sample_count);
        }
    }
    
    
    inline int create_new_input_port(const char* name)
    {
        auto port_count = input_ports.size();
        input_ports.push_back( jack_port_register(client, name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0) );
        return port_count;
    }
    
    inline int create_new_output_port(const char* name)
    {
        auto port_count = output_ports.size();
        output_ports.push_back( jack_port_register(client, name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0) );
        return port_count;
    }
    
    
    inline float* input_buffer(int n)
    {
        return public_inteface->inputBuffer(n);
    }
    
    
    inline float* output_buffer(int n)
    {
        return public_inteface->outputBuffer(n);
    }
    
    
    inline void set_sample_rate(int new_rate)
    {
        public_inteface->setSampleRate(new_rate);
    }
    
    inline void set_buffer_size(int new_size)
    {
        public_inteface->setBufferSize(new_size);
    }

    inline void activate()
    {
        jack_activate(client);
    }
    
    inline void deactivate()
    {
        jack_deactivate(client);
    }
    
    inline void cleanup()
    {
        jack_client_close(client);
    }

#ifdef DEBUG
    inline void run_debug_audio_player(vector<AudioData> data_channels)
    {
        assert(data_channels.size() > 0);
        int channel_size = data_channels[0].size();
        for(int i=1; i<(int)data_channels.size(); i++)
        {
            assert((int)data_channels[i].size() == channel_size);
        }

        for(int i=0; i<(int)data_channels.size(); i++)
        {
            string name = "out";
            name.push_back(i + 48);
            create_new_output_port(name.c_str());
            debug_player_indices.push_back(0);
        }

        debug_player_data_channels = data_channels;

        jack_set_process_callback(client, on_debug_audio_player_io, this);
    }

    inline void do_debug_audio_player_io(unsigned int sample_count)
    {
        for(int i=0; i<(int)output_ports.size(); i++)
        {
           float* port = (float*) jack_port_get_buffer(output_ports[i], sample_count);
           memcpy(port, debug_player_data_channels[i].data() + debug_player_indices[i], sizeof(float) * sample_count);
           if(debug_player_indices[i] + sample_count < debug_player_data_channels[i].size())
               debug_player_indices[i] += sample_count;
        }
    }

#endif//DEBUG
};
    
    
int on_audio_io(jack_nframes_t nframes, void* arg)
{
    auto p = (JackAudioEnginePrivate*) arg;
    p->do_audio_io(nframes);
    return 0;
}


#ifdef DEBUG
int on_debug_audio_player_io(jack_nframes_t nframes, void* arg)
{
    auto p = (JackAudioEnginePrivate*) arg;
    p->do_debug_audio_player_io(nframes);
    return 0;
}
#endif//DEBUG


int on_sample_rate_change(jack_nframes_t new_rate, void* arg)
{
    auto p = (JackAudioEnginePrivate*) arg;
    p->set_sample_rate(new_rate);
    return 0;
}


int on_buffer_size_change(jack_nframes_t new_size, void *arg)
{
    auto p = (JackAudioEnginePrivate*) arg;
    p->set_buffer_size(new_size);
    return 0;
}


void on_shutdown(void* arg)
{
    cerr << "Jack::Shutdown!\n";
}


void on_error(const char* str)
{
    cerr << "Jack::Error: " << str << "\n";
}

    
JackAudioEngine::JackAudioEngine(const char* client_name)
{
    p = new JackAudioEnginePrivate;
    AudioEngine::is_valid = p->init(this, client_name);
}


JackAudioEngine::~JackAudioEngine()
{
    p->cleanup();
}


int JackAudioEngine::createInputPort(const char* name)
{
    createInputBuffer();
    return p->create_new_input_port(name);
}
    
    
int JackAudioEngine::createOutputPort(const char* name)
{
    createOutputBuffer();
    return p->create_new_output_port(name);
}


void JackAudioEngine::activate()
{
    p->activate();
}
    

void JackAudioEngine::deactivate()
{
    p->deactivate();
}

#ifdef DEBUG
void JackAudioEngine::runDebugAudioPlayer(vector<AudioData> data_channels)
{
    p->run_debug_audio_player(data_channels);
}
#endif//DEBUG

    
}//namespace r64fx
