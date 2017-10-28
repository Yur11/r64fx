#include "MemoryUtils.hpp"
#include "Debug.hpp"
#include <unistd.h>
#include <stdlib.h>
#include <new>

#ifdef R64FX_DEBUG
#include <assert.h>
#include <iostream>
#endif//R64FX_DEBUG

namespace r64fx{

unsigned int memory_page_size()
{
    return getpagesize();
}

void* alloc_aligned(int alignment, int nbytes)
{
    void* memptr = nullptr;
    if(posix_memalign(&memptr, alignment, nbytes) != 0)
    {
#ifdef R64FX_DEBUG
        std::cerr << "posix_memalign() failed!\n";
#endif//R64FX_DEBUG
        return nullptr;
    }
    return memptr;
}


void* alloc_chunk(void* buff, long buff_bytes, long chunk_bytes)
{
    if(buff_bytes == 0)
        return nullptr;

    R64FX_DEBUG_ASSERT((long(buff)  & 3) == 0);
    R64FX_DEBUG_ASSERT((buff_bytes  & 3) == 0);
    R64FX_DEBUG_ASSERT((chunk_bytes & 3) == 0);

    auto wordbuff = (unsigned short*)buff;
    for(int i=0;; i++)
    {
        if(wordbuff[i] == 0)
        {
            long header_bytes = (i + 2) * 2;

            long data_bytes = 0;
            if(i > 0)
                data_bytes = (long(wordbuff[i - 1]) << 2);

            long avail_bytes = buff_bytes - header_bytes - data_bytes;
            if(avail_bytes >= chunk_bytes)
            {
                long new_offset = chunk_bytes + data_bytes;
                auto addr = (((unsigned char*)buff) + buff_bytes - new_offset);

                new_offset >>= 2;
                R64FX_DEBUG_ASSERT(new_offset <= 0x7FFF);

                wordbuff[i] = new_offset;
                wordbuff[i + 1] = 0;
                return addr;
            }
            else
            {
                return nullptr;//Buffer full!
            }
        }
        else if(wordbuff[i] & 0x8000)
        {
            long avail_bytes = wordbuff[i] & 0x7FFF;
            if(i > 0)
            {
                avail_bytes -= wordbuff[i - 1] & 0x7FFF;
            }
            avail_bytes = avail_bytes << 2;

            if(avail_bytes == chunk_bytes)
            {
                wordbuff[i] &= 0x7FFF;
                auto addr = (((unsigned char*)buff) + buff_bytes - (wordbuff[i] << 2));
                return addr;
            }
            else if(avail_bytes > chunk_bytes)
            {
                int n = i;
                while(wordbuff[n] != 0)
                    n++;
                long header_bytes = (n + 2) * 2;
                long data_bytes = (wordbuff[n - 1] << 2);
                long middle_bytes = buff_bytes - header_bytes - data_bytes;
                if(middle_bytes <= 0)
                {
                    return nullptr;//No space to grow header!
                }

                long new_offset = (long(wordbuff[i] & 0x7FFF) << 2) - avail_bytes + chunk_bytes;
                for(int j=n; j>=i; j--)
                {
                    wordbuff[j + 1] = wordbuff[j];
                }
                wordbuff[i] = (new_offset >> 2);

                auto addr = (((unsigned char*)buff) + buff_bytes - new_offset);
                return addr;
            }
        }
    }
}


bool free_chunk(void* buff, long buff_bytes, void* chunk)
{
#ifdef R64FX_DEBUG
    assert((long(buff)  & 3) == 0);
    assert((buff_bytes  & 3) == 0);
    assert((long(chunk) & 3) == 0);
#endif//R64FX_DEBUG

    if(chunk < buff)
        return false;

    auto wordbuff = (unsigned short*)buff;
    long offset = ((long(buff) + buff_bytes - long(chunk)) >> 2);
    for(int i=0;; i++)
    {
        if(wordbuff[i] == offset)
        {
            if(wordbuff[i + 1] == 0)
            {
                wordbuff[i] = 0;
                for(int j=(i - 1); j>=0 && (wordbuff[j] & 0x8000); j--)
                    wordbuff[j] = 0;
            }
            else
            {
                wordbuff[i] |= 0x8000;

                unsigned short index = i, shift = 0;
                if(i > 0 && wordbuff[i - 1] & 0x8000)
                {
                    shift += 1;
                    index--;
                }

                if(wordbuff[i + 1] & 0x8000)
                {
                    shift += 1;
                }

                if(shift)
                {
                    for(int j=index; wordbuff[j] != 0; j++)
                        wordbuff[j] = wordbuff[j + shift];
                }
            }
            return true;
        }
        else if(wordbuff[i] == 0)
        {
            return false;
        }
    }
}


HeapBuffer* HeapBuffer::newSelfHostedInstance(unsigned int npages)
{
    unsigned int nbytes = memory_page_size() * npages;
    auto buff = (unsigned short*) alloc_aligned(memory_page_size(), nbytes);
    if(!buff)
        return nullptr;
    buff[0] = 0;
    auto chunk = alloc_chunk(buff, nbytes, sizeof(HeapBuffer));
    auto obj = new(chunk) HeapBuffer(buff, nbytes);
    return obj;
}


void HeapBuffer::deleteSelfHostedInstance(HeapBuffer* buffer)
{
    auto buff = buffer->m_buffer;
    buffer->~HeapBuffer();
    free(buff);
}


void* HeapBuffer::allocChunk(long nbytes)
{
    return alloc_chunk(m_buffer, m_size, nbytes);
}


void* HeapBuffer::allocChunk(long nbytes, long alignment)
{
    R64FX_DEBUG_ASSERT(alignment >= 4);

    long alignment_mask = alignment - 1;
    R64FX_DEBUG_ASSERT((alignment & alignment_mask) == 0);

    void* chunk = allocChunk(nbytes);
    if(!chunk)
        return nullptr;

    long alignment_bytes = alignment - (long(chunk) & alignment_mask);
    if(alignment_bytes == alignment)
        return chunk;
    freeChunk(chunk);

    void* alignment_chunk = allocChunk(alignment_bytes);
    if(!alignment_chunk)
        return nullptr;

    chunk = allocChunk(nbytes);
    R64FX_DEBUG_ASSERT((long(chunk) & alignment_mask) == 0);
    freeChunk(alignment_chunk);
    return chunk;
}


bool HeapBuffer::freeChunk(void* addr)
{
    return free_chunk(m_buffer, m_size, addr);
}


bool HeapBuffer::empty() const
{
    return ((unsigned short*)m_buffer)[1] == 0;
}


unsigned int HeapBuffer::headerSize() const
{
    auto wordbuff = (unsigned short*)m_buffer;
    unsigned int i = 0;
    while(wordbuff[i++]) i++;
    return i<<1;
}


#ifdef R64FX_DEBUG
void HeapBuffer::dumpHeader()
{
    auto wordbuff = (unsigned short*)m_buffer;
    for(int i=0; wordbuff[i] != 0; i++)
        std::cout << (wordbuff[i] & 0x7FFF) << (wordbuff[i] & 0x8000 ? "*" : "") << "\n";
    std::cout << "0\n";
}
#endif//R64FX_DEBUG


void* HeapAllocator::allocChunk(long nbytes, long alignment)
{
    void* chunk = nullptr;
    for(auto buff : m_buffers)
    {
        chunk = buff->allocChunk(nbytes, alignment);
        if(chunk)
            break;
    }

    if(!chunk)
    {
        long buff_size = 0;
        while(buff_size < nbytes)
        {
            buff_size += memory_page_size();
        }
        auto buff = HeapBuffer::newSelfHostedInstance(buff_size);
        m_buffers.append(buff);
        chunk = buff->allocChunk(nbytes, alignment);
    }

    return chunk;
}


void HeapAllocator::freeChunk(void* addr)
{
    HeapBuffer* buff_to_remove = nullptr;
    for(auto buff : m_buffers)
    {
        if(buff->freeChunk(addr))
        {
            if(buff->empty())
                buff_to_remove = buff;
            break;
        }
    }

    if(buff_to_remove)
    {
        m_buffers.remove(buff_to_remove);
        HeapBuffer::deleteSelfHostedInstance(buff_to_remove);
    }
}


bool HeapAllocator::empty() const
{
    return m_buffers.empty();
}


#ifdef R64FX_DEBUG
void HeapAllocator::dumpHeaders()
{
    int i=0;
    for(auto buff : m_buffers)
    {
        i++;
        std::cout << "HeapBuffer: " << i << "\n";
        buff->dumpHeader();
        std::cout << "\n";
    }
}


void HeapAllocator::debugAddr(void* addr)
{
    int i=0;
    for(auto buff : m_buffers)
    {
        i++;
        if(addr < buff->buffer())
            continue;

        auto uchaddr = (unsigned char*) addr;
        auto uchbuff = (unsigned char*) buff->buffer();
        if(uchaddr >= uchbuff && uchaddr < (uchbuff + buff->size()))
        {
            std::cout << "buff: " << i << " -> " << ((buff->size() - long(uchaddr - uchbuff)) >> 2) << "\n";
            break;
        }
    }
}
#endif//R64FX_DEBUG

}//namespace r64fx
