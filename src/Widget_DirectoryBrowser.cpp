#include "Widget_DirectoryBrowser.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

Widget_DirectoryBrowser::Widget_DirectoryBrowser(const std::string &path, Widget* parent)
: Widget_ItemTree(path, parent)
{

}


void Widget_DirectoryBrowser::populate()
{
    Directory dir(text());
    if(!dir.isOpen())
    {
        cerr << "Failed to load directory " << text() << "\n";
        return;
    }

    dir.forEachEntry([](const Directory::Entry* entry, void* arg){
        auto self = (Widget_DirectoryBrowser*) arg;
        self->loadEntry(entry);
    }, this);
}


void Widget_DirectoryBrowser::loadEntry(const Directory::Entry* entry)
{
    if(entry->name()[0] != '.')
    {
        addItem(new Widget_DirectoryBrowser(entry->name()));
    }
}

}//namespace r64fx