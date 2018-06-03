#include "test.hpp"
#include "MemoryUtils.hpp"
#include <iostream>

#define R64FX_CHECK_ALIGNMENT(addr) if((long(addr) % 8) != 0){ cout << "Alignment Fail!\n"; return false; }

#define R64FX_CHECK_HEADER(...)\
{\
    unsigned short buff[] = { __VA_ARGS__ };\
    if(!cmp_buffs<unsigned short>(hb->buffer(), buff, sizeof(buff)/2))\
    {\
        cout << "Header Failure!\n";\
        return false;\
    }\
}\

#define R64FX_CHECK_DATA(offset, ref, nbytes)\
{\
    if(!cmp_buffs<unsigned char>(buffend - sizeof(HeapBuffer) - (offset), (ref), (nbytes)))\
    {\
        cout << "Data Failure!\n";\
        return false;\
    }\
}\


using namespace r64fx;
using namespace std;


template<typename T> bool cmp_buffs(void* a, void* b, long nitems)
{
    auto aitems = (T*) a;
    auto bitems = (T*) b;
    for(int i=0; i<nitems; i++)
    {
        if(aitems[i] != bitems[i])
            return false;
    }
    return true;
}


bool test_MemoryBuffer()
{
    cout << "Testing MemoryBuffer\n";

    MemoryBuffer mb(1);
    R64FX_EXPECT_EQ((long)mb.begin(), (long)mb.ptr());
    R64FX_EXPECT_EQ((long)memory_page_size(), (long)mb.nbytes());

    mb.grow(123);
    R64FX_EXPECT_EQ((long)mb.begin() + 123, (long)mb.ptr());
    R64FX_EXPECT_EQ(1UL, mb.npages());

    mb.grow(memory_page_size());
    R64FX_EXPECT_EQ(2UL, mb.npages());
    R64FX_EXPECT_EQ((long)mb.begin() + memory_page_size() + 123, (long)mb.ptr());
    R64FX_EXPECT_EQ((long)memory_page_size() - 123, (long)mb.bytesAvail());

    mb.resize(0);
    R64FX_EXPECT_EQ(0UL, mb.nbytes());

    return true;
}


bool test_DivergingBuffers()
{
    cout << "Testing DivergingBuffers\n";

    DivergingBuffers db(1, 1);
    R64FX_EXPECT_EQ((long)db.decrPtr(), (long)db.incrPtr());

    db.growDecr(123);
    R64FX_EXPECT_EQ((long)db.decrEnd() - 123, (long)db.decrPtr());

    db.growIncr(10);
    R64FX_EXPECT_EQ((long)db.incrPtr() - 133, (long)db.decrPtr());

    db.ensure(memory_page_size(), memory_page_size());
    R64FX_EXPECT_EQ((long)(memory_page_size() * 4), (long)(db.incrEnd() - db.decrBegin()));

    return true;
}


bool test_HeapBuffer(HeapBuffer* hb)
{
    auto buffuchar = (unsigned char*) hb->buffer();
    auto buffend = buffuchar + hb->size();

    cout << "Testing HeapBuffer\n";
    {
        if(!cmp_buffs<unsigned char>(buffend - sizeof(HeapBuffer), hb, sizeof(HeapBuffer)))
            return false;
        R64FX_CHECK_HEADER(sizeof(HeapBuffer)/4, 0);
    }

    unsigned long num1 = 0x123456789ABCDEF;
    unsigned long num2 = 0xFEDCBA987654321;
    unsigned long num3 = 0x7654321FEDCBA98;
    unsigned long num4 = 0xFEDFEDFEDCAB123;

    cout << "alloc a\n";
    auto a = (unsigned long*) hb->allocChunk(8);
    {
        R64FX_CHECK_ALIGNMENT(a);
        *a = num1;
        R64FX_CHECK_DATA(8, &num1, 8);
        R64FX_CHECK_HEADER(
            sizeof(HeapBuffer)/4,     //self
            sizeof(HeapBuffer)/4 + 2, //a
            0
        );
    }

    cout << "alloc b\n";
    auto b = (unsigned long*) hb->allocChunk(16);
    {
        R64FX_CHECK_ALIGNMENT(b);
        b[0] = num2;
        b[1] = num3;
        unsigned long buff[2] = {num2, num3};
        R64FX_CHECK_DATA(24, buff, 16);
        R64FX_CHECK_HEADER(
            sizeof(HeapBuffer)/4,     //self
            sizeof(HeapBuffer)/4 + 2, //a
            sizeof(HeapBuffer)/4 + 6, //b
            0
        );
    }

    cout << "alloc c\n";
    auto c = (unsigned long*) hb->allocChunk(8);
    {
        R64FX_CHECK_ALIGNMENT(c);
        *c = num4;
        R64FX_CHECK_DATA(32, &num4, 8);
        R64FX_CHECK_HEADER(
            sizeof(HeapBuffer)/4,     //self
            sizeof(HeapBuffer)/4 + 2, //a
            sizeof(HeapBuffer)/4 + 6, //b
            sizeof(HeapBuffer)/4 + 8, //c
            0
        );
    }

    cout << "free b\n";
    hb->freeChunk(b); b = nullptr;
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/4,                 //self
        sizeof(HeapBuffer)/4 + 2,             //a
        (sizeof(HeapBuffer)/4 + 6) | 0x8000,  //free
        sizeof(HeapBuffer)/4 + 8,             //c
        0
    );

    cout << "free c\n";
    hb->freeChunk(c); c = nullptr;
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/4,     //self
        sizeof(HeapBuffer)/4 + 2, //a
        0
    );

    cout << "realloc b\n";
    b = (unsigned long*) hb->allocChunk(24);
    R64FX_CHECK_ALIGNMENT(b);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/4,     //self
        sizeof(HeapBuffer)/4 + 2, //a
        sizeof(HeapBuffer)/4 + 8, //b
        0
    );

    cout << "realloc c\n";
    c = (unsigned long*) hb->allocChunk(16);
    R64FX_CHECK_ALIGNMENT(c);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/4,      //self
        sizeof(HeapBuffer)/4 + 2,  //a
        sizeof(HeapBuffer)/4 + 8,  //b
        sizeof(HeapBuffer)/4 + 12, //c
        0
    );

    cout << "alloc z\n";
    auto z = (unsigned long*) hb->allocChunk(8);
    R64FX_CHECK_ALIGNMENT(z);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/4,      //self
        sizeof(HeapBuffer)/4 + 2,  //a
        sizeof(HeapBuffer)/4 + 8,  //b
        sizeof(HeapBuffer)/4 + 12, //c
        sizeof(HeapBuffer)/4 + 14, //z
        0
    );

    cout << "free a\n";
    hb->freeChunk(a);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/4,                 //self
        (sizeof(HeapBuffer)/4 + 2) | 0x8000,  //free
        sizeof(HeapBuffer)/4 + 8,             //b
        sizeof(HeapBuffer)/4 + 12,            //c
        sizeof(HeapBuffer)/4 + 14,            //z
        0
    );

    cout << "double free a\n";
    if(hb->freeChunk(a))
    {
        cout << "Failed!\n";
        return false;
    }
    a = nullptr;

    cout << "free c\n";
    hb->freeChunk(c); c = nullptr;
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/4,                 //self
        (sizeof(HeapBuffer)/4 + 2) | 0x8000,  //free
        sizeof(HeapBuffer)/4 + 8,             //b
        (sizeof(HeapBuffer)/4 + 12) | 0x8000, //free
        sizeof(HeapBuffer)/4 + 14,            //z
        0
    );

    cout << "free b (Merge Free Space)\n";
    hb->freeChunk(b); b = nullptr;
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/4,                 //self
        (sizeof(HeapBuffer)/4 + 12) | 0x8000, //free
        sizeof(HeapBuffer)/4 + 14,            //z
        0
    );

    cout << "realloc a\n";
    a = (unsigned long*) hb->allocChunk(8);
    R64FX_CHECK_ALIGNMENT(a);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/4,                 //self
        sizeof(HeapBuffer)/4 + 2,             //a
        (sizeof(HeapBuffer)/4 + 12) | 0x8000, //free
        sizeof(HeapBuffer)/4 + 14,            //z
        0
    );

    cout << "realloc b\n";
    b = (unsigned long*) hb->allocChunk(8);
    R64FX_CHECK_ALIGNMENT(b);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/4,                 //self
        sizeof(HeapBuffer)/4 + 2,             //a
        sizeof(HeapBuffer)/4 + 4,             //b
        (sizeof(HeapBuffer)/4 + 12) | 0x8000, //free
        sizeof(HeapBuffer)/4 + 14,            //z
        0
    );

    cout << "realloc c\n";
    c = (unsigned long*) hb->allocChunk(8);
    R64FX_CHECK_ALIGNMENT(c);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/4,                 //self
        sizeof(HeapBuffer)/4 + 2,             //a
        sizeof(HeapBuffer)/4 + 4,             //b
        sizeof(HeapBuffer)/4 + 6,             //c
        (sizeof(HeapBuffer)/4 + 12) | 0x8000, //free
        sizeof(HeapBuffer)/4 + 14,            //z
        0
    );

    cout << "alloc d\n";
    auto d = (unsigned long*) hb->allocChunk(8);
    R64FX_CHECK_ALIGNMENT(d);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/4,                 //self
        sizeof(HeapBuffer)/4 + 2,             //a
        sizeof(HeapBuffer)/4 + 4,             //b
        sizeof(HeapBuffer)/4 + 6,             //c
        sizeof(HeapBuffer)/4 + 8,             //d
        (sizeof(HeapBuffer)/4 + 12) | 0x8000, //free
        sizeof(HeapBuffer)/4 + 14,            //z
        0
    );

    cout << "alloc e\n";
    auto e = (unsigned long*) hb->allocChunk(8);
    R64FX_CHECK_ALIGNMENT(e);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/4,                 //self
        sizeof(HeapBuffer)/4 + 2,             //a
        sizeof(HeapBuffer)/4 + 4,             //b
        sizeof(HeapBuffer)/4 + 6,             //c
        sizeof(HeapBuffer)/4 + 8,             //d
        sizeof(HeapBuffer)/4 + 10,            //e
        (sizeof(HeapBuffer)/4 + 12) | 0x8000, //free
        sizeof(HeapBuffer)/4 + 14,            //z
        0
    );

    cout << "alloc f\n";
    auto f = (unsigned long*) hb->allocChunk(8);
    R64FX_CHECK_ALIGNMENT(f);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/4,      //self
        sizeof(HeapBuffer)/4 + 2,  //a
        sizeof(HeapBuffer)/4 + 4,  //b
        sizeof(HeapBuffer)/4 + 6,  //c
        sizeof(HeapBuffer)/4 + 8,  //d
        sizeof(HeapBuffer)/4 + 10, //e
        sizeof(HeapBuffer)/4 + 12, //f
        sizeof(HeapBuffer)/4 + 14, //z
        0
    );

    cout << "free b & d\n";
    hb->freeChunk(b); b = nullptr;
    hb->freeChunk(d); d = nullptr;
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/4,                 //self
        sizeof(HeapBuffer)/4 + 2,             //a
        (sizeof(HeapBuffer)/4 + 4) | 0x8000,  //free
        sizeof(HeapBuffer)/4 + 6,             //c
        (sizeof(HeapBuffer)/4 + 8) | 0x8000,  //free
        sizeof(HeapBuffer)/4 + 10,            //e
        sizeof(HeapBuffer)/4 + 12,            //f
        sizeof(HeapBuffer)/4 + 14,            //z
        0
    );

    cout << "free a (Merge Down)\n";
    hb->freeChunk(a); a = nullptr;
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/4,                 //self
        (sizeof(HeapBuffer)/4 + 4) | 0x8000,  //free
        sizeof(HeapBuffer)/4 + 6,             //c
        (sizeof(HeapBuffer)/4 + 8) | 0x8000,  //free
        sizeof(HeapBuffer)/4 + 10,            //e
        sizeof(HeapBuffer)/4 + 12,            //f
        sizeof(HeapBuffer)/4 + 14,            //z
        0
    );

    cout << "free e (Merge Up)\n";
    hb->freeChunk(e); e = nullptr;
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/4,                 //self
        (sizeof(HeapBuffer)/4 + 4) | 0x8000,  //free
        sizeof(HeapBuffer)/4 + 6,             //c
        (sizeof(HeapBuffer)/4 + 10) | 0x8000, //free
        sizeof(HeapBuffer)/4 + 12,            //f
        sizeof(HeapBuffer)/4 + 14,            //z
        0
    );

    cout << "free nullptr\n";
    {
        if(hb->freeChunk(a))
        {
            cout << "Failed!\n";
            return false;
        }
    }

    cout << "Alloc Too Much\n";
    {
        if(hb->allocChunk(hb->size()) != nullptr)
        {
            cout << "Failed!\n";
            return false;
        }
    }

    cout << "free z\n";
    hb->freeChunk(z); z = nullptr;
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/4,                 //self
        (sizeof(HeapBuffer)/4 + 4) | 0x8000,  //free
        sizeof(HeapBuffer)/4 + 6,             //c
        (sizeof(HeapBuffer)/4 + 10) | 0x8000, //free
        sizeof(HeapBuffer)/4 + 12,            //f
        0
    );

    cout << "realloc a\n";
    a = (unsigned long*) hb->allocChunk(8);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/4,                 //self
        sizeof(HeapBuffer)/4 + 2,             //a
        (sizeof(HeapBuffer)/4 + 4) | 0x8000,  //free
        sizeof(HeapBuffer)/4 + 6,             //c
        (sizeof(HeapBuffer)/4 + 10) | 0x8000, //free
        sizeof(HeapBuffer)/4 + 12,            //f
        0
    );

    cout << "realloc e\n";
    e = (unsigned long*) hb->allocChunk(16);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/4,                 //self
        sizeof(HeapBuffer)/4 + 2,             //a
        (sizeof(HeapBuffer)/4 + 4) | 0x8000,  //free
        sizeof(HeapBuffer)/4 + 6,             //c
        sizeof(HeapBuffer)/4 + 10,            //e
        sizeof(HeapBuffer)/4 + 12,            //f
        0
    );

    cout << "free c\n";
    hb->freeChunk(c); c = nullptr;
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/4,                 //self
        sizeof(HeapBuffer)/4 + 2,             //a
        (sizeof(HeapBuffer)/4 + 6) | 0x8000,  //free
        sizeof(HeapBuffer)/4 + 10,            //e
        sizeof(HeapBuffer)/4 + 12,            //f
        0
    );

    cout << "free a, e, f\n";
    hb->freeChunk(a); a = nullptr;
    hb->freeChunk(e); e = nullptr;
    hb->freeChunk(f); f = nullptr;
    R64FX_CHECK_HEADER(sizeof(HeapBuffer)/4, 0);

    cout << "alloc 4 bytes\n";
    auto dword1 = (int*)hb->allocChunk(4);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/4,
        sizeof(HeapBuffer)/4 + 1,
        0
    );

    cout << "free all\n";
    hb->freeChunk(dword1);
    R64FX_CHECK_HEADER(sizeof(HeapBuffer)/4, 0);

    cout << "chunk size\n";
    {
        int size_a = (((rand() & 0xF) + 1) << 2);
        int size_b = (((rand() & 0xF) + 1) << 2);
        int size_c = (((rand() & 0xF) + 1) << 2);
        int size_d = (((rand() & 0xF) + 1) << 2);
        auto a = hb->allocChunk(size_a);
        auto b = hb->allocChunk(size_b);
        auto c = hb->allocChunk(size_c);
        auto d = hb->allocChunk(size_d);
        hb->freeChunk(c);
        R64FX_EXPECT_EQ(size_a, hb->chunkSize(a));
        R64FX_EXPECT_EQ(size_b, hb->chunkSize(b));
        R64FX_EXPECT_EQ(size_c, hb->chunkSize(c));
        R64FX_EXPECT_EQ(size_d, hb->chunkSize(d));

        hb->freeChunk(a);
        hb->freeChunk(b);
        hb->freeChunk(d);
        R64FX_CHECK_HEADER(sizeof(HeapBuffer)/4, 0);
    }

    cout << "align 4\n";
    a = (unsigned long*) hb->allocChunk(4, 4);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/4,
        sizeof(HeapBuffer)/4 + 1,
        0
    );

    cout << "align 8\n";
    b = (unsigned long*) hb->allocChunk(8, 8);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/4,
        sizeof(HeapBuffer)/4 + 1,
        (sizeof(HeapBuffer)/4 + 2) | 0x8000,
        sizeof(HeapBuffer)/4 + 4,
        0
    );

    cout << "align 64\n";
    c = (unsigned long*) hb->allocChunk(1024, 64);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/4,
        sizeof(HeapBuffer)/4 + 1,
        (sizeof(HeapBuffer)/4 + 2) | 0x8000,
        sizeof(HeapBuffer)/4 + 4,
        (sizeof(HeapBuffer)/4 + 8) | 0x8000,
        sizeof(HeapBuffer)/4 + 264,
        0
    );

    return true;
}


bool testha(HeapAllocator* ha, int depth, HeapAllocator* freeha = nullptr, unsigned long* freeptr = nullptr)
{
    static const int maxbufs    = 8;
    static const int maxbuflen  = memory_page_size() / 16;

    if(depth == 0)
        return true;

    int nbufs = (rand() % maxbufs) + 1;
    unsigned long*  ptr[maxbufs];
    unsigned long   buf[maxbufs][maxbuflen];
    int             len[maxbufs];
    for(int i=0; i<nbufs; i++)
    {
        len[i] = ((rand() % maxbuflen) + 1);
        ptr[i] = (unsigned long*) ha->allocChunk(len[i] * 8, 8);
        if(!ptr[i])
        {
            cout << "\nAllocation Failed!\n";
            return false;
        }

        for(int j=0; j<len[i]; j++)
        {
            buf[i][j] = (unsigned long)rand();
            ptr[i][j] = buf[i][j];
        }
    }

    HeapAllocator tmpha;
    int act = rand() % 4;
    switch(act)
    {
        case 0:
        {
            for(int i=0; i<8; i++)
            {
                if(!testha(ha, depth - 1))
                {
                    cout << "0 Failed!\n";
                    return false;
                }
            }
            break;
        }

        case 1:
        {
            for(int i=0; i<nbufs; i++)
            {
                int extraoffset = rand() % 16;
                int extralen = rand() % 16;

                auto tmpbuf = (unsigned long*) ha->allocChunk((len[i] + extralen + extraoffset) * 8, 8);
                if(!ptr[i])
                {
                    cout << "\n1.1 Allocation Failed!\n";
                    return false;
                }
                for(int j=0; j<len[i]; j++)
                {
                    tmpbuf[j + extraoffset] = ptr[i][j];
                }

                ha->freeChunk(ptr[i]);
                ptr[i] = nullptr;

                if(!testha(ha, depth - 1))
                    return false;

                ha->freeChunk(ptr[i]);
                ptr[i] = (unsigned long*) ha->allocChunk(len[i] * 8, 8);
                if(!ptr[i])
                {
                    cout << "\n1.2 Reallocation Failed!\n";
                    return false;
                }
                for(int j=0; j<len[i]; j++)
                {
                    ptr[i][j] = tmpbuf[j + extraoffset];
                }
                ha->freeChunk(tmpbuf);

                for(int j=0; j<len[i]; j++)
                {
                    if(ptr[i][j] != buf[i][j])
                    {
                        cout << "\n1.3 Data Corrupted!\n";
                        return false;
                    }
                }
            }
            break;
        }

        case 2:
        {
            if(!testha(&tmpha, depth - 1))
            {
                cout << "2 Failed!\n";
                return false;
            }
            break;
        }

        case 3:
        {
            int extralen = 16 + (rand() % 64);
            auto extrabuf = (unsigned long*) ha->allocChunk(extralen * 8, 8);
            if(depth > 1)
            {
                if(!testha(ha, depth - 1, ha, extrabuf))
                {
                    cout << "3.1 Failed!\n";
                    return false;
                }
            }
            else
            {
                ha->freeChunk(extrabuf);
            }
            break;
        }

        default:
            break;
    }

    if(freeha && freeptr)
    {
        freeha->freeChunk(freeptr);
    }

    for(int i=0; i<nbufs; i++)
    {
        for(int j=0; j<len[i]; j++)
        {
            if(ptr[i][j] != buf[i][j])
            {
                cout << "\nData Corrupted!\n";
                return false;
            }
        }
        ha->freeChunk(ptr[i]);
    }

    return true;
}


bool test_HeapAllocator(HeapAllocator* ha)
{
    cout << "Testing HeapAllocator";
    cout.flush();
    for(int n=0; n<32; n++)
    {
        cout << '.';
        cout.flush();

        if(!testha(ha, 8))
        {
            return false;
        }

        if(!ha->empty())
        {
            cout << "\nHeapAllocator not empty!\n";
            return false;
        }
    }

    cout << "\n";
    return true;
}


int main()
{
    srand(time(nullptr));

    auto ok = true;

    ok = ok && test_MemoryBuffer();
    ok = ok && test_DivergingBuffers();

    auto hb = HeapBuffer::newSelfHostedInstance(1);
    ok = ok && test_HeapBuffer(hb);
    HeapBuffer::deleteSelfHostedInstance(hb);

    auto ha = new HeapAllocator;
    ok = ok && test_HeapAllocator(ha);
    delete ha;

    if(ok)
    {
        cout << "OK!\n";
        return 0;
    }
    else
    {
        cout << "Fail!\n";
        return 1;
    }
}
