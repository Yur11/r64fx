#ifndef R64FX_MACHINE_POOL_HPP
#define R64FX_MACHINE_POOL_HPP

#include "Machine.hpp"
#include "MachineConnection.hpp"

namespace r64fx{

class MachinePoolPrivate;
    
class MachinePool{
    friend class Machine;
    
    MachinePoolPrivate* m = nullptr;
    
public:
    MachinePool();
    
    virtual ~MachinePool();
    
    LinkedList<Machine>::Iterator begin() const;
    
    LinkedList<Machine>::Iterator end() const;
    
    void withdrawAll();
    
    MachineConnection* makeConnection(
        MachineSignalSource*  src_port, 
        MachineSignalSink*    dst_port, 
        MachineConnection::Mapping mapping = MachineConnection::Mapping::Default
    );
    
    void breakConnection(MachineConnection* connection);
    
    void updateConnection(MachineConnection* connection);
};
    
}//namespace r64fx

#endif//R64FX_MACHINE_POOL_HPP