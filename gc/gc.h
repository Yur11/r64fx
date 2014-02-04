#ifndef R64FX_GC_H
#define R64FX_GC_H

namespace r64fx{
    
/** @brief Base class for items that may want to marked for centralized deletion. 
 
    Sometimes it's usefull to manage memory deallocation in a centralized manner.
    Classes that wish to have their instance destruction managed in such a way
    should inherit from the Disposable class an call deleteLater() when needed.
    It's not safe to access an object after its deleteLater() metod has been called.
    
    Disposable instances can be deleted manually, but in such case, deleteLater() must
    not be called on them!
 */
class Disposable{
public:
    virtual ~Disposable() {}
    
    /** @brief Tell the garbage collector that this instance is no longer needed and should be destroyed. */
    virtual void deleteLater();
  
protected:
    /** @brief Called just before destruction. */
    virtual void beforeDestruction() {}
};

class gc{
public:
    /** @brief Delete all objects that were marked for deletion. 
     
        @brief Function returns the number of objects that were deleted.
     */
    static int deleteGarbage();
};
    
}//namespace r64fx

#endif//R64FX_GC_H