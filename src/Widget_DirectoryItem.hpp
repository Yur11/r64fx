#ifndef R64FX_WIDGET_DIRECTORY_BROWSER_HPP
#define R64FX_WIDGET_DIRECTORY_BROWSER_HPP

#include "Widget_DataItem.hpp"
#include "Directory.hpp"

namespace r64fx{

class Widget_DirectoryItem : public Widget_DataItem{
    std::string m_path = "";

public:
    Widget_DirectoryItem(const std::string &caption, const std::string path, bool is_directory, Widget* parent = nullptr);

    std::string path() const;

    std::string fullPath() const;

    bool isDirectory() const;

    void populate();

private:
    void loadEntry(const Directory::Entry* entry);
};

}//namespace r64fx

#endif//R64FX_WIDGET_DIRECTORY_BROWSER_HPP