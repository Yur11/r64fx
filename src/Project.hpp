#ifndef R64FX_PROJECT_HPP
#define R64FX_PROJECT_HPP

#include "LinkedList.hpp"
#include "ProjectView.hpp"

namespace r64fx{

class ProjectItem;

class Project : public LinkedList<Project>::Node{
    LinkedList<ProjectItem> m_items;
    ProjectView* m_view = nullptr;
    std::string m_name = "";

public:
    Project();

    virtual ~Project();

    ProjectView* view() const;
    
    void addItem(ProjectItem* item);

    void removeItem(ProjectItem* item);

    LinkedList<ProjectItem>::Iterator begin() const;

    LinkedList<ProjectItem>::Iterator end() const;

    void clear();

    void setName(const std::string &name);

    std::string name() const;
};


class ProjectItem : public LinkedList<ProjectItem>::Node{
public:
    ProjectItem();

    virtual ~ProjectItem();
};

}//namespace r64fx

#endif//R64FX_PROJECT_HPP
