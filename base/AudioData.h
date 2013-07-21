#ifndef R64FX_BASE_AUDIO_DATA_H
#define R64FX_BASE_AUDIO_DATA_H

#include <vector>

namespace r64fx{

/** @brief An array of audio data. */
class AudioData{
    float* _data;
    unsigned long int _size;

public:
    AudioData(unsigned long int size = 0, float* data = nullptr);

    static std::vector<AudioData> readFile(
        const char* path,
        int target_samplerate,
        const unsigned int chunk_size = 32,
        bool (*feedback)(void*, unsigned long int processed_samples, unsigned long int total_samples) = nullptr,
        void* feedback_data = nullptr
    );

    inline float &operator[](unsigned long int index) { return _data[index]; }

    inline float* data() const { return _data; }

    inline unsigned long int size() const { return _size; }

    inline bool isGood() { return _data != nullptr && _size != 0; }
};

}//namespace r64fx

#endif//R64FX_BASE_AUDIO_DATA_H