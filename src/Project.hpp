#ifndef R64FX_PROJECT_HPP
#define R64FX_PROJECT_HPP

#include "LinkedList.hpp"

namespace r64fx{

class Project : public LinkedList<Project>::Node{
public:
    Project();

    virtual ~Project();
};

}//namespace r64fx

#endif//R64FX_PROJECT_HPP
