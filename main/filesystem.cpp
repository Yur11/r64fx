#include <dirent.h>

namespace r64fx{
namespace Filesystem{


class Entery{
    template<typename T> friend bool listDirectory(T, string, bool (*callback)(T, string prefix, Filesystem::Entery));

    dirent* _ent;

    Entery(dirent* ent) : _ent(ent) {}

public:
    inline const char* name() { return _ent->d_name; }

    inline bool isDir() const { return _ent->d_type == DT_DIR; }
};


template<typename T> bool listDirectory(T t, string path, bool (*callback)(T, string prefix, Filesystem::Entery))
{
    if(path.back() != '/')
        path.push_back('/');

    DIR* dir = opendir(path.c_str());
    if(dir == nullptr)
        return false;

    dirent* entery;
    while((entery = readdir(dir)) && callback(t, path, Entery(entery))) {}

    closedir(dir);
    return true;
}

}//namespace Filesystem
}//namespace r64fx