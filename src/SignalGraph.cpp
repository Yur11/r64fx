#include "SignalGraph.hpp"
#include "SignalNodeFlags.hpp"
#include <cstring>

#define R64FX_REGISTER_NOT_USED               (((unsigned long) this)    )
#define R64FX_REGISTER_USED_NO_STORAGE        (((unsigned long) this) + 1)
#define R64FX_REGISTER_MUST_NEVER_BE_USED     (((unsigned long) this) + 2)


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


void SignalGraphImpl::buildNode(SignalNode &node)
{
    if(node.m_iteration_count < iteration_count)
    {
        node.m_iteration_count = iteration_count;
        node.build();
    }
}


void SignalGraph::link(const NodeSource &node_source, const NodeSink &node_sink)
{
    auto source_node  = node_source.node();
    auto sink_node    = node_sink.node();
    auto source       = node_source.port();
    auto sink         = node_sink.port();
    R64FX_DEBUG_ASSERT(sink->connectedSource() == nullptr);

    sink->m_connected_source = source;
    source->m_connected_sink_count++;

    source_node->m_connection_count++;
    sink_node->m_connection_count++;
}


void SignalGraph::unlink(const NodeSink node_sink)
{
    auto sink_node    = node_sink.node();
    auto sink         = node_sink.port();
    auto source       = sink->m_connected_source;
    auto source_node  = source->parentNode();
    sink->m_connected_source = nullptr;
    R64FX_DEBUG_ASSERT(source->m_connected_sink_count > 0);
    source->m_connected_sink_count--;

    R64FX_DEBUG_ASSERT(source_node->m_connection_count > 0);
    source_node->m_connection_count--;
    if(source_node->m_connection_count == 0)
    {
        source_node->m_iteration_count = 0;
    }

    R64FX_DEBUG_ASSERT(sink_node->m_connection_count > 0);
    sink_node->m_connection_count--;
    if(sink_node->m_connection_count == 0)
    {
        sink_node->m_iteration_count = 0;
    }
}


void SignalNode::ensureBuilt(SignalSource* source)
{
    R64FX_DEBUG_ASSERT(source->m_processed_sink_count < source->m_connected_sink_count);
    m.buildNode(*source->parentNode());
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


void SignalGraph::build(SignalNode* terminal_nodes, unsigned int node_count)
{
    R64FX_DEBUG_ASSERT(terminal_nodes);
    R64FX_DEBUG_ASSERT(m.frame_count > 0);

    m.rewindCode();
    PUSH(rbx);
    PUSH(rbp);

    MOV(rcx, Imm32(-m.frame_count));
    JumpLabel loop;
    mark(loop);

    m.iteration_count++;
    for(unsigned int i=0; i<node_count; i++)
        m.buildNode(terminal_nodes[i]);

    ADD(rcx, Imm32(1));
    JNZ(loop);

    POP(rbp);
    POP(rbx);
    RET();
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


RegisterPack<Register> SignalNode::allocRegisters(
    unsigned int count, unsigned long* reg_table, unsigned int reg_table_size, unsigned int reg_size)
{
    R64FX_DEBUG_ASSERT(count < 16);
    RegisterPack<Register> regpack;

    unsigned int n = 0;
    for(unsigned int i=0; i<reg_table_size && n<count; i++)
    {
        if(reg_table[i] == R64FX_REGISTER_NOT_USED)
        {
            reg_table[i] = R64FX_REGISTER_USED_NO_STORAGE;
            regpack.setRegAt(n++, Register(i));
        }
    }

    if(n == count)
        return regpack;
    R64FX_DEBUG_ASSERT(n < count);

    for(unsigned int i=0; i<reg_table_size && n<count; i++)
    {
        if(reg_table[i] != R64FX_REGISTER_MUST_NEVER_BE_USED && reg_table[i] != R64FX_REGISTER_USED_NO_STORAGE)
        {
            auto storage = (SignalDataStorage*)reg_table[i];
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

            reg_table[i] = R64FX_REGISTER_USED_NO_STORAGE;
            regpack.setRegAt(n++, Register(i));
        }
    }

    return regpack;
}


void SignalNode::setStorageRegisters(SignalDataStorage &storage, RegisterPack<Register> regpack, unsigned long* reg_table)
{
    for(unsigned int i=0; i<regpack.size(); i++)
    {
        R64FX_DEBUG_ASSERT(reg_table[regpack[i].code()] == R64FX_REGISTER_USED_NO_STORAGE);
        reg_table[regpack[i].code()] = (unsigned long)&storage;
    }
}


RegisterPack<Register> SignalNode::getStorageRegisters(SignalDataStorage &storage, unsigned long* reg_table, unsigned int reg_table_size) const
{
    RegisterPack<Register> regpack;
    unsigned int n = 0;
    for(unsigned int i=0; i<reg_table_size && n<storage.size(); i++)
    {
        if(reg_table[i] == (unsigned long)&storage)
        {
            regpack.setRegAt(n++, Register(i));
        }
    }
    regpack.setSize(n);
    return regpack;
}


RegisterPack<Register> SignalNode::removeStorageRegisters(SignalDataStorage &storage, unsigned long* reg_table, unsigned int reg_table_size)
{
    RegisterPack<Register> regpack;
    unsigned int n = 0;
    for(unsigned int i=0; i<reg_table_size && n<storage.size(); i++)
    {
        if(reg_table[i] == (unsigned long)&storage)
        {
            regpack.setRegAt(n++, Register(i));
            reg_table[i] = R64FX_REGISTER_USED_NO_STORAGE;
        }
    }
    regpack.setSize(n);
    return regpack;
}


void SignalNode::freeRegisters(RegisterPack<Register> regpack, unsigned long* reg_table, unsigned int reg_table_size)
{
    for(unsigned int i=0; i<regpack.size(); i++)
    {
        auto reg = regpack.regAt(i);
        R64FX_DEBUG_ASSERT(reg.code() < reg_table_size);
        R64FX_DEBUG_ASSERT(reg_table[reg.code()] == R64FX_REGISTER_USED_NO_STORAGE);
        reg_table[reg.code()] = R64FX_REGISTER_NOT_USED;
    }
}


void SignalNode::initStorage_(
    SignalDataStorage &storage, DataBufferPointer memptr, unsigned int size, RegisterPack<Register> regpack, unsigned long* reg_table)
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
            R64FX_DEBUG_ASSERT(reg_table[regpack[i].code()] == R64FX_REGISTER_USED_NO_STORAGE);
            reg_table[regpack[i].code()] = (unsigned long)&storage;
        }
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
