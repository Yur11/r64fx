#ifndef R64FX_MACHINE_MODEL_HPP
#define R64FX_MACHINE_MODEL_HPP

namespace r64fx{

class HostModel;

class MachineModel{
    HostModel* m_host = nullptr;

public:
    MachineModel(HostModel* host);
};

}//namespace r64fx

#endif//R64FX_MACHINE_MODEL_HPP
