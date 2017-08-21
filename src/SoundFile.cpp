#include "SoundFile.hpp"
#include <sndfile.h>

#define m_sndfile ((SNDFILE*)m)

namespace r64fx{

SoundFile::SoundFile()
{

}


SoundFile::SoundFile(const char* path, SoundFile::Mode mode)
{
    open(path, mode);
}


SoundFile::~SoundFile()
{
    close();
}


void SoundFile::open(const char* path, SoundFile::Mode mode)
{
    close();

    SF_INFO sfinfo;

    int sndfilemode;
    switch(mode)
    {
        case SoundFile::Mode::Read:
        {
            sfinfo.format = 0;
            sndfilemode = SFM_READ;
            break;
        }

        case SoundFile::Mode::Write:
        {
            sndfilemode = SFM_WRITE;
            break;
        }

        case SoundFile::Mode::ReadWrite:
        {
            sndfilemode = SFM_RDWR;
            break;
        }

        default:
            return;
    }

    m = sf_open(path, sndfilemode, &sfinfo);
    if(m)
    {
        m_mode = mode;
        m_component_count = sfinfo.channels;
        m_frame_count     = sfinfo.frames;
        m_sample_rate     = sfinfo.samplerate;
    }
    else
    {
        m_mode = Mode::Bad;
        m_component_count = 0;
        m_frame_count = 0;
        m_sample_rate = 0;
    }
}


void SoundFile::close()
{
    if(m)
    {
        sf_close(m_sndfile);
        m_component_count = 0;
        m_frame_count   = 0;
        m_sample_rate   = 0;
        m = nullptr;
    }
}


bool SoundFile::isGood() const
{
    return m != nullptr;
}


SoundFile::Mode SoundFile::mode() const
{
    return m_mode;
}


int SoundFile::componentCount() const
{
    return m_component_count;
}


int SoundFile::frameCount() const
{
    return m_frame_count;
}


int SoundFile::sampleRate() const
{
    return m_sample_rate;
}


int SoundFile::readFrames(float* out, int nframes)
{
    return sf_readf_float(m_sndfile, out, nframes);
}


int SoundFile::readFramesUnpack(float** out, int nframes)
{
    int frames_read = 0;
    float* buff = new float[componentCount()];

    for(int i=0; i<nframes; i++)
    {
        int chans_read = readFrames(buff, 1) < componentCount();
        if(chans_read != 1)
        {
            break;
        }

        for(auto c=0; c<componentCount(); c++)
        {
            out[c][i] = buff[c];
        }
        frames_read++;
    }

    delete[] buff;
    return frames_read;
}

}//namespace r64fx
