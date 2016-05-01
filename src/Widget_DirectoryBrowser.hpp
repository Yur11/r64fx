#ifndef R64FX_WIDGET_DIRECTORY_BROWSER_HPP
#define R64FX_WIDGET_DIRECTORY_BROWSER_HPP

#include "Widget_ItemTree.hpp"
#include "Directory.hpp"

namespace r64fx{

class Widget_DirectoryBrowser : public Widget_ItemTree{
    std::string m_path = "";

public:
    Widget_DirectoryBrowser(const std::string &caption, const std::string path, Widget* parent = nullptr);

    std::string path() const;

    std::string fullPath() const;

    void populate();

private:
    void loadEntry(const Directory::Entry* entry);
};

}//namespace r64fx

#endif//R64FX_WIDGET_DIRECTORY_BROWSER_HPP