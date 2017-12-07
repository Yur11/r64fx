#include "SignalGraph.hpp"
#include <cstring>

#define R64FX_REGISTER_NOT_USED               0
#define R64FX_REGISTER_USED_NO_STORAGE        1
#define R64FX_REGISTER_MUST_NEVER_BE_USED     2

#define R64FX_BUILD_IN_PROGRESS               1


namespace r64fx{

SignalGraphImpl::SignalGraphImpl()
{
    for(int i=0; i<11; i++)
    {
        gprs[i] = R64FX_REGISTER_NOT_USED;
    }
    gprs[rcx.code()] = R64FX_REGISTER_USED_NO_STORAGE;
    gprs[rsp.code()] = R64FX_REGISTER_MUST_NEVER_BE_USED;
    for(int i=11; i<16; i++)
    {
        gprs[i] = R64FX_REGISTER_MUST_NEVER_BE_USED;
    }

    for(int i=0; i<16; i++)
    {
        xmms[i] = R64FX_REGISTER_NOT_USED;
    }

    resize(1, 1);
    growData(memory_page_size());
}


void SignalGraphImpl::buildNode(SignalNode* node)
{
    R64FX_DEBUG_ASSERT(build_bits & R64FX_BUILD_IN_PROGRESS);

    if(node->m_iteration_count != iteration_count)
    {
        node->m_iteration_count = iteration_count;
        node->build();
    }
}


void SignalGraph::link(SignalSource* source, SignalSink* sink)
{
    R64FX_DEBUG_ASSERT(sink->connectedSource() == nullptr);
    sink->m_connected_source = source;
    source->m_connected_sink_count++;
}


void SignalGraph::unlink(SignalSink* sink)
{
    auto source       = sink->m_connected_source;
    sink->m_connected_source = nullptr;
    R64FX_DEBUG_ASSERT(source->m_connected_sink_count > 0);
    source->m_connected_sink_count--;
}


void SignalGraph::beginBuild()
{
    R64FX_DEBUG_ASSERT(!(m.build_bits & R64FX_BUILD_IN_PROGRESS));
    m.build_bits |= R64FX_BUILD_IN_PROGRESS;
    R64FX_DEBUG_ASSERT(m.frame_count > 0);

    m.rewindCode();
    PUSH(rbx);
    PUSH(rbp);

    MOV(rcx, Imm32(-m.frame_count));
    mark(m.loop);

    m.iteration_count++;
}


void SignalGraph::endBuild()
{
    R64FX_DEBUG_ASSERT(m.build_bits & R64FX_BUILD_IN_PROGRESS);
    m.build_bits &= ~R64FX_BUILD_IN_PROGRESS;

    ADD(rcx, Imm32(1));
    JNZ(m.loop);

    POP(rbp);
    POP(rbx);
    RET();
}


void SignalNode::ensureBuilt(SignalSource* source)
{
    R64FX_DEBUG_ASSERT(source->m_processed_sink_count < source->m_connected_sink_count);
    m.buildNode(source->parentNode());
    source->m_processed_sink_count++;
}

void SignalNode::sourceUsed(SignalSource* source)
{
    R64FX_DEBUG_ASSERT(source->m_processed_sink_count <= source->m_connected_sink_count);
    if(source->m_processed_sink_count == source->m_connected_sink_count)
    {
        source->m_processed_sink_count = 0;
        freeStorage(*source);
    }
}


DataBufferPointer SignalGraphImpl::allocMemoryBytes(unsigned int nbytes, unsigned int align)
{
    R64FX_DEBUG_ASSERT(nbytes > 0);
    R64FX_DEBUG_ASSERT(align > 0);
    unsigned int align_mask = align - 1;
    R64FX_DEBUG_ASSERT((align & align_mask) == 0);

    for(;;)
    {
        HeapBuffer hb(dataBegin(), dataBufferSize());
        auto chunk = (unsigned char*) hb.allocChunk(nbytes, align);
        if(chunk)
        {
            long offset = long(codeBegin()) - long(chunk);
            R64FX_DEBUG_ASSERT(offset > 0);
            R64FX_DEBUG_ASSERT(offset <= 0xFFFFFFFF);
            return DataBufferPointer(offset);
        }

        growData(memory_page_size());
        memcpy(dataBegin(), dataBegin() + memory_page_size(), hb.headerSize());
    }
    return DataBufferPointer(0);
}


RegisterPack<Register> SignalNode::allocRegisters(unsigned int count, RegisterTable rt)
{
    R64FX_DEBUG_ASSERT(count < 16);
    RegisterPack<Register> regpack;

    unsigned int n = 0;
    for(unsigned int i=0; i<rt.size && n<count; i++)
    {
        if(rt[i] == R64FX_REGISTER_NOT_USED)
        {
            rt[i] = R64FX_REGISTER_USED_NO_STORAGE;
            regpack.setRegAt(n++, Register(i));
        }
        else
        {
        }
    }

    if(n < count)
    {
        for(unsigned int i=0; i<rt.size && n<count; i++)
        {
            if(rt[i] != R64FX_REGISTER_MUST_NEVER_BE_USED && rt[i] != R64FX_REGISTER_USED_NO_STORAGE)
            {
                auto storage = (SignalDataStorage*)rt[i];
                if(storage->isLocked())
                    continue;

                DataBufferPointer ptr;
                if(storage->hasMemory())
                {
                    ptr = getStorageMemory(*storage);
                }
                else
                {
                    ptr = m.allocMemoryBytes(storage->totalSize(), storage->registerSize());
                    storage->setMemory(ptr);
                }
                R64FX_DEBUG_ASSERT(ptr);

                rt[i] = R64FX_REGISTER_USED_NO_STORAGE;
                regpack.setRegAt(n++, Register(i));
            }
        }
    }
    R64FX_DEBUG_ASSERT(n == count);

    regpack.setSize(n);
    return regpack;
}


RegisterPack<Register> SignalNode::getStorageRegisters(SignalDataStorage &storage, RegisterTable rt) const
{
    RegisterPack<Register> regpack;
    unsigned int n = 0;
    for(unsigned int i=0; i<rt.size && n<storage.size(); i++)
    {
        if(rt[i] == (unsigned long)&storage)
        {
            regpack.setRegAt(n++, Register(i));
        }
    }
    regpack.setSize(n);
    return regpack;
}


void SignalNode::freeRegisters(RegisterPack<Register> regpack, RegisterTable rt)
{
    for(unsigned int i=0; i<regpack.size(); i++)
    {
        auto reg = regpack.regAt(i);
        R64FX_DEBUG_ASSERT(reg.code() < rt.size);
        R64FX_DEBUG_ASSERT(rt[reg.code()] == R64FX_REGISTER_USED_NO_STORAGE);
        rt[reg.code()] = R64FX_REGISTER_NOT_USED;
    }
}


void SignalNode::initStorage_(
    SignalDataStorage &storage, DataBufferPointer memptr, unsigned int size, RegisterPack<Register> regpack, RegisterTable rt)
{
    storage.setSize(size);

    if(memptr)
    {
        R64FX_DEBUG_ASSERT(m.heapBuffer().chunkSize(addr(storage.memory().offset())) == storage.totalSize());
        storage.setMemory(memptr);
    }

    if(regpack)
    {
        for(unsigned int i=0; i<regpack.size(); i++)
        {
            R64FX_DEBUG_ASSERT(rt[regpack[i].code()] == R64FX_REGISTER_USED_NO_STORAGE);
            rt[regpack[i].code()] = (unsigned long)&storage;
        }
        storage.m |= SignalDataStorage::HasRegistersBit;
    }
}


void SignalNode::freeStorage(SignalDataStorage &storage)
{
    if(storage.hasRegisters())
    {
        R64FX_DEBUG_ASSERT(storage.registerType() != SignalRegisterType::Bad);
        switch(storage.registerType())
        {
            case SignalRegisterType::GPR:
            {
                break;
            }

            case SignalRegisterType::Xmm:
            {
                break;
            }

            case SignalRegisterType::Ymm:
            {
                break;
            }

            default:
                break;
        }
    }

    if(storage.hasMemory())
    {
        freeStorageMemory(storage);
    }
}

}//namespace r64fx
