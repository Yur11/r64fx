#include "Dir.hpp"
#include "dirent.h"
#include "unistd.h"

namespace r64fx{

Dir::Dir()
{
    m_path = Dir::current();
    handle = (void*) opendir(m_path.c_str());
}

    
Dir::Dir(const std::string &path)
{
    m_path = path;
    handle = (void*) opendir(m_path.c_str());
}


Dir::~Dir()
{
    if(isGood())
    {
        closedir((DIR*)handle);
    }
}


bool Dir::isGood() const
{
    return handle != nullptr;
}


bool Dir::Entery::isGood() const
{
    return handle != nullptr;
}
    
    
std::string Dir::Entery::name() const
{
    auto ent = (dirent*) handle;
    return ent->d_name;
}


Dir::Entery Dir::read()
{
    return Dir::Entery(readdir((DIR*)handle));
}


void Dir::rewind()
{
    rewinddir((DIR*)handle);
}


std::string Dir::current()
{
    char buff[PATH_MAX];
    return getcwd(buff, PATH_MAX);
}

}//namespace r64fx