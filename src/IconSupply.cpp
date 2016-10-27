#include "IconSupply.hpp"
#include "LinkedList.hpp"
#include "Image.hpp"
#include "ImageUtils.hpp"

namespace r64fx{

bool operator!=(const IconColors &a, const IconColors &b)
{
    return a.stroke1 != b.stroke1 || a.fill1 != b.fill1 || a.stroke2 != b.stroke2 || a.fill2 != b.fill2;
}


namespace{
    
struct IconEntry : public LinkedList<IconEntry>::Node{
    unsigned long user_count = 0;

    Image       img;
    IconName    name;
    int         size;
    IconColors* colors;
};

LinkedList<IconEntry> g_icons;


IconEntry* find_existing_icon(IconName name, int size, IconColors* ic)
{
    for(auto entry : g_icons)
    {
        if(entry->name == name && entry->size == size && entry->colors[0] == ic[0])
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


void gen_icon_Page(Image* img, int size, IconColors* ic)
{
    if(size < 8)
        return;
    
    int page_x = 1;
    int page_y = 0;
    int page_w = size - 2;
    int page_h = size;
    
    draw_rect_with_border(img, page_x, page_y, page_w, page_h, ic->stroke1, ic->fill2);
    
    int corner_size = size / 3;
    draw_page_folded_corner(img, page_x, page_y, page_w, page_h, corner_size, ic->stroke1, ic->fill1);
}


void gen_icon_DoublePage(Image* img, int size, IconColors* ic)
{
    if(size < 8)
        return;
    
    int page_x = 1;
    int page_y = 0;
    int page_w = size - 2;
    int page_h = size;
    
    int offset = size / 8;
    
    draw_rect_with_border(img, page_x         , page_y         , page_w - offset, page_h - offset, ic->stroke1, ic->fill2);
    draw_rect_with_border(img, page_x + offset, page_y + offset, page_w - offset, page_h - offset, ic->stroke1, ic->fill2);
    
    int corner_size = size / 3;
    draw_page_folded_corner(img, page_x, page_y, page_w, page_h, corner_size, ic->stroke1, ic->fill1);
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


void draw_diskette(Image* img, int dsk_x, int dsk_y, int dsk_w, int dsk_h, IconColors* ic)
{
    draw_rect_with_border(img, dsk_x, dsk_y, dsk_w, dsk_h, ic->stroke1, ic->fill1);
    
    draw_diskette_corner(img, dsk_x, dsk_y, dsk_w, dsk_h, dsk_w / 5, ic->stroke1, ic->fill1);
    
    /* Diskette Label */
    if(dsk_w >= 14)
    {
        int offset = dsk_w / 5;
        int height = dsk_h / 2;
        draw_rect_with_border(img, dsk_x + offset, dsk_y + dsk_h - height, dsk_w - offset*2, height, ic->stroke1, ic->fill2);
    }
    
    /* Diskette Shutter */
    if(dsk_w >= 14)
    {
        int width  = (dsk_w / 2) - 1;
        int height = (dsk_h / 3) + 1;
        int offset = dsk_w / 4;
        fill(img, ic->stroke1, {dsk_x + offset, dsk_y, width, height});
        fill(img, ic->fill1, {dsk_x + offset + 1, dsk_y + 1, (width / 2) - 1, height - 2});
    }
}


void gen_icon_Diskette(Image* img, int size, IconColors* ic)
{
    if(size < 8)
        return;
    
    int dsk_x = 0;
    int dsk_y = 1;
    int dsk_w = size;
    int dsk_h = size - 2;
    
    draw_diskette(img, dsk_x, dsk_y, dsk_w, dsk_h, ic);
}


void gen_icon_DoubleDiskette(Image* img, int size, IconColors* ic)
{
    if(size < 8)
        return;
    
    int dsk_x = 0;
    int dsk_y = 1;
    int dsk_w = size;
    int dsk_h = size - 2;
    
    int offset = size / 8;
    
    draw_rect_with_border(img, dsk_x, dsk_y, dsk_w - offset, dsk_h - offset, ic->stroke1, ic->fill1);
    
    int corner_size = size / 5;
    draw_diskette_corner(img, dsk_x, dsk_y, dsk_w - offset, dsk_h - offset, corner_size, ic->stroke1, ic->fill1);
    
    draw_diskette(img, dsk_x + offset, dsk_y + offset, dsk_w - offset, dsk_h - offset, ic);
}


void gen_icon_Folder(Image* img, int size, IconColors* ic)
{
    if(size < 8)
        return;
    
    int folder_x = 0;
    int folder_y = size / 3;
    int folder_w = size;
    int folder_h = size / 2;
    
    draw_rect_with_border(img, folder_x, folder_y, folder_w, folder_h, ic->stroke1, ic->fill1);
    
    int tab_width = folder_w / 2;
    int tab_x = folder_x + folder_w / 5;
    
    for(int x=1; x<(tab_width - 1); x++)
    {
        int xx = x + tab_x;
        for(int c=0; c<4; c++)
        {
            img->pixel(xx, folder_y)[c] = ic->fill1[c];
        }
    }
    
    for(int c=0; c<4; c++)
    {
        img->pixel(tab_x, folder_y - 1)[c] = img->pixel(tab_x + tab_width - 1, folder_y - 1)[c] = ic->stroke1[c];
    }
    
    for(int x=0; x<tab_width; x++)
    {
        int xx = x + tab_x;
        for(int c=0; c<4; c++)
        {
            img->pixel(xx, folder_y - 2)[c] = ic->stroke1[c];
        }
    }
    
    if(tab_x >= 3)
    {
        for(int c=0; c<4; c++)
        {
            img->pixel(folder_x, folder_y - 1)[c] = img->pixel(folder_x + folder_w - 1, folder_y - 1)[c] = ic->stroke2[c];
        }
        
        for(int x=0; x<(tab_x - 1); x++)
        {
            int xx = x + folder_x;
            for(int c=0; c<4; c++)
            {
                img->pixel(xx, folder_y - 2)[c] = ic->stroke2[c];
            }
        }
        
        for(int x=(tab_x + tab_width + 1); x<folder_w; x++)
        {
            for(int c=0; c<4; c++)
            {
                img->pixel(x, folder_y - 2)[c] = ic->stroke2[c];
            }
        }
        
        for(int x=(tab_x - 2); x<=(tab_x + tab_width + 1); x++)
        {
            int xx = x + folder_x;
            for(int c=0; c<4; c++)
            {
                img->pixel(xx, folder_y - 4)[c] = ic->stroke2[c];
            }
        }
        
        for(int c=0; c<4; c++)
        {
            img->pixel(tab_x - 2, folder_y - 3)[c] = img->pixel(tab_x + tab_width + 1, folder_y - 3)[c] = ic->stroke2[c];
        }
        
        
        for(int x=1; x<(tab_x); x++)
        {
            int xx = x + folder_x;
            for(int c=0; c<4; c++)
            {
                img->pixel(xx, folder_y - 1)[c] = ic->fill2[c];
            }
        }
        
        for(int x=(tab_x + tab_width); x<(folder_w - 1); x++)
        {
            for(int c=0; c<4; c++)
            {
                img->pixel(x, folder_y - 1)[c] = ic->fill2[c];
            }
        }
        
        for(int x=(tab_x - 1); x<=(tab_x + tab_width); x++)
        {
            int xx = x + folder_x;
            for(int c=0; c<4; c++)
            {
                img->pixel(xx, folder_y - 3)[c] = ic->fill2[c];
            }
        }
        
        for(int c=0; c<4; c++)
        {
            img->pixel(tab_x - 1, folder_y - 2)[c] = img->pixel(tab_x + tab_width, folder_y - 2)[c] = ic->fill2[c];
        }
    }
}


IconEntry* gen_new_icon(IconName name, int size, IconColors* ic)
{
    if(size <= 0)
        return nullptr;
    
    auto entry = new IconEntry;
    entry->name = name;
    entry->size = size;
    entry->img.load(size, size, 4);
    fill(&(entry->img), ic->fill1);
    switch(name)
    {
        case IconName::Page:
        {
            gen_icon_Page(&(entry->img), size, ic);
            break;
        }
        
        case IconName::DoublePage:
        {
            gen_icon_DoublePage(&(entry->img), size, ic);
            break;
        }
        
        case IconName::Folder:
        {
            gen_icon_Folder(&(entry->img), size, ic);
            break;
        }
        
        case IconName::Diskette:
        {
            gen_icon_Diskette(&(entry->img), size, ic);
            break;
        }
        
        case IconName::DoubleDiskette:
        {
            gen_icon_DoubleDiskette(&(entry->img), size, ic);
            break;
        }
        
        default:
            break;
    }
    return entry;
}
    
}//namespace
    
Image* get_icon(IconName name, int size, IconColors* ic)
{
    auto entry = find_existing_icon(name, size, ic);
    if(!entry)
    {
        entry = gen_new_icon(name, size, ic);
        entry->user_count++;
    }
    return &(entry->img);
}

void free_icon(Image* icon_image)
{
    auto entry = find_existing_icon(icon_image);
    if(entry)
    {
        entry->user_count--;
        if(entry->user_count == 0)
        {
            entry->img.free();
            delete entry;
        }
    }
}
    
}//namespace r64fx
