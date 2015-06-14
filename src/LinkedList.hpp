#ifndef R64FX_LINKED_LIST_H
#define R64FX_LINKED_LIST_H

#ifdef R64FX_DEBUG
#include <assert.h>
#endif//DEBUG

namespace r64fx{
    
/** @brief Base class mixin for items that can form a linked list. 
 
    Simply inherit from LinkedItems and pass the name of your child class as T.
    I.e. class MyClass : public LinkedItem< MyClass > {};
        
    One disadvantage is that instances of LinkedItem may belong only to a single list at a time.
    Thus one must make sure that a LinkedItem instance does not allready belong to a list before inserting.
    Define the R64FX_DEBUG macro to enable checks.
    
    On a plus side, no extra memory alocation is needed to form lists out of LinkedItem instances.
    
    Some helper classes are also available.
    
        - LinkedItemPointer - Works as an iterator.
        
        - LinkedList - Pointer to a segment of a list. One item, a range of several items or the whole chain.
                            Can give you the first and the last item.
*/
template<typename T> class LinkedItem{
    LinkedItem<T>* prev_item = nullptr;
    LinkedItem<T>* next_item = nullptr;
        
public:
    /** @brief Insert another LinkedItem after this one. */
    void insert(T* item)
    {
#ifdef R64FX_DEBUG
        assert(item != nullptr);
        assert(item->prev_item == nullptr);
#endif//R64FX_DEBUG
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
    
    
    /** @brief Replace this item with another one.
     
        This item is removed from the list.
     */
    void replace(T* item)
    {
#ifdef R64FX_DEBUG
        assert(item != nullptr);
        assert(item->prev == nullptr);
        assert(item->next == nullptr);
#endif//R64FX_DEBUG
        if(prev_item)
            prev_item->next_item = item;
        if(next_item)
            next_item->prev_item = item;
        
        next_item = prev_item = nullptr;
    }
    
    T* next() const { return (T*)next_item; }
    T* prev() const { return (T*)prev_item; }
};


/** @brief A wrapper around LinkedItem to be used as an iterator. 
 
    Will cast to true if can be dereferenced, otherwise false.
 */
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
    
    inline LinkedItemPointer<T> operator--()
    {
        ptr = ptr->prev();
        return ptr;
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
template<typename T> T* append_to_list(T* &first_ptr, T* &last_ptr, T* item)
{
#ifdef R64FX_DEBUG
    assert(item != nullptr);
    assert(item->prev() == nullptr);
    assert(item != first_ptr);
    assert(item != last_ptr);
#endif//R64FX_DEBUG
    
    if(last_ptr == nullptr)
    {
#ifdef R64FX_DEBUG
        assert(first_ptr == nullptr);
#endif//R64FX_DEBUG
        first_ptr = last_ptr = item;
    }
    else
    {
#ifdef R64FX_DEBUG
        assert(first_ptr != nullptr);
#endif//R64FX_DEBUG
        last_ptr->insert(item);
        last_ptr = item;
    }
    
    while(last_ptr->next())
            last_ptr = last_ptr->next();
    
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
template<typename T> T* insert_into_list(T* &first_ptr, T* &last_ptr, T* existing_item, T* new_item)
{
#ifdef R64FX_DEBUG
    assert(new_item != nullptr);
    assert(new_item->next() == nullptr);
    assert(new_item->prev() == nullptr);
    assert(new_item != first_ptr);
    assert(new_item != last_ptr);
#endif//R64FX_DEBUG
    
    if(last_ptr == nullptr)
    {
#ifdef R64FX_DEBUG
        assert(first_ptr == nullptr);
        assert(existing_item == nullptr);
#endif//R64FX_DEBUG
        first_ptr = last_ptr = new_item;
    }
    else
    {
#ifdef R64FX_DEBUG
        assert(first_ptr != nullptr);
        assert(existing_item != nullptr);
#endif//R64FX_DEBUG
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
#ifdef R64FX_DEBUG
    assert(item != nullptr);
    assert(first_ptr != nullptr);
    assert(last_ptr != nullptr);
#endif//R64FX_DEBUG
    
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


/** @brief Container for linked items. 
 
    Pointer to the first and the last item in the chain are contained inside.
    May point to only a part of a larger chain.
 
    This should work with c++11 range based for loop.
    Haven't tried this with std algorithms yet.
 */
template<typename T> class LinkedList{
    T* first_item = nullptr; 
    T* last_item = nullptr;
    
public:
    bool isEmpty() const { return list_is_empty(first_item, last_item); }
    
    T* first() const { return first_item; }
    
    T* last() const { return last_item; }
    
    /** @brief Iterator to the first item in the chain. 
     
        Just like with stl containers.
     */
    LinkedItemPointer<T> begin() const { return first_item; }
    
    /** @brief Iterator to past the last item in the chain. 
     
        Just like with stl containers.
     */
    LinkedItemPointer<T> end() const { return last_item ? last_item->next() : nullptr; }
    
    T* append(T* item) { return append_to_list(first_item, last_item, item); }
    
    void append(LinkedList<T> other) 
    {
#ifdef R64FX_DEBUG
        assert(&other != this);
#endif//R64FX_DEBUG
        append_to_list(first_item, last_item, other.first());
        
    }

    /** @brief Insert a new_item after an existing_item. 
     
        existing_item must belong to the chain.
     */
    T* insert(T* existing_item, T* new_item) { return insert_into_list(first_item, last_item, existing_item, new_item); }
    
    /** @brief Remove an item from the chain. 
     
        item must belong to the chain.
     */
    T* remove(T* item) { return remove_from_list(first_item, last_item, item); };
    
    /** @brief Remove all items from the list. Break all links. */
    void clear() { return clear_list(first_item, last_item); }
};
    
}//namespace r64fx

#endif//R64FX_LINKED_LIST_H