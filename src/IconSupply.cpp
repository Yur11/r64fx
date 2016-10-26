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
Color border ( 31,  31,  31, 0);
Color bright_border(63, 63, 63, 0);


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


void draw_rect_with_border(Image* img, int rx, int ry, int rw, int rh, Color &stroke, Color &fill)
{
    for(int y=1; y<(rh - 1); y++)
    {
        for(int x=1; x<(rw - 1); x++)
        {
            int xx = x + rx;
            int yy = y + ry;
            for(int c=0; c<4; c++)
            {
                img->pixel(xx, yy)[c] = fill[c];
            }
        }
    }

    int l = rx;
    int r = rx + rw - 1;
    for(int y=0; y<rh; y++)
    {
        int yy = y + ry;
        for(int c=0; c<4; c++)
        {
            img->pixel(l, yy)[c] = img->pixel(r, yy)[c] = stroke[c];
        }
    }

    int t = ry;
    int b = ry + rh - 1;
    for(int x=1; x<(rw - 1); x++)
    {
        int xx = x + rx;
        for(int c=0; c<4; c++)
        {
            img->pixel(xx, t)[c] = img->pixel(xx, b)[c] = stroke[c];
        }
    }
}


void draw_page_folded_corner(Image* img, int px, int py, int pw, int ph, int corner_size, Color &stroke, Color &background)
{
    for(int y=0; y<corner_size; y++)
    {
        for(int x=0; x<corner_size; x++)
        {
            int xx = x                     + px + pw - corner_size;
            int yy = (corner_size - y - 1) + py + ph - corner_size;
            auto &color = (x > y ? background : stroke);
            for(int c=0; c<4; c++)
            {
                img->pixel(xx, yy)[c] = color[c];
            }
        }
    }
}


void gen_icon_Page(Image* img, int size, bool highlighted)
{
    if(size < 8)
        return;
    
    int page_x = 1;
    int page_y = 0;
    int page_w = size - 2;
    int page_h = size;
    
    draw_rect_with_border(img, page_x, page_y, page_w, page_h, border, bright);
    
    int corner_size = size / 3;
    draw_page_folded_corner(img, page_x, page_y, page_w, page_h, corner_size, border, bg);
}


void draw_diskette_corner(Image* img, int dsk_x, int dsk_y, int dsk_w, int dsk_h, int corner_size, Color &stroke, Color &fill)
{
    for(int y=0; y<corner_size; y++)
    {
        for(int x=0; x<corner_size; x++)
        {
            int xx = x + dsk_x + dsk_w - corner_size;
            int yy = y + dsk_y;
            auto &color = (x == y || (x + 1) == y ? stroke : fill);
            for(int c=0; c<4; c++)
            {
                img->pixel(xx, yy)[c] = color[c];
            }
        }
    }
}


void gen_icon_Diskette(Image* img, int size, bool highlighted)
{
    if(size < 8)
        return;
    
    int dsk_x = 0;
    int dsk_y = 0;
    int dsk_w = size;
    int dsk_h = size;
    
    draw_rect_with_border(img, dsk_x, dsk_y, dsk_w, dsk_h, border, bg);
    
    int corner_size = size / 5;
    draw_diskette_corner(img, dsk_x, dsk_y, dsk_w, dsk_h, corner_size, border, bg);
    
    /* Diskette Label */
    if(size >= 16)
    {
        int offset = dsk_w / 5;
        int height = dsk_h / 2;
        draw_rect_with_border(img, dsk_x + offset, dsk_y + dsk_h - height, dsk_w - offset*2, height, border, bright);
    }
    
    /* Diskette Shutter */
    if(size >= 16)
    {
        int width  = (dsk_w / 2) - 1;
        int height = (dsk_h / 3) + 1;
        int offset = dsk_w / 4;
        fill(img, border, {dsk_x + offset, dsk_y, width, height});
        fill(img, bg, {dsk_x + offset + 1, dsk_y + 1, (width / 2) - 1, height - 2});
    }
}


void gen_icon_Folder(Image* img, int size, bool highlighted)
{
    if(size < 8)
        return;
    
    int folder_x = 0;
    int folder_y = size / 3;
    int folder_w = size;
    int folder_h = size / 2;
    
    draw_rect_with_border(img, folder_x, folder_y, folder_w, folder_h, border, bg);
    
    int tab_width = folder_w / 2;
    int tab_x = folder_x + folder_w / 5;
    
    for(int x=1; x<(tab_width - 1); x++)
    {
        int xx = x + tab_x;
        for(int c=0; c<4; c++)
        {
            img->pixel(xx, folder_y)[c] = bg[c];
        }
    }
    
    for(int c=0; c<4; c++)
    {
        img->pixel(tab_x, folder_y - 1)[c] = img->pixel(tab_x + tab_width - 1, folder_y - 1)[c] = border[c];
    }
    
    for(int x=0; x<tab_width; x++)
    {
        int xx = x + tab_x;
        for(int c=0; c<4; c++)
        {
            img->pixel(xx, folder_y - 2)[c] = border[c];
        }
    }
    
    if(tab_x >= 3)
    {
        for(int c=0; c<4; c++)
        {
            img->pixel(folder_x, folder_y - 1)[c] = img->pixel(folder_x + folder_w - 1, folder_y - 1)[c] = bright_border[c];
        }
        
        for(int x=0; x<(tab_x - 1); x++)
        {
            int xx = x + folder_x;
            for(int c=0; c<4; c++)
            {
                img->pixel(xx, folder_y - 2)[c] = bright_border[c];
            }
        }
        
        for(int x=(tab_x + tab_width + 1); x<folder_w; x++)
        {
            for(int c=0; c<4; c++)
            {
                img->pixel(x, folder_y - 2)[c] = bright_border[c];
            }
        }
        
        for(int x=(tab_x - 2); x<=(tab_x + tab_width + 1); x++)
        {
            int xx = x + folder_x;
            for(int c=0; c<4; c++)
            {
                img->pixel(xx, folder_y - 4)[c] = bright_border[c];
            }
        }
        
        for(int c=0; c<4; c++)
        {
            img->pixel(tab_x - 2, folder_y - 3)[c] = img->pixel(tab_x + tab_width + 1, folder_y - 3)[c] = bright_border[c];
        }
        
        
        for(int x=1; x<(tab_x); x++)
        {
            int xx = x + folder_x;
            for(int c=0; c<4; c++)
            {
                img->pixel(xx, folder_y - 1)[c] = bright[c];
            }
        }
        
        for(int x=(tab_x + tab_width); x<(folder_w - 1); x++)
        {
            for(int c=0; c<4; c++)
            {
                img->pixel(x, folder_y - 1)[c] = bright[c];
            }
        }
        
        for(int x=(tab_x - 1); x<=(tab_x + tab_width); x++)
        {
            int xx = x + folder_x;
            for(int c=0; c<4; c++)
            {
                img->pixel(xx, folder_y - 3)[c] = bright[c];
            }
        }
        
        for(int c=0; c<4; c++)
        {
            img->pixel(tab_x - 1, folder_y - 2)[c] = img->pixel(tab_x + tab_width, folder_y - 2)[c] = bright[c];
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
        
        case IconName::Folder:
        {
            gen_icon_Folder(&(entry->img), size, highlighted);
            break;
        }
        
        case IconName::Diskette:
        {
            gen_icon_Diskette(&(entry->img), size, highlighted);
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
