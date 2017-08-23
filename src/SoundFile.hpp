#ifndef R64FX_SOUND_FILE_HPP
#define R64FX_SOUND_FILE_HPP

namespace r64fx{

class SoundFile{
public:
    enum class Mode{
        Bad,
        Read,
        Write,
        ReadWrite
    };

private:
    void* m                 = nullptr;
    SoundFile::Mode m_mode  = SoundFile::Mode::Bad;
    int m_component_count   = 0;
    int m_frame_count       = 0;
    int m_sample_rate       = 0;

public:
    SoundFile();

    SoundFile(const char* path, SoundFile::Mode mode);

    ~SoundFile();

    void open(const char* path, SoundFile::Mode mode);

    void close();

    bool isGood() const;

    SoundFile::Mode mode() const;

    int componentCount() const;

    int frameCount() const;

    int sampleRate() const;

    int readFrames(float* out, int nframes);

    int readFramesUnpack(float** out, int nframes);

    int seek(int frames, int whence);
};

}//namespace r64fx

#endif//R64FX_SOUND_FILE_HPP
