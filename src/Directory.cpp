#include "Directory.hpp"
#include <dirent.h>

#define m_dir ((DIR*)m)

namespace r64fx{

Directory::Directory(const std::string &path)
{
    open(path);
}


Directory::Directory()
{

}


Directory::~Directory()
{
    if(m_dir)
    {
        closedir(m_dir);
    }
}


bool Directory::open(const std::string &path)
{
    m = opendir(path.c_str());
    return isOpen();
}


bool Directory::isOpen() const
{
    return m != nullptr;
}


Directory::Entry::Type getEntryType(dirent* ent)
{
    switch(ent->d_type)
    {
        case DT_REG:
            return Directory::Entry::Type::RegularFile;

        case DT_DIR:
            return Directory::Entry::Type::Directory;

        default:
            return Directory::Entry::Type::Other;
    };
}


void Directory::forEachEntry(void (*fun)(Directory::Entry* entry, void* arg), void* arg)
{
    if(!isOpen())
        return;

    for(;;)
    {
        errno = 0;
        dirent* ent = readdir(m_dir);
        if(!ent)
            break;
        Directory::Entry entry(getEntryType(ent), std::string(ent->d_name));
        fun(&entry, arg);
    }
}

}//namespace r64fx