#include "RenderingContext.h"

#ifdef DEBUG
#include <assert.h>
#include <iostream>
using namespace std;
#endif//DEBUG

namespace r64fx{

RenderingContext* RenderingContext::all_rendering_contexts[max_rendering_context_count];

RenderingContext* RenderingContext::current_context = nullptr;


void RenderingContext::registerItem(RenderingContextAware* item)
{
    for(RenderingContextId_t id=0; id<max_rendering_context_count; id++)
    {
        auto ctx = all_rendering_contexts[id];
        if(ctx)
            ctx->new_items.push_back(item);
    }
}

    
void RenderingContext::discardItem(RenderingContextAware* item)
{
    for(RenderingContextId_t id=0; id<max_rendering_context_count; id++)
    {
        if(all_rendering_contexts[id])
        {
            all_rendering_contexts[id]->discarded_items.push_back(item);
            
            for(auto it=all_rendering_contexts[id]->active_items.begin(); it!=all_rendering_contexts[id]->active_items.end(); it++)
                if(*it == item)
                    all_rendering_contexts[id]->active_items.erase(it);
        }            
    }
}


RenderingContextId_t RenderingContext::getFreeId()
{
    for(RenderingContextId_t i=0; i<max_rendering_context_count; i++)
        if(all_rendering_contexts[i] == nullptr)
            return i;
    return BadRenderingContextId;
}


RenderingContext::RenderingContext(RenderingContextId_t id)
: _id(id)
{
    all_rendering_contexts[id] = this;
    
    /* Copy items from the current context to this one. */
    if(current_context)
    {
        for(auto item : current_context->new_items)
            new_items.push_back(item);
        
        for(auto item : current_context->active_items)
            new_items.push_back(item);
    }
}


RenderingContext::~RenderingContext()
{
    all_rendering_contexts[_id] = nullptr;
}


void RenderingContext::makeCurrent()
{
    current_context = this;
}


void RenderingContext::init()
{
    for(RenderingContextId_t i=0; i<max_rendering_context_count; i++)
        all_rendering_contexts[i] = nullptr;
}


void RenderingContext::update()
{
#ifdef DEBUG
    assert(current_context == this);
#endif//DEBUG
 
    if(!new_items.empty())
    {
        cout << "RenderingContext " << id() << " update " << new_items.size() << " items\n";
        
        for(auto item : new_items)
        {
            item->setupForContext(id());
            active_items.push_back(item);
        }
        new_items.clear();
    }
    
    if(!discarded_items.empty())
    {
        cout << "RenderingContext " << id() << " discard " << new_items.size() << " items\n";
        
        for(auto item : discarded_items)
        {
            item->cleanupForContext(id());
            item->deleteLater();
        }
        discarded_items.clear();
    }
}


void RenderingContext::cleanup()
{
    while(!discarded_items.empty())
    {
        discarded_items.back()->cleanupForContext(id());
        discarded_items.pop_back();
    }
    
    while(!active_items.empty())
    {
        active_items.back()->cleanupForContext(id());
        active_items.pop_back();
    }
}


void RenderingContext::beforeDestruction()
{
    makeCurrent();
    cleanup();
}

   
RenderingContextAware::RenderingContextAware()
{
    RenderingContext::registerItem(this);
}


void RenderingContextAware::discard()
{
    RenderingContext::discardItem(this);
}

    
}//namespace r64fx