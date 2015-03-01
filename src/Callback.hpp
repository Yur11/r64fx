#ifndef R64FX_GUI_CALLBACK_H
#define R64FX_GUI_CALLBACK_H

namespace r64fx{
  
class Callback{
public:
    typedef void (*Fun)(void* data);
    
    Callback(Fun fun, void* data)
    : _fun(fun)
    , _data(data)
    {}
    
    inline void exec() { _fun(_data); }
    
private:
    Fun _fun;
    void*  _data;
};
    
}//namespace r64fx

#endif//R64FX_GUI_CALLBACK_H