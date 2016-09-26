#pragma  once

#include <tbb/spin_mutex.h>
#include "pxr/base/arch/defines.h"
#include "pxr/base/vt/api.h"

VT_API void* VtAlignedMalloc(size_t size, size_t align);
VT_API void  VtAlignedFree(void *addr);

VT_API void* VtCachedMalloc(size_t size);
VT_API void VtCachedFree(size_t size, void *addr);



#ifdef USD_ENABLE_CACHED_NEW
    #define VT_DEFINE_CACHED_OPERATOR_NEW(T)\
        static VtFixedAllocator& _GetAllocator() { static VtFixedAllocator _impl(sizeof(T), 256); return _impl; }\
        void* operator new (size_t) { return _GetAllocator().Allocate(); }\
        void operator delete (void* addr) { _GetAllocator().Free(addr); }
#else
    #define VT_DEFINE_CACHED_OPERATOR_NEW(T)
#endif


class VtFixedAllocator
{
public:
    VT_API VtFixedAllocator(size_t bin_size, size_t block_size);
    VT_API void* Allocate();
    VT_API void Free(void *addr);

private:
    size_t _bin_size = 0;
    size_t _block_size = 0;
    size_t _capacity = 0;
    size_t _in_use = 0;
    void *_head = nullptr;
    tbb::spin_mutex _mutex;
};


template<class T, int Alignment = 0x10>
class VtAlignedAllocator
{
public:
    typedef T                                    value_type;
    typedef T *                                  pointer;
    typedef const T *                            const_pointer;
    typedef T &                                  reference;
    typedef const T &                            const_reference;
    typedef std::size_t                          size_type;
    typedef std::ptrdiff_t                       difference_type;

    static const int alignment = Alignment;

    //!Obtains an VtAlignedAllocator that allocates
    //!objects of type T2
    template<class T2>
    struct rebind
    {
        typedef VtAlignedAllocator<T2> other;
    };

    //!Default constructor
    //!Never throws
    VtAlignedAllocator()
    {}

    //!Constructor from other VtAlignedAllocator.
    //!Never throws
    VtAlignedAllocator(const VtAlignedAllocator &)
    {}

    //!Constructor from related VtAlignedAllocator.
    //!Never throws
    template<class T2>
    VtAlignedAllocator(const VtAlignedAllocator<T2> &)
    {}

    //!Allocates memory for an array of count elements.
    //!Throws std::bad_alloc if there is no enough memory
    pointer allocate(size_type size)
    {
        return static_cast<T*>(VtAlignedMalloc(size * sizeof(T), alignment));
    }

    //!Deallocates previously allocated memory.
    //!Never throws
    void deallocate(pointer ptr, size_type size)
    {
        VtAlignedFree(ptr);
    }

    //!Returns the maximum number of elements that could be allocated.
    //!Never throws
    size_type max_size() const
    {
        return size_type(-1) / sizeof(T);
    }

    //!Swaps two allocators, does nothing
    //!because this VtAlignedAllocator is stateless
    friend void swap(VtAlignedAllocator &, VtAlignedAllocator &)
    {}

    //!An VtAlignedAllocator always compares to true, as memory allocated with one
    //!instance can be deallocated by another instance
    friend bool operator==(const VtAlignedAllocator &, const VtAlignedAllocator &)
    {
        return true;
    }

    //!An VtAlignedAllocator always compares to false, as memory allocated with one
    //!instance can be deallocated by another instance
    friend bool operator!=(const VtAlignedAllocator &, const VtAlignedAllocator &)
    {
        return false;
    }
};
