#ifndef R64FX_MODULE_HPP
#define R64FX_MODULE_HPP

#include "LinkedList.hpp"

namespace r64fx{

class Module : public LinkedList<Module>::Node{
    long                m_id = 0;
    LinkedList<Module>  m_children;

public:
    inline long id() const { return m_id; }

    void engage();

    void disengage();
};

}//namespace r64fx

#endif//R64FX_MODULE_HPP
