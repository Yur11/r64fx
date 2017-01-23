#include "MemoryUtils.hpp"
#include <unistd.h>
#include <stdlib.h>
#include <new>

#ifdef R64FX_DEBUG
#include <assert.h>
#include <iostream>
#endif//R64FX_DEBUG

namespace r64fx{

int memory_page_size()
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
#ifdef R64FX_DEBUG
    assert((long(buff)  % 8) == 0);
    assert((buff_bytes  % 8) == 0);
    assert((chunk_bytes % 8) == 0);
#endif//R64FX_DEBUG

    auto wordbuff = (unsigned short*)buff;
    for(int i=0;; i++)
    {
        if(wordbuff[i] == 0)
        {
            long header_bytes = (i + 2) * 2;

            long data_bytes = 0;
            if(i > 0)
                data_bytes = (long(wordbuff[i - 1]) << 3);

            long avail_bytes = buff_bytes - header_bytes - data_bytes;
            if(avail_bytes >= chunk_bytes)
            {
                long new_offset = chunk_bytes + data_bytes;
#ifdef R64FX_DEBUG
                assert((new_offset >> 3) < 32767);
#endif//R64FX_DEBUG
                wordbuff[i] = (new_offset >> 3);
                wordbuff[i + 1] = 0;
                auto addr = (((unsigned char*)buff) + buff_bytes - new_offset);
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
            avail_bytes = avail_bytes << 3;

            if(avail_bytes == chunk_bytes)
            {
                wordbuff[i] &= 0x7FFF;
                auto addr = (((unsigned char*)buff) + buff_bytes - (wordbuff[i] << 3));
                return addr;
            }
            else if(avail_bytes > chunk_bytes)
            {
                int n = i;
                while(wordbuff[n] != 0)
                    n++;
                long header_bytes = (n + 2) * 2;
                long data_bytes = (wordbuff[n - 1] << 3);
                long middle_bytes = buff_bytes - header_bytes - data_bytes;
                if(middle_bytes <= 0)
                {
                    return nullptr;//No space to grow header!
                }

                long new_offset = (long(wordbuff[i] & 0x7FFF) << 3) - avail_bytes + chunk_bytes;
                for(int j=n; j>=i; j--)
                {
                    wordbuff[j + 1] = wordbuff[j];
                }
                wordbuff[i] = (new_offset >> 3);

                auto addr = (((unsigned char*)buff) + buff_bytes - new_offset);
                return addr;
            }
        }
    }
}


bool free_chunk(void* buff, long buff_bytes, void* chunk)
{
#ifdef R64FX_DEBUG
    assert((long(buff)  % 8) == 0);
    assert((buff_bytes  % 8) == 0);
    assert((long(chunk) % 8) == 0);
#endif//R64FX_DEBUG

    if(chunk < buff)
        return false;

    auto wordbuff = (unsigned short*)buff;
    long offset = ((long(buff) + buff_bytes - long(chunk)) >> 3);
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


HeapBuffer* HeapBuffer::newInstance(long nbytes)
{
    auto buff = (unsigned short*) alloc_aligned(nbytes, nbytes);
    if(!buff)
        return nullptr;
    buff[0] = 0;
    auto chunk = alloc_chunk(buff, nbytes, sizeof(HeapBuffer));
    auto obj = new(chunk) HeapBuffer(buff, nbytes);
    return obj;
}


void HeapBuffer::deleteInstance(HeapBuffer* buffer)
{
    auto buff = buffer->m_buffer;
    buffer->~HeapBuffer();
    ::free(buff);
}


void* HeapBuffer::alloc(long nbytes)
{
    return alloc_chunk(m_buffer, m_size, nbytes);
}


bool HeapBuffer::free(void* addr)
{
    return free_chunk(m_buffer, m_size, addr);
}


bool HeapBuffer::isEmpty() const
{
    return ((unsigned short*)m_buffer)[1] == 0;
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


void* HeapAllocator::alloc(long nbytes)
{
    void* chunk = nullptr;
    for(auto buff : m_buffers)
    {
        chunk = buff->alloc(nbytes);
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
        auto buff = HeapBuffer::newInstance(buff_size);
        m_buffers.append(buff);
        chunk = buff->alloc(nbytes);
    }

    return chunk;
}


void HeapAllocator::free(void* addr)
{
    HeapBuffer* buff_to_remove = nullptr;
    for(auto buff : m_buffers)
    {        
        if(buff->free(addr))
        {
            if(buff->isEmpty())
                buff_to_remove = buff;
            break;
        }
    }

    if(buff_to_remove)
    {
        m_buffers.remove(buff_to_remove);
        HeapBuffer::deleteInstance(buff_to_remove);
    }
}


bool HeapAllocator::isEmpty() const
{
    return m_buffers.isEmpty();
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
            std::cout << "buff: " << i << " -> " << ((buff->size() - long(uchaddr - uchbuff)) >> 3) << "\n";
            break;
        }
    }
}
#endif//R64FX_DEBUG

}//namespace r64fx
