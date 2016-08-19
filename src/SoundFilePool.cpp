#include "SoundFilePool.hpp"
#include "LinkedList.hpp"
#include "Timer.hpp"

namespace r64fx{
    
struct SoundFileDataPtrPrivate : public LinkedList<SoundFileDataPtrPrivate>::Node{
    SoundFileData data;
    unsigned long frames_loaded = 0;
    unsigned long user_count = 0;
    SoundFilePoolPrivate* pool = nullptr;
    std::string path = "";
    
    SoundFileDataPtrPrivate(SoundFilePoolPrivate* pool);
    
    ~SoundFileDataPtrPrivate();
};


struct SoundFilePoolPrivate{
    LinkedList<SoundFileDataPtrPrivate> ptrs;
    SoundFile file;
    SoundFileDataPtrPrivate* currently_loading = nullptr;
    Timer timer;
};


SoundFileDataPtrPrivate::SoundFileDataPtrPrivate(SoundFilePoolPrivate* pool)
{
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


SoundFileData* SoundFileDataPtr::data() const
{
    return &(m->data);
}


unsigned long SoundFileDataPtr::totalFrames() const
{
    return m->data.frameCount();
}


unsigned long SoundFileDataPtr::framesLoaded() const
{
    return m->frames_loaded;
}
    
    
SoundFilePool::SoundFilePool()
{
    m = new SoundFilePoolPrivate;
}


SoundFilePool::~SoundFilePool()
{
    delete m;
}


SoundFileDataPtr SoundFilePool::loadFile(const std::string &path)
{
    auto ptr = findLoaded(path);
    if(ptr)
        return ptr;
    
    auto ptrp = new SoundFileDataPtrPrivate(m);
    ptrp->path = path;
    /* Load file here! */
    
    return SoundFileDataPtr(ptrp);
}


SoundFileDataPtr SoundFilePool::findLoaded(const std::string &path)
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