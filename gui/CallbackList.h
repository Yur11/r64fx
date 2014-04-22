#ifndef R64FX_GUI_CALLBACK_LIST_H
#define R64FX_GUI_CALLBACK_LIST_H

#include "Callback.h"
#include <vector>

namespace r64fx{
    
class CallbackList : public std::vector<Callback>{
    
public:
    void exec();

    inline void push_back(Callback::Fun fun, void* data) { std::vector<Callback>::push_back(Callback(fun, data)); }
};
    
}//namespace r64fx

#endif//R64FX_GUI_CALLBACK_LIST_H