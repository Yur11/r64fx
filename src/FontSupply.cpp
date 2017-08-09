#include "FontSupply.hpp"
#include "LinkedList.hpp"

namespace r64fx{

namespace{

struct FontRecord : public LinkedList<FontRecord>::Node{
    Font*        font       = nullptr;
    std::string  name       = "";
    float        size       = 0.0f;
    float        dpi        = 0;
    long         use_count  = 0;
};

LinkedList<FontRecord> g_records;

}//namespace


Font* get_font(std::string name, float size, int dpi)
{
    for(auto record : g_records)
    {
        if(record->size == size && record->dpi == record->dpi && record->name == name)
        {
            record->use_count++;
            return record->font;
        }
    }

    auto font = new Font(name, size, dpi);
    auto record = new FontRecord;
    record->font = font;
    record->name = name;
    record->size = size;
    record->dpi  = dpi;
    record->use_count  = 1;
    g_records.append(record);
    return font;
}


void free_font(Font* font)
{
    FontRecord* record_to_remove = nullptr;
    for(auto record : g_records)
    {
        if(record->font == font)
        {
            record->use_count--;
            if(record->use_count == 0)
                record_to_remove = record;
        }
    }

    if(record_to_remove)
    {
        delete record_to_remove->font;
        g_records.remove(record_to_remove);
        delete record_to_remove;
    }
}

}//namespace r64fx
