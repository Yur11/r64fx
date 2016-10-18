#ifndef R64FX_PROJECT_HPP
#define R64FX_PROJECT_HPP

#include "LinkedList.hpp"

namespace r64fx{

class ProjectItem;

class Project : public LinkedList<Project>::Node{
    LinkedList<ProjectItem> m_items;

public:
    Project();

    virtual ~Project();

    void addItem(ProjectItem* item);

    void removeItem(ProjectItem* item);

    LinkedList<ProjectItem>::Iterator begin() const;

    LinkedList<ProjectItem>::Iterator end() const;

    void clear();
};


class ProjectItem : public LinkedList<ProjectItem>::Node{
public:
    ProjectItem();

    virtual ~ProjectItem();
};

}//namespace r64fx

#endif//R64FX_PROJECT_HPP
