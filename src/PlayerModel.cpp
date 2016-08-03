#include "PlayerModel.hpp"
#include "MachineProcessor.hpp"
#include "ProcessorMessage.hpp"

#include <iostream>
using namespace std;

namespace r64fx{
    
class PlayerProcessor : public MachineProcessor{
public:
    PlayerProcessor(MachineModel* model) : MachineProcessor(model) {}
    
    virtual void deploy();
        
    virtual void detach();
        
    virtual void dispatchMessage(const ProcessorMessage &msg);
};
    
    
PlayerModel::PlayerModel()
{
    m_processor = new PlayerProcessor(this);
}


PlayerModel::~PlayerModel()
{
    if(m_processor)
        delete m_processor;
}


void PlayerModel::dispatchMessage(const ProcessorMessage &msg)
{
    cout << "PlayerModel::dispatchMessage()\n";
}
    
    
MachineProcessor* PlayerModel::processor()
{
    return m_processor;
}


void PlayerProcessor::deploy()
{
    cout << "PlayerModel::deploy()\n";
}
    
    
void PlayerProcessor::detach()
{
    cout << "PlayerModel::detach()\n";
}
    
    
void PlayerProcessor::dispatchMessage(const ProcessorMessage &msg)
{
    cout << "PlayerModel::dispatchMessage()\n";
}
    
}//namespace r64fx