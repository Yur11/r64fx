#include "IconSupply.hpp"
#include "LinkedList.hpp"
#include "Image.hpp"
#include "ImageUtils.hpp"

namespace r64fx{

namespace{
    
struct IconEntry : public LinkedList<IconEntry>::Node{
    Image    img;
    IconName name;
    int      size;
    bool     highlighted;
};

LinkedList<IconEntry> g_icons;

Color bg     (127, 127, 127, 0);
Color bright (223, 223, 223, 0);
Color border ( 63,  63,  63, 0);


IconEntry* find_existing_icon(IconName name, int size, bool highlighted)
{
    for(auto entry : g_icons)
    {
        if(entry->name == name && entry->size == size && entry->highlighted == highlighted)
        {
            return entry;
        }
    }
    return nullptr;
}


IconEntry* find_existing_icon(Image* img)
{
    for(auto entry : g_icons)
    {
        if(&(entry->img) == img)
        {
            return entry;
        }
    }
    return nullptr;
}


void gen_icon_Page(Image* img, int size, bool highlighted)
{
    if(size < 8)
        return;
    
    int page_x = 2;
    int page_y = 1;
    int page_w = size - 4;
    int page_h = size - 2;

    for(int y=1; y<(page_h - 1); y++)
    {
        for(int x=1; x<(page_w - 1); x++)
        {
            int xx = x + page_x;
            int yy = y + page_y;
            for(int c=0; c<4; c++)
            {
                img->pixel(xx, yy)[c] = bright[c];
            }
        }
    }

    int l = page_x;
    int r = page_x + page_w - 1;
    for(int y=0; y<page_h; y++)
    {
        int yy = y + page_y;
        for(int c=0; c<4; c++)
        {
            img->pixel(l, yy)[c] = img->pixel(r, yy)[c] = border[c];
        }
    }

    int t = page_y;
    int b = page_y + page_h - 1;
    for(int x=1; x<(page_w - 1); x++)
    {
        int xx = x + page_x;
        for(int c=0; c<4; c++)
        {
            img->pixel(xx, t)[c] = img->pixel(xx, b)[c] = border[c];
        }
    }
    
    int corner_size = size / 3;
    
    for(int y=0; y<corner_size; y++)
    {
        for(int x=0; x<corner_size; x++)
        {
            int xx = x                     + page_x + page_w - corner_size;
            int yy = (corner_size - y - 1) + page_y + page_h - corner_size;
            auto &color = (x > y ? bg : border);
            for(int c=0; c<4; c++)
            {
                img->pixel(xx, yy)[c] = color[c];
            }
        }
    }
}


IconEntry* gen_new_icon(IconName name, int size, bool highlighted)
{
    if(size <= 0)
        return nullptr;
    
    auto entry = new IconEntry;
    entry->name = name;
    entry->size = size;
    entry->highlighted = highlighted;
    entry->img.load(size, size, 4);
    fill(&(entry->img), bg);
    switch(name)
    {
        case IconName::Page:
        {
            gen_icon_Page(&(entry->img), size, highlighted);
            break;
        }
        
        default:
            break;
    }
    return entry;
}
    
}//namespace
    
Image* get_icon(IconName name, int size, bool highlighted)
{
    auto entry = find_existing_icon(name, size, highlighted);
    if(!entry)
    {
        entry = gen_new_icon(name, size, highlighted);
    }
    return &(entry->img);
}

void free_icon(Image* icon_image)
{
    auto entry = find_existing_icon(icon_image);
    if(entry)
    {
        entry->img.free();
        delete entry;
    }
}
    
}//namespace r64fx
