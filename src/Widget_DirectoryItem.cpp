#include "Widget_DirectoryItem.hpp"
#include "WidgetFlags.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

Widget_DirectoryItem::Widget_DirectoryItem(const std::string &caption, const std::string path, bool is_directory, Widget* parent)
: Widget_DataItem(caption, (is_directory ? Widget_DirectoryItem::Kind::Plain : Widget_DataItem::Kind::Tree), parent)
, m_path(path)
{

}


std::string Widget_DirectoryItem::path() const
{
    return m_path;
}


std::string Widget_DirectoryItem::fullPath() const
{
    std::string path = this->path();
    if(!path.empty() && path.back() != '/')
        path.push_back('/');

    if(!parent())
        return path;

    auto parent_browser = dynamic_cast<Widget_DirectoryItem*>(parent());
    if(!parent_browser)
        return path;

    return parent_browser->fullPath() + path;
}


void Widget_DirectoryItem::populate()
{
    Directory dir(fullPath());
    if(!dir.isOpen())
    {
        cerr << "Failed to load directory " << fullPath() << "\n";
        return;
    }

    dir.forEachEntry([](const Directory::Entry* entry, void* arg){
        auto self = (Widget_DirectoryItem*) arg;
        self->loadEntry(entry);
    }, this);

    resizeAndReallign(0);
    collapse();
}


void Widget_DirectoryItem::loadEntry(const Directory::Entry* entry)
{
    if(entry->name()[0] != '.')
    {
        Widget_DataItem* item;
        if(entry->isDirectory())
        {
            auto dt = new Widget_DirectoryItem(entry->name(), entry->name(), true);
            dt->populate();
            item = dt;
        }
        else
        {
            item = new Widget_DirectoryItem(entry->name(), entry->name(), false);
        }
        addItem(item);
    }
}

}//namespace r64fx