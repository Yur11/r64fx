#ifndef R64FX_ARRAY_HPP
#define R64FX_ARRAY_HPP

namespace r64fx{
    
template<typename T> class Array{
    T* m_buffer = nullptr;
    unsigned long m_size = 0;
    
public:
    Array()
    {
        
    }
    
    Array(unsigned long size)
    {
        resize(size);
    }
    
    ~Array()
    {
        clear();
    }
    
    inline T &at(unsigned long i) const
    {
        return m_buffer[i];
    }
    
    inline T &operator[](unsigned long i) const
    {
        return at(i);
    }
    
    inline unsigned long size() const
    {
        return m_size;
    }
    
    inline void clear()
    {
        if(m_buffer)
        {
            delete[] m_buffer;
            m_size = 0;
            m_buffer = nullptr;
        }
    }
    
    inline void resize(unsigned long size)
    {
        clear();
        if(size > 0)
        {
            m_buffer = new T[size];
            m_size = size;
        }
    }
};
    
}//namespace r64fx

#endif//R64FX_ARRAY_HPP