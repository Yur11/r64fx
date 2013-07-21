#include "AudioData.h"
#include <sndfile.h>
#include <samplerate.h>
#ifdef DEBUG
#include <assert.h>
#include <iostream>
#endif//DEBUG


using namespace std;

namespace r64fx{


template<typename T> inline T max(T a, T b) { return a > b ? a : b; }


AudioData::AudioData(unsigned long int size, float* data)
: _data(data)
, _size(size)
{
    if(_data == nullptr && size > 0)
    {
        _data = new float[size];
    }
}


bool feedback_stub(void*, unsigned long int, unsigned long int) { return true; }


std::vector<AudioData> AudioData::readFile(
    const char* file_path,
    int target_samplerate,
    const unsigned int chunk_size,
    bool (*feedback)(void*, unsigned long int, unsigned long int),
    void* feedback_data
)
{
    vector<AudioData> channels;

    /* See libsndfile http://www.mega-nerd.com/libsndfile/ */
    SF_INFO file_info;
    file_info.format = 0;
    SNDFILE* sndfile = sf_open(file_path, SFM_READ, &file_info);
    if(!sndfile)
        return channels;

    if(!feedback)
        feedback = &feedback_stub;

#ifdef DEBUG
    assert(file_info.channels > 0);
#endif//DEBUG

    double samplerate_ratio = double(target_samplerate) / double(file_info.samplerate);

    for(int i=0; i<file_info.channels; i++)
    {
        channels.push_back(AudioData(file_info.frames * samplerate_ratio));
    }

    /* See linsamplerate (Secret Rabbit Code) http://www.mega-nerd.com/SRC/ */
    SRC_STATE* src = nullptr;            //Not used if no samplerate conversion is done.
    float* converted_buffer = nullptr;   //Used to hold data chunk after converting the samplerate.
    int converted_buffer_size = 0;       //Not used if no samplerate conversion is done.
    if(samplerate_ratio != 1.0)
    {
        int error;
        src = src_new(SRC_SINC_MEDIUM_QUALITY, file_info.channels, &error);
        if(!src)
        {
            sf_close(sndfile);
            return channels;
        }
        src_set_ratio(src, samplerate_ratio);
        converted_buffer_size = chunk_size * samplerate_ratio + 1;
        converted_buffer = new float[converted_buffer_size];
    }

    /* Read the data in chunks. Convert the samplerate if needed. */
    int frames_to_read = chunk_size / file_info.channels;
    int frames_read = 0;
    while(frames_read < file_info.frames)
    {
        float buffer[chunk_size];
        float* buff_ptr = buffer;
        auto frames_read_at_this_time = sf_readf_float(sndfile, buffer, frames_to_read);
        if(frames_read_at_this_time == 0) break;

        if(samplerate_ratio != 1.0)
        {
            SRC_DATA src_data;
            src_data.data_in = buffer;
            src_data.data_out = converted_buffer;
            src_data.input_frames = frames_read_at_this_time;
            src_data.output_frames = frames_read_at_this_time * samplerate_ratio;
            src_data.src_ratio = samplerate_ratio;
            src_data.end_of_input = 0;
            buff_ptr = converted_buffer;

            int status = src_process(src, &src_data);
            if(status != 0)
            {
#ifdef DEBUG
                cerr << "Samplerate conversion failed!\n";
                cerr << src_strerror(status);
                abort();
#endif//DEBUG
            }

            buff_ptr = converted_buffer;
        }

        for(int i=0; i<frames_read_at_this_time * samplerate_ratio; i++)
        {
            for(int ch=0; ch<file_info.channels; ch++)
            {
                channels[ch][frames_read * samplerate_ratio + i] = buff_ptr[i * file_info.channels + ch];
            }
        }

        frames_read += frames_read_at_this_time;

        if(!feedback(feedback_data, frames_read, file_info.frames))
        {
            goto _cleanup;
        }
    }

_cleanup:
    if(samplerate_ratio != 1.0)
    {
        delete[] converted_buffer;
        src_delete(src);
    }

    sf_close(sndfile);

    return channels;
}

}//namespace r64fx