#ifndef R64FX_SOUND_FILE_LOADER_HPP
#define R64FX_SOUND_FILE_LOADER_HPP

namespace r64fx{

class SoundFileHandle;

typedef void (Response_OpenSoundFile)          (SoundFileHandle* handle, void* data);
typedef void (Response_GetSoundFileProperties) (SoundFileHandle* handle, float sample_rate, int frame_count, int component_count, void* data);
typedef void (Response_LoadSoundFileChunk)     (SoundFileHandle* handle, float* chunk, int index, int nframes, void* data);
typedef void (Response_FreeSoundFileChunk)     (SoundFileHandle* handle, void* data);
typedef void (Response_CloseSoundFile)         (void* data);


class SoundFileLoader{
    void* SoundFileLoader_Members[2];

public:
    class Port{
        friend class SoundFileLoader;

        void* SoundFileLoaderPort_Members = nullptr;

        Port();

        ~Port();

    public:
        void run();

        void open(const char* file_path, Response_OpenSoundFile* callback, void* data);

        void getFileProperties(SoundFileHandle* handle, Response_GetSoundFileProperties* callback, void* data);

        void loadChunk(SoundFileHandle* handle, int index, int nframes, Response_LoadSoundFileChunk* callback, void* data);

        void freeChunk(SoundFileHandle* handle, float* chunk, Response_FreeSoundFileChunk* callback, void* data);

        void close(SoundFileHandle* handle, Response_CloseSoundFile* callback, void* data);
    };

    SoundFileLoader();

    ~SoundFileLoader();

    SoundFileLoader::Port* newPort();

    void deletePort(SoundFileLoader::Port* port);
};

}//namespace r64fx

#endif//R64FX_SOUND_FILE_LOADER_HPP
