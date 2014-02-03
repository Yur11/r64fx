#include "RenderingContext.h"
#include <iostream>

using namespace std;

namespace r64fx{

RenderingContext* RenderingContext::all_rendering_contexts[max_rendering_context_count];


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
}


RenderingContext::~RenderingContext()
{
    all_rendering_contexts[_id] = nullptr;
}


void RenderingContext::init()
{
    for(RenderingContextId_t i=0; i<max_rendering_context_count; i++)
        all_rendering_contexts[i] = nullptr;
}


void RenderingContext::setup()
{
    makeCurrent();
    for(auto item = RenderingContextAware::first_item; item != nullptr; item = item->nextItem())
    {
        if(item->is_setup_for[RenderingContext::id()])
            continue;
        item->setupForContext(this->id());
        item->is_setup_for[RenderingContext::id()] = true;
    }
}


void RenderingContext::cleanup()
{
    makeCurrent();
    for(auto item = RenderingContextAware::first_item; item != nullptr; item = item->nextItem())
    {
        if(!item->is_setup_for[RenderingContext::id()])
            continue;
        item->cleanupForContext(this->id());
        item->is_setup_for[RenderingContext::id()] = false;
    }
}

    
RenderingContextAware* RenderingContextAware::first_item = nullptr;


RenderingContextAware::RenderingContextAware()
{
    for(RenderingContextId_t i=0; i<max_rendering_context_count; i++)
    {
        is_setup_for[i] = false;
    }
    
    this->next = first_item;
    first_item = this;   
}


RenderingContextAware::~RenderingContextAware()
{
    if(this == first_item)
        first_item = nullptr;
    remove();
}


    
}//namespace r64fx