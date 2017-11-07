#ifndef R64FX_MEMORY_UTILS_HPP
#define R64FX_MEMORY_UTILS_HPP

#include "LinkedList.hpp"

namespace r64fx{

unsigned int memory_page_size();

void* alloc_aligned(int alignment, int nbytes);


class HeapBuffer : public LinkedList<HeapBuffer>::Node{
    void* m_buffer = nullptr;
    long  m_size = 0;

public:
    HeapBuffer(){}
    HeapBuffer(void* buffer, long size) : m_buffer(buffer), m_size(size) {};
    HeapBuffer(const HeapBuffer&){};
    ~HeapBuffer(){};

    inline void setBuffer(void* buffer, long size) { m_buffer = buffer; m_size = size; }

    inline void* buffer() const { return m_buffer; }

    inline long size() const { return m_size; }

    static HeapBuffer* newSelfHostedInstance(unsigned int npages);

    static void deleteSelfHostedInstance(HeapBuffer* buffer);

    void* allocChunk(long nbytes);

    void* allocChunk(long nbytes, long alignment);

    long chunkSize(void* addr);

    bool freeChunk(void* addr);

    bool empty() const;

    unsigned int headerSize() const;

#ifdef R64FX_DEBUG
    void dumpHeader();
#endif//R64FX_DEBUG
};


class HeapAllocator{
    LinkedList<HeapBuffer> m_buffers;

public:
    void* allocChunk(long nbytes, long alignment);

    void freeChunk(void* addr);

    bool empty() const;

#ifdef R64FX_DEBUG
    void dumpHeaders();

    void debugAddr(void* addr);
#endif//R64FX_DEBUG

    template<typename T, typename... CtorArgs> inline T* allocObj(CtorArgs... ctor_args)
    {
        int chunksize = sizeof(T);
        while(chunksize & 7)
            chunksize++;
        void* memchunk = allocChunk(chunksize, alignof(T));
        if(!chunksize)
            return nullptr;
        return new(memchunk) T(ctor_args...);
    }

    template<typename T> inline void freeObj(T* obj)
    {
        obj->~T();
        HeapAllocator::freeChunk(obj);
    }
};

}//namespace r64fx

#endif//R64FX_MEMORY_UTILS_HPP
