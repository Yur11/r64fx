#ifndef R64FX_SIGNAL_GRAPH_HPP
#define R64FX_SIGNAL_GRAPH_HPP

#include "Debug.hpp"
#include "TypeUtils.hpp"
#include "jit.hpp"

#define R64FX_NODE_SOURCE(name) private: SignalSource m_##name; public: inline NodeSource name() { return {this, &m_##name}; } private:
#define R64FX_NODE_SINK(name)   private: SignalSink   m_##name; public: inline NodeSink   name() { return {this, &m_##name}; } private:


namespace r64fx{

class SignalGraphCompiler;


class SignalNode{
    friend class SignalGraphCompiler;
    unsigned long m_iteration_count = 0;
    unsigned long m_connection_count = 0;

public:
    SignalNode() {}

    virtual ~SignalNode() {}

private:
    virtual void build(SignalGraphCompiler &c) = 0;

    virtual void cleanup(SignalGraphCompiler &c) = 0;
};


/*  */
class SignalDataStorage{
    friend class SignalGraphCompiler;

    unsigned long m = 0;

    R64FX_VALUE_TYPE(DataType, unsigned long);
    R64FX_VALUE_TYPE(RegType,  unsigned long);

    inline void setRegType(RegType rt) { m &= ~(0xFFUL << 48); m |= (rt.value() << 48); }

    inline void setMemoryOffset(unsigned long offset)
        { R64FX_DEBUG_ASSERT(offset < 0xFFFFUL); m &= ~(0xFFFFUL << 16); m |= (offset << 16); }
    inline unsigned long memoryOffset() const { return (m >> 16) & 0xFFFF; }

    inline void setRegisterBits(unsigned long bits)
        { R64FX_DEBUG_ASSERT(bits < 0xFFFFUL); m &= ~(0xFFFFUL << 32); m |= (bits << 32); }
    inline unsigned long registerBits() const { return (m >> 32) & 0xFFFFUL; }

public:
    inline bool isInMemory() const { return memoryOffset(); }

    inline bool isInRegisters() const { return registerBits(); }

    inline void setSize(unsigned long size)
        { R64FX_DEBUG_ASSERT(size < 0xFFFFUL); m &= ~0xFFFFUL; m |= size; }
    inline unsigned int size() const { return m & 0xFFFFUL; }

    inline unsigned int elementSize() const { return (type().value() & 1) ? 8 : 4; }

    inline void setType(DataType dt) 
        { m &= ~(0xFFUL << 40); m |= (dt.value() << 40); }
    inline DataType type() const { return DataType((m >> 40) & 0xFFUL); }

    inline static DataType Type(float)   { return DataType(0); }
    inline static DataType Type(double)  { return DataType(1); }
    inline static DataType Type(int)     { return DataType(2); }
    inline static DataType Type(long)    { return DataType(3); }

    inline RegType regType() const { return RegType((m >> 48) & 0xFFUL); }

    inline static RegType Type(GPR) { return RegType(0); }
    inline static RegType Type(Xmm) { return RegType(1); }
    inline static RegType Type(Ymm) { return RegType(2); }
};


class DataBufferPointer{
    friend class SignalGraphCompiler;

    unsigned short offset = 0;

    DataBufferPointer(unsigned short offset) : offset(offset) {}

public:
    inline operator bool() const { return offset; }
};


template<typename RegT> class RegisterPack{
    friend class SignalGraphCompiler;

    unsigned long bits = 0;

    RegisterPack(unsigned long bits) : bits(bits) {}

    inline void setSize(unsigned int size) { R64FX_DEBUG_ASSERT(size < 16); bits &= ~0xFUL; bits |= size; }

    inline void setRegAt(unsigned int i, RegT reg)
    {
        R64FX_DEBUG_ASSERT(i < 16);
        bits &= ~(0xFUL << ((i+1) << 2));
        bits |= (((unsigned long)reg.code()) << ((i+1) << 2));
    }

public:
    RegisterPack() {}

    RegisterPack(std::initializer_list<RegT> regs)
    {
        R64FX_DEBUG_ASSERT(regs.size() < 16);
        unsigned int i = 0;
        for(auto reg : regs)
        {
            setRegAt(i++, reg);
        }
        setSize(regs.size());
    }

    inline operator bool() const { return bits; }

    inline unsigned int size() const { return bits & 0xFUL; }

    inline RegT regAt(unsigned int i) const
    {
        R64FX_DEBUG_ASSERT(i < size());
        return RegT((bits >> ((i+1) << 2)) & 0xFUL);
    }

    inline RegT operator[](unsigned int i) { return regAt(i); }
};


/* One SignalSource can be connected to multiple SignalSink instances. */
class SignalSource : public SignalDataStorage{
    friend class SignalGraphCompiler;
    SignalNode* m_parent_node = nullptr;
    unsigned int m_connected_sink_count = 0;
    unsigned int m_processed_sink_count = 0;

public:
    SignalSource(SignalNode* parent_node)
    : m_parent_node(parent_node) {}

    ~SignalSource() {}

    inline SignalNode* parentNode() const { return m_parent_node; }

    inline int connectedSinkCount() const { return m_connected_sink_count; }

    inline int processedSinkCount() const { return m_processed_sink_count; }
};


/* Each SignalSink can be connected to only one SignalSource. */
class SignalSink{
    friend class SignalGraphCompiler;

    SignalSource*  m_connected_source  = nullptr;

public:
    SignalSink() {}

    ~SignalSink() {}

    inline SignalSource* connectedSource() const { return m_connected_source; }
};


/*  */
template<typename SignalPortT> class NodePort{
    SignalNode*   m_node  = nullptr;
    SignalPortT*  m_port  = nullptr;

public:
    NodePort(SignalNode* node, SignalPortT* port)
    : m_node(node), m_port(port) {}

    inline SignalNode*  node() const { return m_node; }

    inline SignalPortT* port() const { return m_port; }
};

typedef NodePort<SignalSource> NodeSource;
typedef NodePort<SignalSink> NodeSink;


/*  */
class SignalGraphCompiler : public Assembler{
    unsigned long  m_iteration_count = 0;
    unsigned long  m_flags        = 0;
    unsigned long  m_frame_count  = 0;

    unsigned long m_gprs[16];
    inline unsigned long* registerTable      (GPR64) { return m_gprs; }
    inline unsigned int   registerTableSize  (GPR64) { return 16; }

    unsigned long m_xmms[16];
    inline unsigned long* registerTable      (Xmm)   { return m_xmms; }
    inline unsigned int   registerTableSize  (Xmm)   { return 16; }

public:
    SignalGraphCompiler();

    void link(const NodeSource &node_source, const NodeSink &node_sink);

    void unlink(const NodeSink node_sink);

    void build(SignalNode* terminal_nodes, unsigned int nnodes);

    void ensureBuilt(SignalSource* source);

    void sourceUsed(SignalSource* source);

private:
    void buildNode(SignalNode &node);

public:
    inline long run() { return ((long (*)())codeBegin())(); }

    inline unsigned long frameCount() const { return m_frame_count; }

    inline void setFrameCount(unsigned long frame_count) { m_frame_count = frame_count; }

    inline GPR64 mainLoopCounter() const { return rcx; }


private:
    DataBufferPointer allocMemoryBytes(unsigned int nbytes, unsigned int align);

public:
    template<typename T> inline DataBufferPointer allocMemory(unsigned int nitems) { return allocMemory<T>(nitems, sizeof(T)); }

    template<typename T> inline DataBufferPointer allocMemory(unsigned int nitems, unsigned align)
    {
        R64FX_DEBUG_ASSERT(!(align & (align - 1)));
        R64FX_DEBUG_ASSERT(align >= sizeof(T));
        allocMemoryBytes(nitems * sizeof(T), align);
    }

    template<typename T = void*> inline T ptr(SignalDataStorage storage)
    {
        R64FX_DEBUG_ASSERT(storage.type() == SignalDataStorage::type(T()));
        return ptr<T>(storage.memoryOffset());
    }

    template<typename T = void*> inline T ptr(DataBufferPointer ptr)
    {
        R64FX_DEBUG_ASSERT(ptr.offset > 0);
        return (T)(codeBegin() - ptr.offset);
    }

    inline DataBufferPointer allocStorageMemory(SignalDataStorage &storage)
    {
        R64FX_DEBUG_ASSERT(storage.size() > 0);
        R64FX_DEBUG_ASSERT(storage.memoryOffset() == 0);
        auto mem = allocMemoryBytes(storage.size() * storage.elementSize(), storage.elementSize());
        storage.setMemoryOffset(mem.offset);
        return mem;
    }

    inline DataBufferPointer getStorageMemory(SignalDataStorage &storage) const { return storage.memoryOffset(); }

    inline DataBufferPointer removeStorageMemory(SignalDataStorage &storage)
        { auto val = storage.memoryOffset(); storage.setMemoryOffset(0); return val; }

    inline void freeStorageMemory(SignalDataStorage &storage) { freeMemory(removeStorageMemory(storage)); }

    void freeMemory(DataBufferPointer ptr);


private:
    RegisterPack<Register> allocRegisters(
        unsigned int count, unsigned long* reg_table, unsigned int reg_table_size, unsigned int reg_size);
public:
    template<typename T> inline RegisterPack<T> allocRegisters(unsigned int count)
        { return allocRegisters(count, registerTable(T()), registerTableSize(T()), T::Size()).bits; }

private:
    void setStorageRegisters(SignalDataStorage &storage, RegisterPack<Register> regpack, unsigned long* reg_table);
public:
    template<typename T> inline void setStorageRegisters(SignalDataStorage &storage, RegisterPack<T> regpack)
    {
        setStorageRegisters(storage, regpack.bits, registerTable(T()));
        storage.regType() == SignalDataStorage::RegType(T());
    }

private:
    RegisterPack<Register> getStorageRegisters(SignalDataStorage &storage, unsigned long* reg_table) const;
public:
    template<typename T> inline RegisterPack<T> getStorageRegisters(SignalDataStorage &storage) const
    {
        R64FX_DEBUG_ASSERT(storage.type() == SignalDataStorage::Type(T()));
        return getStorageRegisters(storage, registerTable(T())).bits;
    }

private:
    RegisterPack<Register> removeStorageRegisters(SignalDataStorage &storage, unsigned long* reg_table);
public:
    template<typename T> inline RegisterPack<T> removeStorageRegisters(SignalDataStorage &storage)
    {
        R64FX_DEBUG_ASSERT(storage.type() == SignalDataStorage::Type(T()));
        return removeStorageRegisters(storage, registerTable(T())).bits;
    }

    template<typename T> inline void freeStorageRegisters(SignalDataStorage &storage)
        { freeRegisters<T>(removeStorageRegisters<T>(storage)); }

private:
    void freeRegisters(RegisterPack<Register> pack, unsigned long* reg_table, unsigned int reg_table_size);
public:
    template<typename T> void freeRegisters(RegisterPack<T> regpack)
        { freeRegisters(regpack.bits, registerTable(T()), registerTableSize(T())); }

    void freeStorage(SignalDataStorage &storage);
};

}//namespace r64fx

#endif//R64FX_SIGNAL_GRAPH_HPP
