#ifndef R64FX_UTIL_DIR_HPP
#define R64FX_UTIL_DIR_HPP

#include <string>

namespace r64fx{

class Dir{
    void* handle = nullptr;
    std::string m_path;
    
public:
    Dir();
    
    Dir(const std::string &path);
    
    ~Dir();
    
    bool isGood() const;
    
    inline operator bool() const { return isGood(); }
    
    inline std::string path() const { return m_path; }
    
    class Entery{
        friend class Dir;
        
        void* handle = nullptr;
        
        Entery(void* handle)
        : handle(handle)
        {}
        
    public:
        Entery() {}
        
        bool isGood() const;
        
        inline operator bool() const { return isGood(); }
        
        std::string name() const;
    };
    
    Dir::Entery read();
    
    void rewind();
    
    static std::string current();
};

}//namespace r64fx

#endif//R64FX_UTIL_DIR_HPP