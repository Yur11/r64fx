#include "ImageAnimation.hpp"
#include <vector>
#include <iostream>

#define m_frames ((vector<unsigned char*>*)m)

using namespace std;

namespace r64fx{

ImageAnimation::ImageAnimation(int w, int h, int c, int nframes)
: Image(0, 0, 0)
{
    resize(w, h, c, nframes);
}


ImageAnimation::ImageAnimation()
: Image(0, 0, 0)
{

}


ImageAnimation::~ImageAnimation()
{
    free();
}


void ImageAnimation::resize(int w, int h, int c, int nframes)
{
    if(w <= 0 || h <= 0 || c <= 0 || nframes <= 0)
        return;

    m = new(std::nothrow) vector<unsigned char*>(nframes, nullptr);
    if(!m)
    {
        cerr << "ImageAnimation: Failed to allocate frames!\n";
        return;
    }

    int nbytes = w * h * c;

    for(auto &frame : (*m_frames))
    {
        frame = new(std::nothrow) unsigned char[nbytes];
        if(!frame)
        {
            cerr << "ImageAnimation: Failed to allocate frame!\n";
            free();
            return;
        }
    }

    load(w, h, c, (*m_frames)[0]);
}


void ImageAnimation::free()
{
    if(m_frames)
    {
        for(auto &frame : (*m_frames))
        {
            if(frame)
            {
                delete[] frame;
                frame = nullptr;
            }
        }
        delete m_frames;
    }
}


bool ImageAnimation::isGood() const
{
    return m_frames != nullptr;
}


int ImageAnimation::frameCount() const
{
    return m_frames->size();
}


void ImageAnimation::pickFrame(int i)
{
    setData((*m_frames)[i]);
}

}//namespace r64fx