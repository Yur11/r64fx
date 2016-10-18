#include "Project.hpp"

namespace r64fx{

Project::Project()
{

}


Project::~Project()
{

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


ProjectItem::ProjectItem()
{

}


ProjectItem::~ProjectItem()
{

}

}//namespace r64fx
