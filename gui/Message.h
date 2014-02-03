#ifndef R64FX_GUI_MESSAGE_H
#define R64FX_GUI_MESSAGE_H

namespace r64fx{
    
/** @brief Message passed between objects. */
struct Message{
    typedef void* (*Callback)(void* source, void* data);
    
    /** @brief Message callback. */
    Callback callback;
    
    /** @brief Extra data passed with the message. */
    void* data;
    
    Message(Callback callback, void* data = nullptr)
    : callback(callback)
    , data(data)
    {}
    
    Message();
    
    /** @brief Send a message  */
    inline void* send(void* caller) { return callback(caller, data); }
    
    inline void* operator()(void* caller) { return callback(caller, data); }
    
    void clear();
};
    
}//namespace r64fx

#endif//R64FX_GUI_MESSAGE_H
