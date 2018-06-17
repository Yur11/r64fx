#ifndef R64FX_MEMORY_UTILS_HPP
#define R64FX_MEMORY_UTILS_HPP

#include "LinkedList.hpp"
#include "Debug.hpp"
#include <new>

namespace r64fx{

unsigned int memory_page_size();

void* alloc_aligned(int alignment, int nbytes);

inline void* alloc_pages(int npages) { return alloc_aligned(memory_page_size(), memory_page_size() * npages); }

template<typename T> inline T* alloc_buffer(int size) { return (T*)alloc_aligned(64, sizeof(T)*size); }


/* Dynamically allocated memory buffer. */
class MemoryBuffer{
    unsigned char* m_begin  = nullptr;
    unsigned char* m_ptr    = nullptr;
    unsigned char* m_end    = nullptr;

public:
    MemoryBuffer(unsigned long npages) { resize(npages); }

    MemoryBuffer() {}

    ~MemoryBuffer() { resize(0); }

    /* Buffer is a multiple of value returned by memory_page_size() function.
       In case of reallocation existing data is copied. */
    void resize(unsigned long npages);

    /* Make sure that this buffer has at least nbytes available between ptr() and end().
       This method calls resize() method if reallocation is needed. */
    void ensure(unsigned long nbytes);

    /* Ensure that nbytes are available an move ptr forward by nbytes.
       Returns ptr value before advancing, but after any reallocations. */
    inline unsigned char* grow(unsigned long nbytes)
        { ensure(nbytes); auto p = m_ptr; m_ptr += nbytes; return p; }

    /* Move ptr nbytes back. */
    inline unsigned char* shrink(unsigned long nbytes)
    {
        R64FX_DEBUG_ASSERT(nbytes <= bytesUsed());
        m_ptr -= nbytes;
        return m_ptr;
    }

    /* Move ptr back to the beginning of the buffer. */
    inline unsigned char* rewind()
    {
        m_ptr = m_begin;
        return m_ptr;
    }

    /* Pointer to the first byte in this buffer. */
    inline unsigned char* begin() const { return m_begin; }

    /* Pointer that can move between begin and end. */
    inline unsigned char* ptr() const { return m_ptr; }

    /* Pointer past the last byte in this buffer. */
    inline unsigned char* end() const { return m_end; }

    inline unsigned long nbytes() const { return m_end - m_begin; }

    inline unsigned long npages() const { return nbytes() / memory_page_size(); }

    inline unsigned long bytesUsed() const { return m_ptr - m_begin; }

    inline unsigned long bytesAvail() const { return m_end - m_ptr; }
};


/* Buffer that can allocate smaller object inside itself.
   Chunks must be 4 byte aligned. */
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


/* Memory allocator that uses HeapBuffer instances. */
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
