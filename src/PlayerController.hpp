#ifndef R64FX_PLAYER_CONTROLLER_HPP
#define R64FX_PLAYER_CONTROLLER_HPP

namespace r64fx{
    
class PlayerControllerPrivate;
    
class PlayerController{
    PlayerControllerPrivate* m = nullptr;
    
public:
    PlayerController();
    
    virtual ~PlayerController();
    
    void exec();
};
    
}//namespace r64fx

#endif//R64FX_PLAYER_CONTROLLER_HPP