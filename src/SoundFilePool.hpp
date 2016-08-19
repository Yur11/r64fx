#ifndef R64FX_SOUND_FILE_POOL_HPP
#define R64FX_SOUND_FILE_POOL_HPP

#include "SoundFile.hpp"
#include "SoundFileData.hpp"

namespace r64fx{
    
class SoundFileDataPtrPrivate;
class SoundFilePoolPrivate;

class SoundFileDataPtr{
    SoundFileDataPtrPrivate* m = nullptr;
    
public:
    explicit SoundFileDataPtr();
    
    explicit SoundFileDataPtr(SoundFileDataPtrPrivate* p);
    
    SoundFileDataPtr(const SoundFileDataPtr &other);

    ~SoundFileDataPtr();
    
    void clear();
    
    inline operator bool() const
    {
        return m != nullptr;
    }
    
    SoundFileData* data() const;
    
    unsigned long frameCount() const;
    
    unsigned long loadedFrameCount() const;
    
    unsigned long componentCount() const;
};


class SoundFilePool{
    SoundFilePoolPrivate* m = nullptr;
    
public:
    SoundFilePool();
    
    ~SoundFilePool();
    
    SoundFileDataPtr load(const std::string &path);
    
    SoundFileDataPtr find(const std::string &path);    
};
    
}//namespace r64fx

#endif//R64FX_SOUND_FILE_POOL_HPP