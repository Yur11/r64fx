#ifndef R64FX_WIDGET_DIRECTORY_BROWSER_HPP
#define R64FX_WIDGET_DIRECTORY_BROWSER_HPP

#include "Widget_DataItem.hpp"
#include "Directory.hpp"

namespace r64fx{

class Widget_DirectoryItem : public Widget_DataItem{
    std::string m_path = "";

public:
    Widget_DirectoryItem(const std::string &caption, const std::string path = "", Widget* parent = nullptr);

    virtual ~Widget_DirectoryItem();

    std::string path() const;

    std::string fullPath() const;

    bool isDirectory() const;

    bool isPopulated() const;

    virtual void collapse();

    virtual void expand();

    void copyToClipboard();

protected:
    virtual void showContextMenu(Point<int> position);

    virtual void keyPressEvent(KeyPressEvent* event);

    virtual void clipboardDataRecieveEvent(ClipboardDataRecieveEvent* event);

    virtual void clipboardDataTransmitEvent(ClipboardDataTransmitEvent* event);

    virtual void clipboardMetadataRecieveEvent(ClipboardMetadataRecieveEvent* event);

    virtual void getClipboardMetadata(ClipboardMetadata &metadata);

private:
    void populate();

    void depopulate();

    void loadEntry(const Directory::Entry* entry);
};

}//namespace r64fx

#endif//R64FX_WIDGET_DIRECTORY_BROWSER_HPP