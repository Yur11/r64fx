#ifndef R64FX_SOUND_FILE_DATA_PTR_HPP
#define R64FX_SOUND_FILE_DATA_PTR_HPP

#include "SoundFileData.hpp"

namespace r64fx{
  
class SoundFileDataPtrPrivate;
    
class SoundFileDataPtr{
    SoundFileDataPtrPrivate* m = nullptr;
    
public:
    explicit SoundFileDataPtr();
    
    explicit SoundFileDataPtr(SoundFileDataPtrPrivate* p);
    
    SoundFileDataPtr(const SoundFileDataPtr &other);

    ~SoundFileDataPtr();
    
    void clear();
    
    SoundFileDataPtr &operator=(const SoundFileDataPtr &other);
    
    inline operator bool() const
    {
        return m != nullptr;
    }
    
    SoundFileData* data() const;
    
    unsigned long frameCount() const;
    
    unsigned long loadedFrameCount() const;
    
    unsigned long componentCount() const;
    
    int sampleRate() const;

    SoundFileData* operator->();
};
    
}//namespace r64fx

#endif//R64FX_SOUND_FILE_DATA_PTR_HPP
