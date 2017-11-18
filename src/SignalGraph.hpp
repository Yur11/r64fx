#ifndef R64FX_SIGNAL_GRAPH_HPP
#define R64FX_SIGNAL_GRAPH_HPP

#include "Debug.hpp"
#include "TypeUtils.hpp"
#include "jit.hpp"

/* Convenience macros for declaring SignalNode ports. */
#define R64FX_NODE_SOURCE(name) private: SignalSource m_##name; public: inline NodeSource name() { return {this, &m_##name}; } private:
#define R64FX_NODE_SINK(name)   private: SignalSink   m_##name; public: inline NodeSink   name() { return {this, &m_##name}; } private:


namespace r64fx{

class SignalGraph;
class SignalNode;


/* A class used for sharing intermediate data between SignalNode instances.
 * It represents vectors of data that are stored in registers and(or) memory. */
class SignalDataStorage{
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
    /* Signal data can be stored either in registers or in memory. */
    inline bool isInMemory()    const { return memoryOffset(); }
    inline bool isInRegisters() const { return registerBits(); }

    /* Chunks of signal data vector will inevitably end up in registers during processing.
       Therefore we shall store the size of the vector as register count. */
    inline void setRegisterCount(unsigned long count)
        { R64FX_DEBUG_ASSERT(count < 0xFFFFUL); m &= ~0xFFFFUL; m |= count; }
    inline unsigned int registerCount() const { return m & 0xFFFFUL; }

    /* Only one register type shall be used in a SignalDataStorage insstance. */
    inline static RegType Type(GPR) { return RegType(0); }
    inline static RegType Type(Xmm) { return RegType(1); }
    inline static RegType Type(Ymm) { return RegType(2); }

    inline RegType registerType() const { return RegType((m >> 48) & 0xFFUL); }

    /* Size of current register type in bytes. */
    inline unsigned int registerSize() const { static char s[3] = {8, 16, 32}; return s[registerType().value()]; }

    inline unsigned int byteCount() const { return registerCount() * registerSize(); }


    /* The type of data stored by this instance. */
    inline static DataType Type(float)   { return DataType(0); }
    inline static DataType Type(double)  { return DataType(1); }
    inline static DataType Type(int)     { return DataType(2); }
    inline static DataType Type(long)    { return DataType(3); }

    inline void setType(DataType dt)
        { m &= ~(0xFFUL << 40); m |= (dt.value() << 40); }
    inline DataType type() const { return DataType((m >> 40) & 0xFFUL); }

    inline bool isDouble() const { return type().value() & 1; }

    friend class SignalNode;
};


class DataBufferPointer{
    unsigned short offset = 0;

    DataBufferPointer(unsigned short offset) : offset(offset) {}

public:
    inline operator bool() const { return offset; }

    friend class SignalGraphImpl;
    friend class SignalNode;
};


template<typename RegT> class RegisterPack{
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

    friend class SignalNode;
};


/* One SignalSource can be connected to multiple SignalSink instances. */
class SignalSource : public SignalDataStorage{
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

    friend class SignalGraph;
    friend class SignalNode;
};


/* Each SignalSink can be connected to only one SignalSource. */
class SignalSink{
    SignalSource*  m_connected_source  = nullptr;

public:
    SignalSink() {}

    ~SignalSink() {}

    inline SignalSource* connectedSource() const { return m_connected_source; }

    friend class SignalGraph;
};


/* Convenience decoration class used for function args. */
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


/* Common implementation structure used by SignalGraph and SignalNode instanes.*/
class SignalGraphImpl : public AssemblerBuffers{
    friend class SignalGraph;
    friend class SignalNode;
    friend class SignalGraphImplVar;

    unsigned long  iteration_count = 0;
    unsigned long  flags        = 0;
    unsigned long  frame_count  = 0;

    unsigned long gprs[16];
    inline unsigned long* registerTable      (GPR64) { return gprs; }
    inline unsigned int   registerTableSize  (GPR64) { return 16; }

    unsigned long xmms[16];
    inline unsigned long* registerTable      (Xmm)   { return xmms; }
    inline unsigned int   registerTableSize  (Xmm)   { return 16; }

    SignalGraphImpl();

    void buildNode(SignalNode &node);

    inline HeapBuffer heapBuffer() const { return HeapBuffer(dataBegin(), dataBufferSize()); }

    DataBufferPointer allocMemoryBytes(unsigned int nbytes, unsigned int align);

    inline void freeMemory(DataBufferPointer ptr) { heapBuffer().freeChunk(codeBegin() - ptr.offset); }
};

class SignalGraphImplVar{
protected:
    SignalGraphImpl m;
    SignalGraphImplVar() {}
};

class SignalGraphImplRef{
protected:
    SignalGraphImpl &m;
    SignalGraphImplRef(SignalGraphImpl &ref) : m(ref) {}
};

/*  */
class SignalGraph : private AssemblerInstructions<SignalGraphImplVar>{
public:
    SignalGraph();

    void link(const NodeSource &node_source, const NodeSink &node_sink);

    void unlink(const NodeSink node_sink);

    void build(SignalNode* terminal_nodes, unsigned int node_count);

    inline long run() { return ((long (*)())m.codeBegin())(); }

    inline unsigned long frameCount() const { return m.frame_count; }

    inline void setFrameCount(unsigned long frame_count) { m.frame_count = frame_count; }

    friend class SignalNode;
};


/* Base class for elements of a signal graph. */
class SignalNode : protected AssemblerInstructions<SignalGraphImplRef>{
    unsigned long m_iteration_count = 0;
    unsigned long m_connection_count = 0;

public:
    /* All nodes must exist within the context of a parent graph. */
    SignalNode(SignalGraph &sg) : AssemblerInstructions(sg.m) {}

    virtual ~SignalNode() {}

private:
    /* Main build routine. Reimplement in a sub-class. */
    virtual void build() = 0;

protected:
    /* Make sure that source node has been built.
       Call this in the build routine before using the source data.*/
    void ensureBuilt(SignalSource* source);

    /* Call this in the build routine after you are done using the source data. */
    void sourceUsed(SignalSource* source);


    /* Allocate some memory in the data buffer. */
    template<typename T> inline DataBufferPointer allocMemory(unsigned int nitems) { return allocMemory<T>(nitems, sizeof(T)); }

    template<typename T> inline DataBufferPointer allocMemory(unsigned int nitems, unsigned align)
    {
        R64FX_DEBUG_ASSERT(!(align & (align - 1)));
        R64FX_DEBUG_ASSERT(align >= sizeof(T));
        m.allocMemoryBytes(nitems * sizeof(T), align);
    }


    /* Get actual memory address. Do not cache the value returned, use it immediately! */
    template<typename T = float> inline T* addr(DataBufferPointer ptr)
    {
        R64FX_DEBUG_ASSERT(ptr.offset > 0);
        return (T*)(m.codeBegin() - ptr.offset);
    }

    template<typename T = float> inline T* addr(SignalDataStorage storage)
    {
        R64FX_DEBUG_ASSERT(storage.type() == SignalDataStorage::type(T()));
        return addr<T>(storage.memoryOffset());
    }


    /* Assing memory to storage. */
    inline void setStorageMemory(SignalDataStorage &storage, DataBufferPointer ptr)
    {
        R64FX_DEBUG_ASSERT(m.heapBuffer().chunkSize(addr(storage.memoryOffset())) == storage.byteCount());
        storage.setMemoryOffset(ptr.offset);
    }

    /* Get storage memory. */
    inline DataBufferPointer getStorageMemory(SignalDataStorage &storage) const { return storage.memoryOffset(); }

    /* Detach memory from storage but do not free it. */
    inline DataBufferPointer removeStorageMemory(SignalDataStorage &storage)
        { auto val = storage.memoryOffset(); storage.setMemoryOffset(0); return val; }

    /* Detach memory from storage and free it. */
    inline void freeStorageMemory(SignalDataStorage &storage) { freeMemory(removeStorageMemory(storage)); }

    inline void freeMemory(DataBufferPointer ptr) { m.freeMemory(ptr); }


    /* Allocate registers. */
private:
    RegisterPack<Register> allocRegisters(
        unsigned int count, unsigned long* reg_table, unsigned int reg_table_size, unsigned int reg_size);
protected:
    template<typename T> inline RegisterPack<T> allocRegisters(unsigned int count)
        { return allocRegisters(count, registerTable(T()), registerTableSize(T()), T::Size()).bits; }


    /* Assign registers to storage. */
private:
    void setStorageRegisters(SignalDataStorage &storage, RegisterPack<Register> regpack, unsigned long* reg_table);
protected:
    template<typename T> inline void setStorageRegisters(SignalDataStorage &storage, RegisterPack<T> regpack)
    {
        setStorageRegisters(storage, regpack.bits, registerTable(T()));
        storage.registerType() == SignalDataStorage::RegType(T());
    }


    /* Get storage registers. */
private:
    RegisterPack<Register> getStorageRegisters(SignalDataStorage &storage, unsigned long* reg_table) const;
protected:
    template<typename T> inline RegisterPack<T> getStorageRegisters(SignalDataStorage &storage) const
    {
        R64FX_DEBUG_ASSERT(storage.type() == SignalDataStorage::Type(T()));
        return getStorageRegisters(storage, registerTable(T())).bits;
    }


    /* Remove registers from storage. */
private:
    RegisterPack<Register> removeStorageRegisters(SignalDataStorage &storage, unsigned long* reg_table);
protected:
    template<typename T> inline RegisterPack<T> removeStorageRegisters(SignalDataStorage &storage)
    {
        R64FX_DEBUG_ASSERT(storage.type() == SignalDataStorage::Type(T()));
        return removeStorageRegisters(storage, registerTable(T())).bits;
    }


    /* Free registers. */
private:
    void freeRegisters(RegisterPack<Register> pack, unsigned long* reg_table, unsigned int reg_table_size);
protected:
    template<typename T> void freeRegisters(RegisterPack<T> regpack)
        { freeRegisters(regpack.bits, registerTable(T()), registerTableSize(T())); }


    /* Remove registers from storage and free them. */
    template<typename T> inline void freeStorageRegisters(SignalDataStorage &storage)
        { freeRegisters<T>(removeStorageRegisters<T>(storage)); }


    /* Free storage memory and(or) registers. */
    void freeStorage(SignalDataStorage &storage);


    friend class SignalGraph;
    friend class SignalGraphImpl;
};

}//namespace r64fx

#endif//R64FX_SIGNAL_GRAPH_HPP
