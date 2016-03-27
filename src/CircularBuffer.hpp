#ifndef R64FX_CIRCULAR_BUFFER_HPP
#define R64FX_CIRCULAR_BUFFER_HPP

#include <new>
#include <cstring>

namespace r64fx{

template<typename T> void copy_items(T* dst, const T* src, int nitems)
{
    memcpy(dst, src, nitems * sizeof(T));
}


template<typename T> class CircularBuffer{
    T*            m_buffer = nullptr;

    // Must be >= 2
    unsigned int  m_size   = 0;

    // Read and Write pointers.
    //
    // MSBs are used as flags to distinguish
    // between full and empty states
    // when read pointer is equal to write pointer.
    volatile unsigned int mw = 0;
    volatile unsigned int mr = 0;

public:
    CircularBuffer(int size, T* storage = nullptr)
    {
        if(size < 2)
            return;

        if(storage)
        {
            m_buffer = storage;
        }
        else
        {
            m_buffer = new(std::nothrow) T[size];
            if(!m_buffer)
                return;
        }

        m_size = size;
        mw = mr = 0;
    }


    int size() const
    {
        return m_size;
    }

    T &operator[](int i) const
    {
        return m_buffer[i];
    }


    inline int write(const T& input)
    {
        return write(&input, 1);
    }


    int write(const T* input, int nitems)
    {
        volatile unsigned int w = mw;
        volatile unsigned int r = mr;

        // Extract pointers.
        unsigned int wptr = w & 0x7FFFFFFF;
        unsigned int rptr = r & 0x7FFFFFFF;

        // Extract flags.
        unsigned int ws = w & 0x80000000;
        unsigned int rs = r & 0x80000000;

        if(wptr == rptr && ws != rs) // Buffer is full! Nowhere to write!
        {
            return 0;
        }

        bool free_space_is_split = (wptr >= rptr);
        int avail = m_size - (free_space_is_split ? (wptr - rptr) : (m_size - rptr + wptr));

        if(nitems >= avail) // Write saturation! Update write flag.
        {
            nitems = avail;
            ws = (~rs) & 0x80000000;
        }
        else // No saturation!
        {
            ws = 0;
        }

        if(free_space_is_split)
        {
            int tail_chunk_size = m_size - wptr;
            if(nitems <= tail_chunk_size)
            {
                copy_items(m_buffer + wptr, input, nitems);
                wptr += nitems;
                if(wptr == m_size)
                    wptr = 0;
            }
            else
            {
                copy_items(m_buffer + wptr, input, tail_chunk_size);

                int head_chunk_size = nitems - tail_chunk_size;
                copy_items(m_buffer, input + tail_chunk_size, head_chunk_size);
                wptr = head_chunk_size;
            }
        }
        else // Free space is in the middle.
        {
            copy_items(m_buffer + wptr, input, nitems);
            wptr += nitems;
        }

        w = ws | wptr;
        mw = w;

        return nitems;
    }


    int read(T* output, int nitems)
    {
        volatile unsigned int w = mw;
        volatile unsigned int r = mr;

        if(w == r) // Buffer is empty! Nothing to read!
            return 0;

        // Extract pointers.
        unsigned int wptr = w & 0x7FFFFFFF;
        unsigned int rptr = r & 0x7FFFFFFF;

        // Extract flags.
        unsigned int ws = w & 0x80000000;
        unsigned int rs = r & 0x80000000;

        bool data_is_split = (rptr >= wptr);
        int avail = (data_is_split ? (m_size - rptr + wptr) : (wptr - rptr));

        // Update read flag if needed.
        if(nitems >= avail) // Read saturation!
        {
            nitems = avail;
            rs = ws;
        }
        else // No saturation!
        {
            rs = 0;
        }

        if(data_is_split)
        {
            int tail_chunk_size = m_size - rptr;
            if(nitems <= tail_chunk_size)
            {
                copy_items(output, m_buffer + rptr, nitems);
                rptr += nitems;
                if(rptr == m_size)
                    rptr = 0;
            }
            else
            {
                copy_items(output, m_buffer + rptr, tail_chunk_size);

                int head_chunk_size = nitems - tail_chunk_size;
                copy_items(output + tail_chunk_size, m_buffer, head_chunk_size);
                rptr = head_chunk_size;
            }
        }
        else // Data is in the middle.
        {
            copy_items(output, m_buffer + rptr, nitems);
            rptr += nitems;
        }

        r = rs | rptr;
        mr = r;

        return nitems;
    }
};

}//namespace r64fx

#endif//R64FX_CIRCULAR_BUFFER_HPP