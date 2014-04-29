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
template<typename T> class LinkedItem{
    LinkedItem<T>* prev_item = nullptr;
    LinkedItem<T>* next_item = nullptr;
        
public:
    /** @brief Insert another LinkedItem after this one. */
    void insert(T* item)
    {
#ifdef DEBUG
        assert(item->prev_item == nullptr);
#endif//DEBUG
        item->prev_item = this;
        if(next_item)
        {
            next_item->prev_item = item;
            item->next_item = this->next_item;
        }
        this->next_item = item;
    }
    
    /** @brief Remove this item and link it's neighbours. */
    void remove()
    {
        if(prev_item)
            prev_item->next_item = this->next_item;
        if(next_item)
            next_item->prev_item = this->prev_item;
        next_item = prev_item = nullptr;
    }
    
    /** @brief Replace this item with another one.*/
    void replace(T* item)
    {
#ifdef DEBUG
        assert(item->prev == nullptr);
        assert(item->next == nullptr);
#endif//DEBUG
        if(prev_item)
            prev_item->next_item = item;
        if(next_item)
            next_item->prev_item = item;
        
        next_item = prev_item = nullptr;
    }
    
    T* next() const { return (T*)next_item; }
    T* prev() const { return (T*)prev_item; }
};


/** */
template<typename T>
class LinkedItemPointer{
    T* ptr;

public:
    LinkedItemPointer(T* ptr = nullptr)
    : ptr(ptr)
    {
        
    }
    
    inline operator bool() { return ptr == nullptr; }
    
    inline LinkedItemPointer<T> operator++()
    {
        ptr = ptr->next();
        return ptr;
    }
    
    inline LinkedItemPointer<T> operator++(int)
    {
        auto old = ptr;
        ptr = ptr->next();
        return old;
    }
    
    inline LinkedItemPointer<T> operator--()
    {
        ptr = ptr->prev();
        return ptr;
    }
    
    inline LinkedItemPointer<T> operator--(int)
    {
        auto old = ptr;
        ptr = ptr->prev();
        return old;
    }
    
    inline T &operator*()
    {
        return *ptr;
    }
    
    inline T* operator->()
    {
        return ptr;
    }
    
    bool operator==(LinkedItemPointer<T> other)
    {
        return other.ptr == ptr;
    }
    
    bool operator!=(LinkedItemPointer<T> other)
    {
        return other.ptr != ptr;
    }
};



    
}//namespace r64fx

#endif//R64FX_LINKED_ITEM_H