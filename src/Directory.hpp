#ifndef R64FX_DIRECTORY_HPP
#define R64FX_DIRECTORY_HPP

#include <string>

namespace r64fx{

class Directory{
    void* m = nullptr;

public:
    Directory(const std::string &path);

    Directory();

    ~Directory();

    bool open(const std::string &path);

    bool isOpen() const;

    class Entry{
    public:
        enum class Type{
            None,
            RegularFile,
            Directory,
            Other
        };

    private:
        Type m_type = Type::None;
        std::string m_name = "";

    public:
        Entry(Type type, const std::string &name)
        : m_type(type)
        , m_name(name)
        {

        }

        inline Type type() const
        {
            return m_type;
        }

        inline std::string name() const
        {
            return m_name;
        }

        inline bool isDirectory() const
        {
            return type() == Directory::Entry::Type::Directory;
        }
    };

    void forEachEntry(void (*fun)(const Directory::Entry* entry, void* arg), void* arg);
};

}//namespace r64fx

#endif//R64FX_DIRECTORY_HPP