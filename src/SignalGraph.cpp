#include "SignalGraph.hpp"
#include "SignalNodeFlags.hpp"
#include <cstring>


namespace r64fx{

SignalGraphCompiler::SignalGraphCompiler()
{
    for(int i=0; i<11; i++)
    {
        m_gprs[i] = 0;
    }
    m_gprs[mainLoopCounter().code()]  = 0xFFFFFFFFFFFFFFFFUL;
    m_gprs[rsp.code()]                = 0xFFFFFFFFFFFFFFFFUL;//Never touch!

    for(int i=0; i<16; i++)
    {
        m_xmms[i] = 0;
    }

    resize(1, 1);
    growData(memory_page_size());
}


void SignalGraphCompiler::link(const NodeSource &node_source, const NodeSink &node_sink)
{
    auto source_node = node_source.node();
    R64FX_DEBUG_ASSERT(source_node);

    auto sink_node = node_sink.node();
    R64FX_DEBUG_ASSERT(sink_node);

    auto source = node_source.port();
    R64FX_DEBUG_ASSERT(source);

    auto sink = node_sink.port();
    R64FX_DEBUG_ASSERT(sink);
    R64FX_DEBUG_ASSERT(sink->connectedSource() == nullptr);

    sink->m_connected_source = source;
    source->m_connected_sink_count++;

    source_node->m_connection_count++;
    sink_node->m_connection_count++;
}


void SignalGraphCompiler::unlink(const NodeSink node_sink)
{
    auto sink_node = node_sink.node();
    R64FX_DEBUG_ASSERT(sink_node);

    auto sink = node_sink.port();
    R64FX_DEBUG_ASSERT(sink);

    auto source = sink->m_connected_source;
    R64FX_DEBUG_ASSERT(source);

    auto source_node = source->parentNode();
    R64FX_DEBUG_ASSERT(source_node);

    sink->m_connected_source = nullptr;
    R64FX_DEBUG_ASSERT(source->m_connected_sink_count > 0);
    source->m_connected_sink_count--;

    R64FX_DEBUG_ASSERT(source_node->m_connection_count > 0);
    source_node->m_connection_count--;
    if(source_node->m_connection_count == 0)
    {
        source_node->cleanup(*this);
        source_node->m_iteration_count = 0;
    }

    R64FX_DEBUG_ASSERT(sink_node->m_connection_count > 0);
    sink_node->m_connection_count--;
    if(sink_node->m_connection_count == 0)
    {
        sink_node->cleanup(*this);
        sink_node->m_iteration_count = 0;
    }
}


void SignalGraphCompiler::build(SignalNode* terminal_nodes, unsigned int nnodes)
{
    R64FX_DEBUG_ASSERT(frameCount() > 0);

    rewindCode();
    PUSH(rbx);
    PUSH(rbp);

    MOV(mainLoopCounter(), Imm32(-frameCount()));
    JumpLabel loop;
    mark(loop);

    m_iteration_count++;
    for(unsigned int i=0; i<nnodes; i++)
        buildNode(terminal_nodes + i);

    ADD(mainLoopCounter(), Imm32(1));
    JNZ(loop);

    POP(rbp);
    POP(rbx);
    RET();
}


void SignalGraphCompiler::ensureBuilt(SignalSource* source)
{
    R64FX_DEBUG_ASSERT(source->m_processed_sink_count < source->m_connected_sink_count);
    buildNode(source->parentNode());
    source->m_processed_sink_count++;
}

void SignalGraphCompiler::sourceUsed(SignalSource* source)
{
    R64FX_DEBUG_ASSERT(source->m_processed_sink_count <= source->m_connected_sink_count);
    if(source->m_processed_sink_count == source->m_connected_sink_count)
    {
        source->m_processed_sink_count = 0;
        freeStorage(*source);
    }
}


void SignalGraphCompiler::buildNode(SignalNode* node)
{
    if(node->m_iteration_count < m_iteration_count)
    {
        node->build(*this);
        node->m_iteration_count = m_iteration_count;
    }
}


DataBufferPointer SignalGraphCompiler::allocMemory(unsigned int nbytes, unsigned int align)
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


void SignalGraphCompiler::freeMemory(DataBufferPointer ptr)
{
    R64FX_DEBUG_ASSERT(ptr);
    HeapBuffer hb(dataBegin(), dataBufferSize());
    hb.freeChunk(ptrMem<unsigned char*>(ptr));
}


namespace{

template<typename RegT, unsigned int MaxRegs> inline unsigned int alloc_regs(unsigned long* m_regs, RegT* regs, unsigned int nregs)
{
    R64FX_DEBUG_ASSERT(nregs < MaxRegs);
    unsigned int n = 0;
    for(unsigned int i=0; i<MaxRegs && n<nregs; i++)
    {
        if(m_regs[i] == 0)
        {
            regs[n++] = RegT(i);
            m_regs[i] = 0xFFFFFFFFFFFFFFFFUL;
        }
    }
    return n;
}


template<typename RegT, unsigned int MaxRegs> inline void free_regs(unsigned long* m_regs, RegT* regs, unsigned int nregs)
{
    R64FX_DEBUG_ASSERT(nregs < MaxRegs);
    for(unsigned int i=0; i<nregs; i++)
    {
        R64FX_DEBUG_ASSERT(m_regs[regs[i].code()]);
        m_regs[i] = 0;
    }
}


template<typename RegT> inline unsigned int pack_regs(RegT* regs, unsigned int nregs)
{
    unsigned int bits = 0;
    for(unsigned int i=0; i<nregs; i++)
    {
        bits |= (1<<regs[i].code());
    }
    return bits;
}


template<typename RegT> inline void unpack_regs(unsigned int bits, RegT* regs, unsigned int* nregs)
{
    unsigned int n = 0;
    for(int i=0; i<16 && bits; i++)
    {
        unsigned int bit = (1<<i);
        if(bits & bit)
        {
            regs[n] = RegT(i);
            bits &= ~bits;
            n++;
        }
    }
    *nregs = n;
}

}//namespace


unsigned int SignalGraphCompiler::allocGPR(GPR64* gprs, unsigned int ngprs)
{
    return alloc_regs<GPR64, 11>(m_gprs, gprs, ngprs);
}


void SignalGraphCompiler::freeGPR(GPR64* gprs, unsigned int ngprs)
{
    free_regs<GPR64, 11>(m_gprs, gprs, ngprs);
}


unsigned int SignalGraphCompiler::allocXmm(Xmm* xmms, unsigned int nxmms)
{
    return alloc_regs<Xmm, 16>(m_xmms, xmms, nxmms);
}


void SignalGraphCompiler::freeXmm(Xmm* xmms, unsigned int nxmms)
{
    free_regs<Xmm, 16>(m_xmms, xmms, nxmms);
}


void SignalGraphCompiler::setStorage(SignalDataStorage &storage, DataBufferPointer ptr)
{
    storage.setMemory();
    storage.u.d[0] = ptr.offset();
}


DataBufferPointer SignalGraphCompiler::getPtr(SignalDataStorage storage) const
{
    R64FX_DEBUG_ASSERT(storage.isMemory());
    return storage.u.d[0];
}


void SignalGraphCompiler::setStorage(SignalDataStorage &storage, GPR* gprs, unsigned int ngprs)
{
    storage.setGPR();
    storage.u.d[0] = pack_regs(gprs, ngprs);
}


void SignalGraphCompiler::getStorage(SignalDataStorage storage, GPR* gprs, unsigned int* ngprs)
{
    unpack_regs(storage.u.d[0], gprs, ngprs);
}


void SignalGraphCompiler::setStorage(SignalDataStorage &storage, Xmm* xmms, unsigned int nxmms)
{
    storage.setXmm();
    storage.u.d[0] = pack_regs(xmms, nxmms);
}


void SignalGraphCompiler::getStorage(SignalDataStorage storage, Xmm* xmms, unsigned int* nxmms)
{
    unpack_regs(storage.u.d[0], xmms, nxmms);
}


void SignalGraphCompiler::freeStorage(SignalDataStorage &storage)
{
    if(storage.isXmm())
    {
        Xmm regs[16]; unsigned int nregs;
        getStorage(storage, regs, &nregs);
        freeXmm(regs, nregs);
    }
    else if(storage.isGPR())
    {
        GPR64 regs[16]; unsigned int nregs;
        getStorage(storage, regs, &nregs);
        freeGPR(regs, nregs);
    }
    else
    {
        freeMemory(storage.u.d[0]);
    }
    storage.clear();
}

}//namespace r64fx
