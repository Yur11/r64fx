#ifndef R64FX_HOST_MODEL_HPP
#define R64FX_HOST_MODEL_HPP

namespace r64fx{

class HostModelPrivate;
class MachineModel;

class HostModel{
    HostModelPrivate* m = nullptr;
    
public:
    HostModel();

    ~HostModel();

    void deployMachine(MachineModel* machine);
};

}//namespace r64fx

#endif//R64FX_HOST_MODEL_HPP