#include "Widget_DirectoryBrowser.hpp"
#include "WidgetFlags.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

Widget_DirectoryBrowser::Widget_DirectoryBrowser(const std::string &caption, const std::string path, Widget* parent)
: Widget_ItemTree(caption, parent)
, m_path(path)
{

}


std::string Widget_DirectoryBrowser::path() const
{
    return m_path;
}


std::string Widget_DirectoryBrowser::fullPath() const
{
    std::string path = this->path();
    if(!path.empty() && path.back() != '/')
        path.push_back('/');

    if(!parent())
        return path;

    auto parent_browser = dynamic_cast<Widget_DirectoryBrowser*>(parent());
    if(!parent_browser)
        return path;

    return parent_browser->fullPath() + path;
}


void Widget_DirectoryBrowser::populate()
{
    Directory dir(fullPath());
    if(!dir.isOpen())
    {
        cerr << "Failed to load directory " << fullPath() << "\n";
        return;
    }

    dir.forEachEntry([](const Directory::Entry* entry, void* arg){
        auto self = (Widget_DirectoryBrowser*) arg;
        self->loadEntry(entry);
    }, this);

    resizeAndReallign();
    collapse();
}


void Widget_DirectoryBrowser::loadEntry(const Directory::Entry* entry)
{
    if(entry->name()[0] != '.')
    {
        Widget_DataItem* item;
        if(entry->isDirectory())
        {
            auto browser = new Widget_DirectoryBrowser(entry->name(), fullPath() + entry->name());
            browser->populate();
            item = browser;
        }
        else
        {
            item = new Widget_DataItem(entry->name());
        }
        addItem(item);
    }
}

}//namespace r64fx