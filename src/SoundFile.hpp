#ifndef R64FX_SOUND_FILE_HPP
#define R64FX_SOUND_FILE_HPP

#include <string>

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
    void* m = nullptr;
    SoundFile::Mode m_mode = SoundFile::Mode::Bad;
    int m_channel_count = 0;
    int m_frame_count   = 0;
    int m_sample_rate   = 0;

public:

    SoundFile();

    SoundFile(const std::string &path, SoundFile::Mode mode);

    ~SoundFile();

    void open(const std::string &path, SoundFile::Mode mode);

    void close();

    bool isGood() const;

    SoundFile::Mode mode() const;

    int channelCount() const;

    int frameCount() const;

    int sampleRate() const;

    int readFrames(float* out, int nframes);

    int readFramesUnpack(float** out, int nframes);
};

}//namespace r64fx

#endif//R64FX_SOUND_FILE_HPP