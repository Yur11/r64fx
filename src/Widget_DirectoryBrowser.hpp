#ifndef R64FX_WIDGET_DIRECTORY_BROWSER_HPP
#define R64FX_WIDGET_DIRECTORY_BROWSER_HPP

#include "Widget_ItemTree.hpp"
#include "Directory.hpp"

namespace r64fx{

class Widget_DirectoryBrowser : public Widget_ItemTree{

public:
    Widget_DirectoryBrowser(const std::string &path, Widget* parent = nullptr);

    void populate();

private:
    void loadEntry(const Directory::Entry* entry);
};

}//namespace r64fx

#endif//R64FX_WIDGET_DIRECTORY_BROWSER_HPP