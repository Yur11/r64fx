#include "SoundFilePool.hpp"
#include "SoundFileDataPtr.hpp"
#include "SoundFile.hpp"
#include "LinkedList.hpp"
#include "Timer.hpp"

#include <iostream>
using namespace std;

namespace r64fx{
    
struct SoundFileDataPtrPrivate : public LinkedList<SoundFileDataPtrPrivate>::Node{
    SoundFileData data;
    unsigned long frames_loaded = 0;
    unsigned long user_count = 0;
    SoundFilePoolPrivate* pool = nullptr;
    std::string path = "";
    
    SoundFileDataPtrPrivate(SoundFilePoolPrivate* pool);
    
    ~SoundFileDataPtrPrivate();
    
    void loadMoreFrames(SoundFile* file, unsigned long nframes);
};


struct SoundFilePoolPrivate{
    LinkedList<SoundFileDataPtrPrivate> ptrs;
    SoundFile file;
    SoundFileDataPtrPrivate* currently_loading = nullptr;
    Timer timer;
};


SoundFileDataPtrPrivate::SoundFileDataPtrPrivate(SoundFilePoolPrivate* pool)
{
    this->pool = pool;
    pool->ptrs.append(this);
}
    
    
SoundFileDataPtrPrivate::~SoundFileDataPtrPrivate()
{
    pool->ptrs.remove(this);
    data.free();
}


SoundFileDataPtr::SoundFileDataPtr()
{
    
}


SoundFileDataPtr::SoundFileDataPtr(SoundFileDataPtrPrivate* p)
{
    m = p;
    m->user_count++;
}


SoundFileDataPtr::SoundFileDataPtr(const SoundFileDataPtr &other)
{
    m = other.m;
    m->user_count++;
}


SoundFileDataPtr::~SoundFileDataPtr()
{
    clear();
}


void SoundFileDataPtr::clear()
{
    if(m)
    {
        m->user_count--;
        if(m->user_count == 0)
        {
            delete m;
        }
    }
}


SoundFileDataPtr &SoundFileDataPtr::operator=(const SoundFileDataPtr &other)
{
    m = other.m;
    m->user_count++;
    return *this;
}


SoundFileData* SoundFileDataPtr::data() const
{
    return &(m->data);
}


unsigned long SoundFileDataPtr::frameCount() const
{
    return m->data.frameCount();
}


unsigned long SoundFileDataPtr::loadedFrameCount() const
{
    return m->frames_loaded;
}


unsigned long SoundFileDataPtr::componentCount() const
{
    return m->data.componentCount();
}
    
    
int SoundFileDataPtr::sampleRate() const
{
    return m->data.sampleRate();
}


SoundFileData* SoundFileDataPtr::operator->()
{
    return &(m->data);
}

    
SoundFilePool::SoundFilePool()
{
    m = new SoundFilePoolPrivate;
}


SoundFilePool::~SoundFilePool()
{
    delete m;
}


SoundFileDataPtr SoundFilePool::load(const std::string &path)
{
    auto ptr = find(path);
    if(ptr)
        return ptr;

    if(m->file.isGood())
    {
        cerr << "Refusing to load another file!\n";
        cerr << "\"" << path << "\"\n";
        return SoundFileDataPtr();
    }
    
    auto ptrp = new SoundFileDataPtrPrivate(m);
    ptrp->user_count = 1;
    ptrp->path = path;
    m->file.open(path, SoundFile::Mode::Read);
    if(!m->file.isGood())
    {
        cerr << "Failed to load \"" << path << "\"!\n";
        return SoundFileDataPtr();
    }
    
    if(m->file.frameCount() <= 0 || m->file.componentCount() <= 0)
    {
        cerr << "Bad sound file specs!\n";
        m->file.close();
        return SoundFileDataPtr();
    }
    
    ptrp->data.load(m->file.frameCount(), m->file.componentCount());
    ptrp->data.setSampleRate(m->file.sampleRate());
    ptrp->loadMoreFrames(&(m->file), m->file.frameCount());
    
    m->file.close();
    
    return SoundFileDataPtr(ptrp);
}


void SoundFileDataPtrPrivate::loadMoreFrames(SoundFile* file, unsigned long nframes)
{
    unsigned long frames_left = data.frameCount() - frames_loaded;
    if(frames_left > 0)
    {
        unsigned long actual_frames = min(nframes, frames_left);
        file->readFrames(data.frame(frames_loaded), actual_frames);
    }
}


SoundFileDataPtr SoundFilePool::find(const std::string &path)
{
    for(auto ptr : m->ptrs)
    {
        if(ptr->path == path)
        {
            return SoundFileDataPtr(ptr);
        }
    }
    
    return SoundFileDataPtr();
}
    
}//namespace
