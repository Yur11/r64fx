#ifndef R64FX_SOUND_FILE_LOADER_HPP
#define R64FX_SOUND_FILE_LOADER_HPP

namespace r64fx{

class SoundFileHandle;

class SoundFileLoader{
    void* SoundFileLoader_Members[2];

public:
    class Port{
        friend class SoundFileLoader;

        void* SoundFileLoaderPort_Members = nullptr;

        Port();

        ~Port();

    public:
        void readMessages();

        SoundFileHandle* open(const char* file_path);

        void getFileProperties(SoundFileHandle* handle, void (*callback)(float sample_rate, int frame_count, int component_count));

        void loadChunk(SoundFileHandle* handle, int index, int nframes, void (*callback)(float* chunk, int index, int nframes));

        void close(SoundFileHandle* handle);
    };

    SoundFileLoader();

    ~SoundFileLoader();

    SoundFileLoader::Port* newPort();

    void freePort(SoundFileLoader::Port* port);
};

}//namespace r64fx

#endif//R64FX_SOUND_FILE_LOADER_HPP
