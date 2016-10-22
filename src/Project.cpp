#include "Project.hpp"

namespace r64fx{

Project::Project()
{
    m_view = new ProjectView;
}


Project::~Project()
{
    m_view->setParent(nullptr);
    delete m_view;
}


ProjectView* Project::view() const
{
    return m_view;
}


void Project::addItem(ProjectItem* item)
{
    m_items.append(item);
}


void Project::removeItem(ProjectItem* item)
{
    m_items.remove(item);
}


LinkedList<ProjectItem>::Iterator Project::begin() const
{
    return m_items.begin();
}


LinkedList<ProjectItem>::Iterator Project::end() const
{
    return m_items.end();
}


void Project::clear()
{
    m_items.clear();
}


void Project::setName(const std::string &name)
{
    m_name = name;
}


std::string Project::name() const
{
    return m_name;
}


ProjectItem::ProjectItem()
{

}


ProjectItem::~ProjectItem()
{

}

}//namespace r64fx
