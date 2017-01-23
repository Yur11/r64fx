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


bool test_HeapBuffer(HeapBuffer* hb)
{
    auto buffuchar = (unsigned char*) hb->buffer();
    auto buffend = buffuchar + hb->size();

    cout << "Testing HeapBuffer\n";
    {
        if(!cmp_buffs<unsigned char>(buffend - sizeof(HeapBuffer), hb, sizeof(HeapBuffer)))
            return false;
        R64FX_CHECK_HEADER(sizeof(HeapBuffer)/8, 0);
    }

    unsigned long num1 = 0x123456789ABCDEF;
    unsigned long num2 = 0xFEDCBA987654321;
    unsigned long num3 = 0x7654321FEDCBA98;
    unsigned long num4 = 0xFEDFEDFEDCAB123;

    cout << "alloc a\n";
    auto a = (unsigned long*) hb->alloc(8);
    {
        R64FX_CHECK_ALIGNMENT(a);
        *a = num1;
        R64FX_CHECK_DATA(8, &num1, 8);
        R64FX_CHECK_HEADER(
            sizeof(HeapBuffer)/8,     //self
            sizeof(HeapBuffer)/8 + 1, //a
            0
        );
    }

    cout << "alloc b\n";
    auto b = (unsigned long*) hb->alloc(16);
    {
        R64FX_CHECK_ALIGNMENT(b);
        b[0] = num2;
        b[1] = num3;
        unsigned long buff[2] = {num2, num3};
        R64FX_CHECK_DATA(24, buff, 16);
        R64FX_CHECK_HEADER(
            sizeof(HeapBuffer)/8,     //self
            sizeof(HeapBuffer)/8 + 1, //a
            sizeof(HeapBuffer)/8 + 3, //b
            0
        );
    }

    cout << "alloc c\n";
    auto c = (unsigned long*) hb->alloc(8);
    {
        R64FX_CHECK_ALIGNMENT(c);
        *c = num4;
        R64FX_CHECK_DATA(32, &num4, 8);
        R64FX_CHECK_HEADER(
            sizeof(HeapBuffer)/8,     //self
            sizeof(HeapBuffer)/8 + 1, //a
            sizeof(HeapBuffer)/8 + 3, //b
            sizeof(HeapBuffer)/8 + 4, //c
            0
        );
    }

    cout << "free b\n";
    hb->free(b); b = nullptr;
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/8,                 //self
        sizeof(HeapBuffer)/8 + 1,             //a
        (sizeof(HeapBuffer)/8 + 3) | 0x8000,  //free
        sizeof(HeapBuffer)/8 + 4,             //c
        0
    );

    cout << "free c\n";
    hb->free(c); c = nullptr;
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/8,     //self
        sizeof(HeapBuffer)/8 + 1, //a
        0
    );

    cout << "realloc b\n";
    b = (unsigned long*) hb->alloc(24);
    R64FX_CHECK_ALIGNMENT(b);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/8,     //self
        sizeof(HeapBuffer)/8 + 1, //a
        sizeof(HeapBuffer)/8 + 4, //b
        0
    );

    cout << "realloc c\n";
    c = (unsigned long*) hb->alloc(16);
    R64FX_CHECK_ALIGNMENT(c);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/8,     //self
        sizeof(HeapBuffer)/8 + 1, //a
        sizeof(HeapBuffer)/8 + 4, //b
        sizeof(HeapBuffer)/8 + 6, //c
        0
    );

    cout << "alloc z\n";
    auto z = (unsigned long*) hb->alloc(8);
    R64FX_CHECK_ALIGNMENT(z);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/8,     //self
        sizeof(HeapBuffer)/8 + 1, //a
        sizeof(HeapBuffer)/8 + 4, //b
        sizeof(HeapBuffer)/8 + 6, //c
        sizeof(HeapBuffer)/8 + 7, //z
        0
    );

    cout << "free a\n";
    hb->free(a);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/8,                //self
        (sizeof(HeapBuffer)/8 + 1) | 0x8000, //free
        sizeof(HeapBuffer)/8 + 4,            //b
        sizeof(HeapBuffer)/8 + 6,            //c
        sizeof(HeapBuffer)/8 + 7,            //z
        0
    );

    cout << "double free a\n";
    if(hb->free(a))
    {
        cout << "Failed!\n";
        return false;
    }
    a = nullptr;

    cout << "free c\n";
    hb->free(c); c = nullptr;
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/8,                //self
        (sizeof(HeapBuffer)/8 + 1) | 0x8000, //free
        sizeof(HeapBuffer)/8 + 4,            //b
        (sizeof(HeapBuffer)/8 + 6) | 0x8000, //free
        sizeof(HeapBuffer)/8 + 7,            //z
        0
    );

    cout << "free b (Merge Free Space)\n";
    hb->free(b); b = nullptr;
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/8,                //self
        (sizeof(HeapBuffer)/8 + 6) | 0x8000, //free
        sizeof(HeapBuffer)/8 + 7,            //z
        0
    );

    cout << "realloc a\n";
    a = (unsigned long*) hb->alloc(8);
    R64FX_CHECK_ALIGNMENT(a);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/8,                //self
        sizeof(HeapBuffer)/8 + 1,            //a
        (sizeof(HeapBuffer)/8 + 6) | 0x8000, //free
        sizeof(HeapBuffer)/8 + 7,            //z
        0
    );

    cout << "realloc b\n";
    b = (unsigned long*) hb->alloc(8);
    R64FX_CHECK_ALIGNMENT(b);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/8,                //self
        sizeof(HeapBuffer)/8 + 1,            //a
        sizeof(HeapBuffer)/8 + 2,            //b
        (sizeof(HeapBuffer)/8 + 6) | 0x8000, //free
        sizeof(HeapBuffer)/8 + 7,            //z
        0
    );

    cout << "realloc c\n";
    c = (unsigned long*) hb->alloc(8);
    R64FX_CHECK_ALIGNMENT(c);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/8,                //self
        sizeof(HeapBuffer)/8 + 1,            //a
        sizeof(HeapBuffer)/8 + 2,            //b
        sizeof(HeapBuffer)/8 + 3,            //c
        (sizeof(HeapBuffer)/8 + 6) | 0x8000, //free
        sizeof(HeapBuffer)/8 + 7,            //z
        0
    );

    cout << "alloc d\n";
    auto d = (unsigned long*) hb->alloc(8);
    R64FX_CHECK_ALIGNMENT(d);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/8,                //self
        sizeof(HeapBuffer)/8 + 1,            //a
        sizeof(HeapBuffer)/8 + 2,            //b
        sizeof(HeapBuffer)/8 + 3,            //c
        sizeof(HeapBuffer)/8 + 4,            //d
        (sizeof(HeapBuffer)/8 + 6) | 0x8000, //free
        sizeof(HeapBuffer)/8 + 7,            //z
        0
    );

    cout << "alloc e\n";
    auto e = (unsigned long*) hb->alloc(8);
    R64FX_CHECK_ALIGNMENT(e);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/8,                //self
        sizeof(HeapBuffer)/8 + 1,            //a
        sizeof(HeapBuffer)/8 + 2,            //b
        sizeof(HeapBuffer)/8 + 3,            //c
        sizeof(HeapBuffer)/8 + 4,            //d
        sizeof(HeapBuffer)/8 + 5,            //e
        (sizeof(HeapBuffer)/8 + 6) | 0x8000, //free
        sizeof(HeapBuffer)/8 + 7,            //z
        0
    );

    cout << "alloc f\n";
    auto f = (unsigned long*) hb->alloc(8);
    R64FX_CHECK_ALIGNMENT(f);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/8,     //self
        sizeof(HeapBuffer)/8 + 1, //a
        sizeof(HeapBuffer)/8 + 2, //b
        sizeof(HeapBuffer)/8 + 3, //c
        sizeof(HeapBuffer)/8 + 4, //d
        sizeof(HeapBuffer)/8 + 5, //e
        sizeof(HeapBuffer)/8 + 6, //f
        sizeof(HeapBuffer)/8 + 7, //z
        0
    );

    cout << "free b & d\n";
    hb->free(b); b = nullptr;
    hb->free(d); d = nullptr;
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/8,                //self
        sizeof(HeapBuffer)/8 + 1,            //a
        (sizeof(HeapBuffer)/8 + 2) | 0x8000, //free
        sizeof(HeapBuffer)/8 + 3,            //c
        (sizeof(HeapBuffer)/8 + 4) | 0x8000, //free
        sizeof(HeapBuffer)/8 + 5,            //e
        sizeof(HeapBuffer)/8 + 6,            //f
        sizeof(HeapBuffer)/8 + 7,            //z
        0
    );

    cout << "free a (Merge Down)\n";
    hb->free(a); a = nullptr;
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/8,                //self
        (sizeof(HeapBuffer)/8 + 2) | 0x8000, //free
        sizeof(HeapBuffer)/8 + 3,            //c
        (sizeof(HeapBuffer)/8 + 4) | 0x8000, //free
        sizeof(HeapBuffer)/8 + 5,            //e
        sizeof(HeapBuffer)/8 + 6,            //f
        sizeof(HeapBuffer)/8 + 7,            //z
        0
    );

    cout << "free e (Merge Up)\n";
    hb->free(e); e = nullptr;
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/8,                //self
        (sizeof(HeapBuffer)/8 + 2) | 0x8000, //free
        sizeof(HeapBuffer)/8 + 3,            //c
        (sizeof(HeapBuffer)/8 + 5) | 0x8000, //free
        sizeof(HeapBuffer)/8 + 6,            //f
        sizeof(HeapBuffer)/8 + 7,            //z
        0
    );

    cout << "free nullptr\n";
    {
        if(hb->free(a))
        {
            cout << "Failed!\n";
            return false;
        }
    }

    cout << "Alloc Too Much\n";
    {
        if(hb->alloc(hb->size()) != nullptr)
        {
            cout << "Failed!\n";
            return false;
        }
    }

    cout << "free z\n";
    hb->free(z); z = nullptr;
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/8,                //self
        (sizeof(HeapBuffer)/8 + 2) | 0x8000, //free
        sizeof(HeapBuffer)/8 + 3,            //c
        (sizeof(HeapBuffer)/8 + 5) | 0x8000, //free
        sizeof(HeapBuffer)/8 + 6,            //f
        0
    );

    cout << "realloc a\n";
    a = (unsigned long*) hb->alloc(8);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/8,                //self
        sizeof(HeapBuffer)/8 + 1,            //a
        (sizeof(HeapBuffer)/8 + 2) | 0x8000, //free
        sizeof(HeapBuffer)/8 + 3,            //c
        (sizeof(HeapBuffer)/8 + 5) | 0x8000, //free
        sizeof(HeapBuffer)/8 + 6,            //f
        0
    );

    cout << "realloc e\n";
    e = (unsigned long*) hb->alloc(16);
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/8,                //self
        sizeof(HeapBuffer)/8 + 1,            //a
        (sizeof(HeapBuffer)/8 + 2) | 0x8000, //free
        sizeof(HeapBuffer)/8 + 3,            //c
        sizeof(HeapBuffer)/8 + 5,            //e
        sizeof(HeapBuffer)/8 + 6,            //f
        0
    );

    cout << "free c\n";
    hb->free(c); c = nullptr;
    R64FX_CHECK_HEADER(
        sizeof(HeapBuffer)/8,                //self
        sizeof(HeapBuffer)/8 + 1,            //a
        (sizeof(HeapBuffer)/8 + 3) | 0x8000, //free
        sizeof(HeapBuffer)/8 + 5,            //e
        sizeof(HeapBuffer)/8 + 6,            //f
        0
    );

    cout << "free a, e, f\n";
    hb->free(a); a = nullptr;
    hb->free(e); e = nullptr;
    hb->free(f); f = nullptr;
    R64FX_CHECK_HEADER(sizeof(HeapBuffer)/8, 0);

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
        ptr[i] = (unsigned long*) ha->alloc(len[i] * 8);
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

                auto tmpbuf = (unsigned long*) ha->alloc((len[i] + extralen + extraoffset) * 8); 
                if(!ptr[i])
                {
                    cout << "\n1.1 Allocation Failed!\n";
                    return false;
                }
                for(int j=0; j<len[i]; j++)
                {
                    tmpbuf[j + extraoffset] = ptr[i][j];
                }

                ha->free(ptr[i]);
                ptr[i] = nullptr;

                if(!testha(ha, depth - 1))
                    return false;

                ha->free(ptr[i]);
                ptr[i] = (unsigned long*) ha->alloc(len[i] * 8);
                if(!ptr[i])
                {
                    cout << "\n1.2 Reallocation Failed!\n";
                    return false;
                }
                for(int j=0; j<len[i]; j++)
                {
                    ptr[i][j] = tmpbuf[j + extraoffset];
                }
                ha->free(tmpbuf);

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
            auto extrabuf = (unsigned long*) ha->alloc(extralen * 8);
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
                ha->free(extrabuf);
            }
            break;
        }

        default:
            break;
    }

    if(freeha && freeptr)
    {
        freeha->free(freeptr);
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
        ha->free(ptr[i]);
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

        if(!ha->isEmpty())
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

    auto hb = HeapBuffer::newInstance(128);
    ok = ok && test_HeapBuffer(hb);
    HeapBuffer::deleteInstance(hb);

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
