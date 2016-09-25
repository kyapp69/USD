#pragma once


template<bool Value>
struct VtAlignedAllocatorBool
{
    static const bool value = Value;
};

VT_API void* VtAlignedMalloc(size_t size, size_t align);
VT_API void  VtAlignedFree(void *addr);

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

    //!A integral constant of type bool with value true
    typedef BOOST_CONTAINER_IMPDEF(VtAlignedAllocatorBool<true>) propagate_on_container_move_assignment;
    //!A integral constant of type bool with value true
    typedef BOOST_CONTAINER_IMPDEF(VtAlignedAllocatorBool<true>) is_always_equal;

    //!Obtains an VtAlignedAllocator that allocates
    //!objects of type T2
    template<class T2>
    struct rebind
    {
        typedef VtAlignedAllocator<T2> other;
    };

    //!Default constructor
    //!Never throws
    VtAlignedAllocator() BOOST_NOEXCEPT_OR_NOTHROW
    {}

    //!Constructor from other VtAlignedAllocator.
    //!Never throws
    VtAlignedAllocator(const VtAlignedAllocator &) BOOST_NOEXCEPT_OR_NOTHROW
    {}

    //!Constructor from related VtAlignedAllocator.
    //!Never throws
    template<class T2>
    VtAlignedAllocator(const VtAlignedAllocator<T2> &) BOOST_NOEXCEPT_OR_NOTHROW
    {}

    //!Allocates memory for an array of count elements.
    //!Throws std::bad_alloc if there is no enough memory
    pointer allocate(size_type count) BOOST_NOEXCEPT_OR_NOTHROW
    {
        return static_cast<T*>(VtAlignedMalloc(count * sizeof(T), alignment));
    }

    //!Deallocates previously allocated memory.
    //!Never throws
    void deallocate(pointer ptr, size_type) BOOST_NOEXCEPT_OR_NOTHROW
    {
        VtAlignedFree(ptr);
    }

    //!Returns the maximum number of elements that could be allocated.
    //!Never throws
    size_type max_size() const BOOST_NOEXCEPT_OR_NOTHROW
    {
        return size_type(-1) / sizeof(T);
    }

    //!Swaps two allocators, does nothing
    //!because this VtAlignedAllocator is stateless
    friend void swap(VtAlignedAllocator &, VtAlignedAllocator &) BOOST_NOEXCEPT_OR_NOTHROW
    {}

    //!An VtAlignedAllocator always compares to true, as memory allocated with one
    //!instance can be deallocated by another instance
    friend bool operator==(const VtAlignedAllocator &, const VtAlignedAllocator &) BOOST_NOEXCEPT_OR_NOTHROW
    {
        return true;
    }

    //!An VtAlignedAllocator always compares to false, as memory allocated with one
    //!instance can be deallocated by another instance
    friend bool operator!=(const VtAlignedAllocator &, const VtAlignedAllocator &) BOOST_NOEXCEPT_OR_NOTHROW
    {
        return false;
    }
};