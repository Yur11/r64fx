#ifndef R64FX_LINKED_LIST_H
#define R64FX_LINKED_LIST_H

#ifdef R64FX_DEBUG
#include <assert.h>
#endif//DEBUG

namespace r64fx{

template<typename T> class LinkedList{
    T* first_node = nullptr;
    T* last_node  = nullptr;

public:
    class Node{
        friend class LinkedList<T>;

        LinkedList<T>::Node* prev_node = nullptr;
        LinkedList<T>::Node* next_node = nullptr;

        void linkPrevious(T* node)
        {
            node->next_node = this;
            if(prev_node)
            {
                prev_node->next_node = node;
                node->prev_node = prev_node;
            }
            prev_node = node;
        }


        void linkNext(T* node)
        {
            node->prev_node = this;
            if(next_node)
            {
                next_node->prev_node = node;
                node->next_node = next_node;
            }
            next_node = node;
        }


        void unlink()
        {
            if(prev_node)
                prev_node->next_node = this->next_node;
            if(next_node)
                next_node->prev_node = this->prev_node;
            next_node = prev_node = nullptr;
        }


        void replace(T* node)
        {
            if(prev_node)
                prev_node->next_node = node;
            if(next_node)
                next_node->prev_node = node;

            next_node = prev_node = nullptr;
        }

    public:
        inline T* next() const { return static_cast<T*>(next_node); }
        inline T* prev() const { return static_cast<T*>(prev_node); }

        inline bool isLinked() const { return next() || prev(); }
    };//Node


    class Iterator{
        T* ptr;

    public:
        Iterator(T* ptr = nullptr)
        : ptr(ptr)
        {

        }

        inline operator bool() { return ptr == nullptr; }

        inline LinkedList<T>::Iterator operator++()
        {
            ptr = ptr->next();
            return ptr;
        }

        inline LinkedList<T>::Iterator operator--()
        {
            ptr = ptr->prev();
            return ptr;
        }

        inline LinkedList<T>::Iterator operator++(int)
        {
            ptr = ptr->next();
            return ptr;
        }

        inline LinkedList<T>::Iterator operator--(int)
        {
            ptr = ptr->prev();
            return ptr;
        }

        inline T* operator*()
        {
            return ptr;
        }

        inline T* operator->()
        {
            return ptr;
        }

        bool operator==(LinkedList<T>::Iterator other)
        {
            return other.ptr == ptr;
        }

        bool operator!=(LinkedList<T>::Iterator other)
        {
            return other.ptr != ptr;
        }

    };//Iterator


    bool empty() const
    {
        return first_node == nullptr && last_node == nullptr;
    }

    T* first() const { return first_node; }

    T* last() const { return last_node; }

    LinkedList<T>::Iterator begin() const { return first_node; }

    LinkedList<T>::Iterator end() const { return last_node ? last_node->next() : nullptr; }


    T* preppend(T* node)
    {
#ifdef R64FX_DEBUG
        assert(node != nullptr);
        assert(node->next() == nullptr);
        assert(node->prev() == nullptr);
        assert(node != first_node);
        assert(node != last_node);
#endif//R64FX_DEBUG

        if(first_node == nullptr)
        {
#ifdef R64FX_DEBUG
            assert(last_node == nullptr);
#endif//R64FX_DEBUG
            first_node = last_node = node;
        }
        else
        {
#ifdef R64FX_DEBUG
            assert(first_node != nullptr);
#endif//R64FX_DEBUG
            first_node->linkPrevious(node);
            first_node = node;
        }

        return node;
    }


    T* append(T* node)
    {
#ifdef R64FX_DEBUG
        assert(node != nullptr);
        assert(node->next() == nullptr);
        assert(node->prev() == nullptr);
        assert(node != first_node);
        assert(node != last_node);
#endif//R64FX_DEBUG

        if(last_node == nullptr)
        {
#ifdef R64FX_DEBUG
            assert(first_node == nullptr);
#endif//R64FX_DEBUG
            first_node = last_node = node;
        }
        else
        {
#ifdef R64FX_DEBUG
            assert(first_node != nullptr);
#endif//R64FX_DEBUG
            last_node->linkNext(node);
            last_node = node;
        }

        return node;
    }


    T* insertAfter(T* existing_node, T* node)
    {
#ifdef R64FX_DEBUG
        assert(node != nullptr);
        assert(node->next() == nullptr);
        assert(node->prev() == nullptr);
        assert(first_node != nullptr);
        assert(existing_node != nullptr);
        assert(existing_node != node);
#endif//R64FX_DEBUG

        existing_node->linkNext(node);
        if(existing_node == last_node)
        {
            last_node = node;
        }

        return node;
    }


    T* insertBefore(T* existing_node, T* node)
    {
#ifdef R64FX_DEBUG
        assert(node != nullptr);
        assert(node->next() == nullptr);
        assert(node->prev() == nullptr);
        assert(first_node != nullptr);
        assert(existing_node != nullptr);
        assert(existing_node != node);
#endif//R64FX_DEBUG

        existing_node->linkPrevious(node);
        if(existing_node == first_node)
        {
            first_node = node;
        }

        return node;
    }


    T* remove(T* node)
    {
#ifdef R64FX_DEBUG
        assert(node != nullptr);
        assert(first_node != nullptr);
        assert(last_node != nullptr);
#endif//R64FX_DEBUG

        if(first_node == node)
        {
            first_node = node->next();
        }

        if(last_node == node)
        {
            last_node = node->prev();
        }

        node->unlink();

        return node;
    }


    void clear()
    {
        while(last_node)
            remove(last_node);
    }

};

}//namespace r64fx

#endif//R64FX_LINKED_LIST_H
