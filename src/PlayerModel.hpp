#ifndef R64FX_PLAYER_MODEL_HPP
#define R64FX_PLAYER_MODEL_HPP

#include "MachineModel.hpp"

namespace r64fx{
    
class PlayerProcessor;
    
class PlayerModel : public MachineModel{
    PlayerProcessor* m_processor = nullptr;
    
public:
    PlayerModel();
    
    virtual ~PlayerModel();
    
private:
    virtual void dispatchMessage(const ProcessorMessage &msg);
    
    virtual MachineProcessor* processor();
};
    
}//namespace r64fx

#endif//R64FX_PLAYER_MODEL_HPP