#ifndef R64FX_SIGNAL_GRAPH_HPP
#define R64FX_SIGNAL_GRAPH_HPP

#include "Debug.hpp"
#include "jit.hpp"

/* Convenience macros for declaring SignalNode ports. */
#define R64FX_NODE_SOURCE(name) private: SignalSource m_##name; public: inline NodeSource name() { return {this, &m_##name}; } private:
#define R64FX_NODE_SINK(name)   private: SignalSink   m_##name; public: inline NodeSink   name() { return {this, &m_##name}; } private:


namespace r64fx{

class SignalGraph;
class SignalNode;


enum class SignalDataType : unsigned long{
    Bad, Float32, Float64, Int32, Int64
};

template<typename T>
           inline SignalDataType signal_data_type         (){ return SignalDataType::Bad; }
template<> inline SignalDataType signal_data_type<float>  (){ return SignalDataType::Float32; }
template<> inline SignalDataType signal_data_type<double> (){ return SignalDataType::Float64; }
template<> inline SignalDataType signal_data_type<int>    (){ return SignalDataType::Int32; }
template<> inline SignalDataType signal_data_type<long>   (){ return SignalDataType::Int64; }


enum class SignalRegisterType : unsigned long{
    Bad, GPR, Xmm, Ymm
};

template<typename RegisterT>
           inline SignalRegisterType register_type        (){ return SignalRegisterType::Bad; }
template<> inline SignalRegisterType register_type<GPR64> (){ return SignalRegisterType::GPR; }
template<> inline SignalRegisterType register_type<Xmm>   (){ return SignalRegisterType::Xmm; }
template<> inline SignalRegisterType register_type<Ymm>   (){ return SignalRegisterType::Ymm; }


class DataBufferPointer{
    unsigned int m_offset = 0;

public:
    DataBufferPointer() {}

    DataBufferPointer(unsigned int offset) : m_offset(offset) {}

    inline unsigned int offset() const { return m_offset; }

    inline operator bool() const { return m_offset; }
};


template<typename RegisterT> class RegisterPack{
    unsigned long m_bits = 0;

public:
    RegisterPack() {}

    RegisterPack(std::initializer_list<RegisterT> regs)
    {
        R64FX_DEBUG_ASSERT(regs.size() < 16);
        unsigned int i = 0;
        for(auto reg : regs)
        {
            setRegAt(i++, reg);
        }
        setSize(regs.size());
    }

    template<typename OtherT> explicit
    RegisterPack(RegisterPack<OtherT> other) : m_bits(other.bits()) {}

    inline unsigned long bits() const { return m_bits; }

    inline operator bool() const { return m_bits; }

private:
    inline void setSize(unsigned int size)
        { R64FX_DEBUG_ASSERT(size < 16); m_bits &= ~(0xFUL << 60); m_bits |= (((unsigned long)size) << 60); }
public:
    inline unsigned int size() const { return (m_bits>>60) & 0xFUL; }

private:
    inline void setRegAt(unsigned int i, RegisterT reg)
    {
        R64FX_DEBUG_ASSERT(i < 16);
        m_bits &= ~(0xFUL << (i<<2));
        m_bits |= (((unsigned long)reg.code()) << (i<<2));
    }

public:
    inline RegisterT regAt(unsigned int i) const
    {
        R64FX_DEBUG_ASSERT(i < size());
        return RegisterT((m_bits >> (i<<2)) & 0xFUL);
    }

    inline RegisterT operator[](unsigned int i) const { return regAt(i); }

    inline operator RegisterT() const { return regAt(0); }

    friend class SignalNode;
};


/* A class used for sharing intermediate data between SignalNode instances.
 * It represents vectors of data that are stored in registers and/or memory. */
class SignalDataStorage{
    unsigned long m = 0;

    /* SignalDataStorage always uses a particular reigister type. */
private:
    inline void setRegisterType(SignalRegisterType regtype) { m &= ~(3UL<<62); m |= (((unsigned long)(regtype))<<62); }
public:
    inline SignalRegisterType registerType() const { return (SignalRegisterType)((m>>62) & 3); }

    /* Size of current register type in bytes. */
    inline unsigned int registerSize() const { static char s[4] = {0, 8, 16, 32}; return s[(unsigned long)registerType()]; }

    /* Storage size expressed as a number of registers. */
private:
    inline void setSize(unsigned int size) { R64FX_DEBUG_ASSERT(size < ((1UL<<32)-1)); m &= ~((1UL<<32)-1); m |= size;}
public:
    inline unsigned int size() const { return m & ((1UL<<32)-1); }

    inline bool empty() const { return size() == 0; }


    /* True if at least some of the data is stored in registers. */
private:
    constexpr static unsigned long HasRegistersBit = (1UL << 61);
public:
    inline bool hasRegisters() const { return m & HasRegistersBit; }


    /* Prevent storage registers from being reallocated by allocRegisters() method. */
private:
    constexpr static unsigned long LockedBit = (1UL << 60);
public:
    inline void lock()     { m |= LockedBit; }
    inline void unlock()   { m &= ~LockedBit; }
    inline bool isLocked() { return m & LockedBit; }


private:
    inline void setMemory(DataBufferPointer ptr)
        { R64FX_DEBUG_ASSERT(ptr.offset() < 0xFFFFFF); m &= ~(0xFFFFFFUL<<32); m |= ((unsigned long)(ptr.offset()))<<32; }
public:
    inline DataBufferPointer memory() const { return DataBufferPointer((m>>32) & 0xFFFFFF); }

    inline bool hasMemory() const { return memory(); }


    /* Total size in bytes. */
    inline unsigned int totalSize() const { return size() * registerSize(); }

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

    inline operator SignalNode*() { return m_node; }

    inline SignalPortT* port() const { return m_port; }

    inline operator SignalPortT*() { return m_port; }
};

typedef NodePort<SignalSource> NodeSource;
typedef NodePort<SignalSink> NodeSink;


/* Common implementation structure used by SignalGraph and SignalNode instanes.*/
class SignalGraphImpl : public AssemblerBuffers{
    friend class SignalGraph;
    friend class SignalNode;
    friend class SignalGraphImplVar;

    unsigned long  iteration_count  = 0;
    unsigned long  frame_count      = 0;

    unsigned long  gprs[16];
    unsigned long  xmms[16];

    SignalGraphImpl();

    void buildNode(SignalNode &node);

    inline HeapBuffer heapBuffer() const { return HeapBuffer(dataBegin(), dataBufferSize()); }

    DataBufferPointer allocMemoryBytes(unsigned int nbytes, unsigned int align);

    inline void freeMemory(DataBufferPointer ptr) { heapBuffer().freeChunk(codeBegin() - ptr.offset()); }

    template<typename T = float> inline T* addr(DataBufferPointer ptr)
    {
        R64FX_DEBUG_ASSERT(ptr.offset() > 0);
        return (T*)(codeBegin() - ptr.offset());
    }
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
    SignalGraph()
    {
        m.rewindCode();
        RET();
    }

    /* Connect two nodes together. */
    void link(const NodeSource &node_source, const NodeSink &node_sink);

    /* Disconnect node from its source. */
    void unlink(const NodeSink node_sink);

    /* Build signal graph function. */
    void build(SignalNode* terminal_node);

    /* Run signal graph function. */
    inline long run() { return ((long (*)())m.codeBegin())(); }

    /* Number of frames in a single buffer. */
    inline unsigned long frameCount() const { return m.frame_count; }

    inline void setFrameCount(unsigned long frame_count) { m.frame_count = frame_count; }

    inline DataBufferPointer allocBuffer()
    {
        R64FX_DEBUG_ASSERT(frameCount() > 0);
        unsigned int nbytes = frameCount() * sizeof(float);
        return m.allocMemoryBytes(nbytes, nbytes >= 64 ? 64 : nbytes);
    }

    inline void freeBuffer(DataBufferPointer ptr) { m.freeMemory(ptr); }

    template<typename T = float> inline T* addr(DataBufferPointer ptr) { return m.addr<T>(ptr); }

    friend class SignalNode;
};


/* Base class for SignalGraph nodes. */
class SignalNode : protected AssemblerInstructions<SignalGraphImplRef>{
    unsigned long m_iteration_count = 0;
    unsigned long m_link_count = 0;

public:
    /* All nodes must exist within the context of a parent graph. */
    SignalNode(SignalGraph &sg)
    : AssemblerInstructions(sg.m)
    , m_iteration_count(sg.m.iteration_count)
    {}

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


    inline unsigned long frameCount() const { return m.frame_count; }


    /* Allocate some memory in the data buffer. */
    template<typename T> inline DataBufferPointer allocMemory(unsigned int nitems) { return allocMemory<T>(nitems, sizeof(T)); }

    template<typename T> inline DataBufferPointer allocMemory(unsigned int nitems, unsigned align)
    {
        R64FX_DEBUG_ASSERT(!(align & (align - 1)));
        R64FX_DEBUG_ASSERT(align >= sizeof(T));
        m.allocMemoryBytes(nitems * sizeof(T), align);
    }


    template<typename T = float> inline T* addr(DataBufferPointer ptr) const { return m.addr<T>(ptr); }

    template<typename T = float> inline T* addr(SignalDataStorage storage) const
    {
        return addr<T>(storage.memory());
    }


    /* Get memory assigned to storage. */
    inline DataBufferPointer getStorageMemory(SignalDataStorage &storage) const { return storage.memory(); }

    /* Detach memory from storage but do not free it. */
    inline DataBufferPointer removeStorageMemory(SignalDataStorage &storage)
        { auto val = storage.memory(); storage.setMemory(0); return val; }

    /* Detach memory from storage and free it. */
    inline void freeStorageMemory(SignalDataStorage &storage) { freeMemory(removeStorageMemory(storage)); }

    inline void freeMemory(DataBufferPointer ptr) { m.freeMemory(ptr); }


    /* Registers */
private:
    struct RegisterTable{
        unsigned long*       regs = nullptr;
        const unsigned long  size = 0;

        RegisterTable(unsigned long* regs, unsigned long size) : regs(regs), size(size) {}
        inline unsigned long &operator[](unsigned int i) { R64FX_DEBUG_ASSERT(i < size); return regs[i]; }
    };
    inline RegisterTable registerTable(GPR64) const { return {m.gprs, sizeof(m.gprs) >> 3}; }
    inline RegisterTable registerTable(Xmm)   const { return {m.xmms, sizeof(m.xmms) >> 3}; }


    /* Allocate registers. */
    RegisterPack<Register> allocRegisters(unsigned int count, RegisterTable rt);
protected:
    template<typename RegisterT> inline RegisterPack<RegisterT> allocRegisters(unsigned int count)
        { return RegisterPack<RegisterT>(allocRegisters(count, registerTable(RegisterT()))); }


    /* Retrieve registers from storage. */
private:
    RegisterPack<Register> getStorageRegisters(SignalDataStorage &storage, RegisterTable rt) const;
protected:
    template<typename RegisterT> inline RegisterPack<RegisterT> getStorageRegisters(SignalDataStorage &storage) const
    {
        R64FX_DEBUG_ASSERT(storage.registerType() == register_type<RegisterT>());
        return RegisterPack<RegisterT>(getStorageRegisters(storage, registerTable(RegisterT())));
    }

private:
    RegisterPack<Register> removeStorageRegisters(SignalDataStorage &storage, RegisterTable rt);
public:
    template<typename RegisterT> inline RegisterPack<RegisterT> removeStorageRegisters(SignalDataStorage &storage)
    {
        R64FX_DEBUG_ASSERT(storage.registerType() == register_type<RegisterT>());
    }


    /* Free registers. */
private:
    void freeRegisters(RegisterPack<Register> pack, RegisterTable rt);
protected:
    template<typename RegisterT> void freeRegisters(RegisterPack<RegisterT> regpack)
        { freeRegisters(regpack.bits, registerTable(RegisterT())); }


private:
    void initStorage_(SignalDataStorage &storage, DataBufferPointer memptr, unsigned int memsize, RegisterPack<Register> regpack, RegisterTable rt);
public:
    template<typename DataT, typename RegisterT> inline
    void initStorage(
        SignalDataStorage &storage, DataBufferPointer memptr, unsigned int memsize, RegisterPack<RegisterT> regpack)
    {
        R64FX_DEBUG_ASSERT(storage.empty());
        storage.setRegisterType(register_type<RegisterT>());
        initStorage_(storage, memptr, memsize, RegisterPack<Register>(regpack), registerTable(RegisterT()));
    }

    template<typename DataT, typename RegisterT> inline
    void initStorage(SignalDataStorage &storage, RegisterPack<RegisterT> regpack)
    {
        initStorage<DataT, RegisterT>(storage, DataBufferPointer(), regpack.size(), regpack);
    }

    template<typename DataT, typename RegisterT> inline
    void initStorage(SignalDataStorage &storage, DataBufferPointer memptr, unsigned int memsize)
    {
        initStorage<DataT, RegisterT>(storage, memptr, memsize, RegisterPack<RegisterT>());
    }

    template<typename DataT, typename RegisterT> inline
    void initStorage(SignalDataStorage &storage, RegisterT reg)
    {
        initStorage<DataT, RegisterT>(storage, RegisterPack<RegisterT>({reg}));
    }

    /* Free storage memory and(or) registers. */
    void freeStorage(SignalDataStorage &storage);


    friend class SignalGraph;
    friend class SignalGraphImpl;
};

}//namespace r64fx

#endif//R64FX_SIGNAL_GRAPH_HPP
