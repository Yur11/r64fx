#ifndef R64FX_LINKED_ITEM_H
#define R64FX_LINKED_ITEM_H

#ifdef DEBUG
#include <assert.h>
#endif//DEBUG

namespace r64fx{
    
/** @brief Base class mixin for items that can form a linked list. 
 
    Simply inherit from LinkedItems and pass the name of your child class as T.
    I.e. class MyClass : public LinkedItem< MyClass > {};
 */
template<typename T> struct LinkedItem{
    LinkedItem<T>* prev = nullptr;
    LinkedItem<T>* next = nullptr;
        
    /** @brief Insert another LinkedItem after this one. */
    void insert(T* item)
    {
#ifdef DEBUG
        assert(item->prev == nullptr);
#endif//DEBUG
        item->prev = this;
        if(next)
        {
            next->prev = item;
            item->next = next;
        }
        next = item;
    }
    
    /** @brief Remove this item and link it's neighbours. */
    void remove()
    {
        if(prev)
            prev->next = next;
        if(next)
            next->prev = prev;
        next = prev = nullptr;
    }
    
    /** @brief Replace this item with another one.*/
    void replace(T* item)
    {
#ifdef DEBUG
        assert(item->prev == nullptr);
        assert(item->next == nullptr);
#endif//DEBUG
        if(prev)
            prev->next = item;
        if(next)
            next->prev = item;
        
        next = prev = nullptr;
    }
    
    T* nextItem() { return (T*)next; }
    T* prevItem() { return (T*)prev; }
};
    
}//namespace r64fx

#endif//R64FX_LINKED_ITEM_H