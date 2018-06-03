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
       This method calls resize() method if reallocation is needed.*/
    void ensure(unsigned long nbytes);

    /* Ensure that nbytes are available an move ptr forward by nbytes. */
    inline unsigned char* grow(unsigned long nbytes) { ensure(nbytes); m_ptr += nbytes; return m_ptr; }

    /* Move ptr nbytes back. */
    inline unsigned char* shrink(unsigned long nbytes)
    {
        R64FX_DEBUG_ASSERT(nbytes <= bytesUsed());
        m_ptr -= nbytes;
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


/* Two memory buffers concatenated together.
   One is called increasing buffer and grows towards larger addresses.
   The other is called decreasing buffer and grows towards smaller addresses.
   Both meet in the middle and are reallocated together if any of them has grow. */
class DivergingBuffers{
    unsigned char*  m_begin     = nullptr;
    unsigned char*  m_decr_ptr  = nullptr;
    unsigned char*  m_middle    = nullptr;
    unsigned char*  m_incr_ptr  = nullptr;
    unsigned char*  m_end       = nullptr;

public:
    DivergingBuffers(unsigned long decr_page_count, unsigned long incr_page_count)
    {
        resize(decr_page_count, incr_page_count);
    }

    DivergingBuffers() {}

    ~DivergingBuffers() { resize(0, 0); }

    /* Similar to resize method in MemoryBuffer but for both buffers. */
    void resize(unsigned long decr_page_count, unsigned long incr_page_count);

    /* Similar to ensure method in MemoryBuffer but for both buffers. */
    void ensure(unsigned long decr_byte_count, unsigned long incr_byte_count);

    void ensureDecr(unsigned long decr_byte_count);

    void ensureIncr(unsigned long incr_byte_count);

    inline unsigned char* growDecr(unsigned long decr_byte_count)
        { ensureDecr(decr_byte_count); m_decr_ptr -= decr_byte_count; return m_decr_ptr; }

    inline unsigned char* growIncr(unsigned long incr_byte_count)
        { ensureIncr(incr_byte_count); m_incr_ptr += incr_byte_count; return m_incr_ptr; }

    inline unsigned long decrByteCount() const { R64FX_DEBUG_ASSERT(m_middle >= m_begin); return m_middle - m_begin; }
    inline unsigned long incrByteCount() const { R64FX_DEBUG_ASSERT(m_end >= m_middle); return m_end - m_middle; }

    inline unsigned long decrPageCount() const { return decrByteCount() / memory_page_size(); }
    inline unsigned long incrPageCount() const { return incrByteCount() / memory_page_size(); }

    inline unsigned long decrBytesUsed() const { R64FX_DEBUG_ASSERT(m_middle >= m_decr_ptr); return m_middle - m_decr_ptr; }
    inline unsigned long incrBytesUsed() const { R64FX_DEBUG_ASSERT(m_incr_ptr >= m_middle); return m_incr_ptr - m_middle; }

    inline unsigned long decrBytesAvail() const { R64FX_DEBUG_ASSERT(m_decr_ptr >= m_begin); return m_decr_ptr - m_begin; }
    inline unsigned long incrBytesAvail() const { R64FX_DEBUG_ASSERT(m_end >= m_incr_ptr); return m_end - m_incr_ptr; }

    inline unsigned char* decrBegin() const { return m_begin; }
    inline unsigned char* decrPtr() const { return m_decr_ptr; }
    inline unsigned char* decrEnd() const { return m_middle; }

    inline unsigned char* incrBegin() const { return m_middle; }
    inline unsigned char* incrPtr() const { return m_incr_ptr; }
    inline unsigned char* incrEnd() const { return m_end; }
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
