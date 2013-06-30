#ifndef R64FX_GUI_ACTION_H
#define R64FX_GUI_ACTION_H

#include "Message.h"
#include "Texture.h"
#include <string>

namespace r64fx{

/** @brief An action that can be taken by the user. */
class Action{
    Texture* _icon_texture;
    std::string _name;
    Message _message;

public:
    Action(Texture* icon_texture, std::string name, Message message = Message())
    : _icon_texture(icon_texture)
    , _name(name)
    , _message(message)
    {}

    Action(std::string name, Message message = Message())
    : _icon_texture(Texture::transparent16x16())
    , _name(name)
    , _message(message)
    {}

    inline void setIconTexture(Texture* texture) { _icon_texture = texture; }
    
    inline Texture* iconTexture() const { return _icon_texture; }

    inline void setName(std::string name) { _name = name; }

    inline std::string name() const { return _name; }

    inline void setMessage(Message message) { _message = message; }

    inline Message message() const { return _message; }

    inline void trigger() { _message.send(this); }
};

}//namespace r64fx

#endif//R64FX_GUI_ACTION_H