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
#ifdef DEBUG_LINKED_ITEM_CHAIN
        assert(item != nullptr);
        assert(item->prev_item == nullptr);
#endif//DEBUG_LINKED_ITEM_CHAIN
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
    
    /** @brief Set pointers to nullptr, without affecting the siblings. 
     
        Usefull for clearing the list.
     */
    void reset()
    {
        prev_item = next_item = nullptr;
    }
    
    /** @brief Replace this item with another one.*/
    void replace(T* item)
    {
#ifdef DEBUG_LINKED_ITEM_CHAIN
        assert(item != nullptr);
        assert(item->prev == nullptr);
        assert(item->next == nullptr);
#endif//DEBUG_LINKED_ITEM_CHAIN
        if(prev_item)
            prev_item->next_item = item;
        if(next_item)
            next_item->prev_item = item;
        
        next_item = prev_item = nullptr;
    }
    
    T* next() const { return (T*)next_item; }
    T* prev() const { return (T*)prev_item; }
};


/** @brief A wrapper around LinkedItem to be used as an iterator. */
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


struct Item : public LinkedItem<Item>{
    int num = false;
    
    Item(int num)
    : num(num)
    {}
};


/** @brief Insert a new item at the end of the list. 
    
    Parameters first_ptr and last_ptr are pointers to the first and the last item of the list respectively.
    They are updated as needed.
    Their initial value can be nullptr. In that case the list is assumed to be empty.
    They both must be either nullptr or non nullptr.
        
    The function returns the item that has been inserted.
    It must be a valid object.
    The item's prev() pointer must be nullptr.
    If item is the first item of another list, the lists are linked.
 */
template<typename T> Item* append_to_list(T* &first_ptr, T* &last_ptr, T* item)
{
#ifdef DEBUG_LINKED_ITEM_CHAIN
    assert(item != nullptr);
    assert(item->prev() == nullptr);
    assert(item != first_ptr);
    assert(item != last_ptr);
#endif//DEBUG_LINKED_ITEM_CHAIN
    
    if(last_ptr == nullptr)
    {
#ifdef DEBUG_LINKED_ITEM_CHAIN
        assert(first_ptr == nullptr);
#endif//DEBUG_LINKED_ITEM_CHAIN
        first_ptr = last_ptr = item;
    }
    else
    {
#ifdef DEBUG_LINKED_ITEM_CHAIN
        assert(first_ptr != nullptr);
#endif//DEBUG_LINKED_ITEM_CHAIN
        last_ptr->insert(item);
        last_ptr = item;
    }
    
    return item;
}


/** @brief Insert a new item after an existing one. 
 
    Parameters first_ptr and last_ptr are pointers to the first and the last item of the list respectively.
    They are updated as needed.
    Their initial value can be nullptr. In that case the list is assumed to be empty.
    They both must be either nullptr or non nullptr.
    
    The existing_item must belong to the list bound by first_ptr and last_ptr.
    No check is done for this.
    It can be nullptr only if the list is empty, i.e both first_ptr and last_item are null.
    In that case it must be null.
    
    The function returns the item that has been inserted.
    It must be a valid object.
    Also it must not belong to a list.
 */
template<typename T> T* insert_to_list(T* &first_ptr, T* &last_ptr, T* existing_item, T* new_item)
{
#ifdef DEBUG_LINKED_ITEM_CHAIN
    assert(new_item != nullptr);
    assert(new_item->next() == nullptr);
    assert(new_item->prev() == nullptr);
    assert(new_item != first_ptr);
    assert(new_item != last_ptr);
#endif//DEBUG_LINKED_ITEM_CHAIN
    
    if(last_ptr == nullptr)
    {
#ifdef DEBUG_LINKED_ITEM_CHAIN
        assert(first_ptr == nullptr);
        assert(existing_item == nullptr);
#endif//DEBUG_LINKED_ITEM_CHAIN
        first_ptr = last_ptr = new_item;
    }
    else
    {
#ifdef DEBUG_LINKED_ITEM_CHAIN
        assert(first_ptr != nullptr);
        assert(existing_item != nullptr);
#endif//DEBUG_LINKED_ITEM_CHAIN
        existing_item->insert(new_item);
    }
    
    return new_item;
}


/** @brief Remove one item from the list. 
 
    Parameters first_ptr and last_ptr are pointers to the first and the last item of the list respectively.
    They are updated as needed.
 
    The item must be a valid object.
    It must belong to the list.
    
    The list must not be empty.
 */
template<typename T> T* remove_from_list(T* &first_ptr, T* &last_ptr, T* item)
{
#ifdef DEBUG_LINKED_ITEM_CHAIN
    assert(item != nullptr);
    assert(first_ptr != nullptr);
    assert(last_ptr != nullptr);
#endif//DEBUG_LINKED_ITEM_CHAIN
    
    if(first_ptr == item)
    {
        first_ptr = item->next();
    }
    
    if(last_ptr == item)
    {
        last_ptr = item->prev();
    }
    
    item->remove();
    
    return item;
}


/** @brief Remove all items from the list. 
 
    Parameters first_ptr and last_ptr are set to null.
 */
template<typename T> void clear_list(T* &first_ptr, T* &last_ptr)
{
    while(last_ptr)
        remove_from_list(first_ptr, last_ptr, last_ptr);
}


template<typename T> bool list_is_empty(T* first_ptr, T* last_ptr)
{
    return first_ptr == nullptr && last_ptr == nullptr;
}


/** @brief Container for linked items. */
template<typename T> class LinkedItemChain{
    T* first_item = nullptr; 
    T* last_item = nullptr;
    
public:
    bool isEmpty() const { return list_is_empty(first_item, last_item); }
    
    T* first() const { return first_item; }
    
    T* last() const { return last_item; }
    
    LinkedItemPointer<T> begin() const { return first_item; }
    
    LinkedItemPointer<T> end() const { return last_item ? last_item->next() : nullptr; }
    
    T* append(T* item) { return append_to_list(first_item, last_item, item); }
    
    T* append(LinkedItemChain* other) 
    {
        assert(this != other);
        append(other->first());
    }
    
    void clear() { return clear_list(first_item, last_item); }
};

    
}//namespace r64fx

#endif//R64FX_LINKED_ITEM_H