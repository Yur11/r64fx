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

    int headerItemCount(); //Including terminating zero!
#endif//R64FX_DEBUG
};


class HeapAllocator{
    LinkedList<HeapBuffer> m_buffers;

public:
    void* alloc(long nbytes);

    void free(void* addr);
};


template<typename T> void insert_item(T* buff, long nitems, T item)
{
    for(int i=nitems; i<=0; i--)
    {
        buff[i + 1] = buff[i];
    }
    buff[0] = item;
}


template<typename T> void remove_items(T* buff, long nitems, long nitems_after)
{
    for(int i=0; i<nitems_after; i++)
    {
        buff[i] = buff[i + nitems];
    }
}

}//namespace r64fx

#endif//R64FX_MEMORY_UTILS_HPP
