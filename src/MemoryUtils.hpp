#ifndef R64FX_MEMORY_UTILS_HPP
#define R64FX_MEMORY_UTILS_HPP

#include "LinkedList.hpp"

namespace r64fx{

int memory_page_size();

void* alloc_aligned(int alignment, int nbytes);


class HeapBuffer : public LinkedList<HeapBuffer>::Node{
    void* m_buffer = nullptr;
    long  m_size = 0;

    HeapBuffer(void* buffer, long size) : m_buffer(buffer), m_size(size) {};
    HeapBuffer(const HeapBuffer&){};
    ~HeapBuffer(){};

public:
    inline void* buffer() const { return m_buffer; }

    inline long size() const { return m_size; }

    static HeapBuffer* newInstance(long nbytes);

    static void deleteInstance(HeapBuffer* buffer);

    void* alloc(long nbytes);

    bool free(void* addr);

    bool isEmpty() const;

#ifdef R64FX_DEBUG
    void dumpHeader();
#endif//R64FX_DEBUG
};


class HeapAllocator{
    LinkedList<HeapBuffer> m_buffers;

public:
    void* alloc(long nbytes);

    void free(void* addr);

    bool isEmpty() const;

#ifdef R64FX_DEBUG
    void dumpHeaders();

    void debugAddr(void* addr);
#endif//R64FX_DEBUG
};

}//namespace r64fx

#endif//R64FX_MEMORY_UTILS_HPP
