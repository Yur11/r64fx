#ifndef R64FX_SOUND_FILE_POOL_HPP
#define R64FX_SOUND_FILE_POOL_HPP

#include <string>
#include "SoundFileDataPtr.hpp"

namespace r64fx{

class SoundFilePoolPrivate;

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