#include "pxr/base/arch/defines.h"
#include "pxr/base/vt/api.h"
#include "pxr/base/vt/allocator.h"

#include <cstddef>
#ifdef ARCH_OS_WINDOWS
    #include <windows.h>
#else
    #include <stdlib.h>
#endif

VT_API void* VtAlignedMalloc(size_t size, size_t alignment)
{
    size_t mask = alignment - 1;
    size = (size + mask) & (~mask);
#ifdef ARCH_OS_WINDOWS
    return _aligned_malloc(size, alignment);
#else
    void *ret;
    return posix_memalign(&ret, alignment, size) == 0 ? ret : nullptr;
#endif
}

VT_API void  VtAlignedFree(void *addr)
{
#ifdef ARCH_OS_WINDOWS
    _aligned_free(addr);
#else
    free(addr);
#endif
}

struct Bin
{
    Bin *next;
};

VT_API VtFixedAllocator::VtFixedAllocator(size_t bin_size, size_t block_size)
    : _bin_size(bin_size), _block_size(block_size)
{
}

VT_API void* VtFixedAllocator::Allocate()
{
    tbb::spin_mutex::scoped_lock lock(_mutex);
    if (!_head) {
        _head = VtAlignedMalloc(_bin_size * _block_size, 0x20);
        auto *c = (char*)_head;
        for (size_t i = 0; i < _block_size - 1; ++i) {
            ((Bin*)(c + _bin_size*i))->next = (Bin*)(c + _bin_size*(i + 1));
        }
        ((Bin*)(c + (_bin_size*(_block_size - 1))))->next = nullptr;
        _capacity += _block_size;
    }

    ++_in_use;
    void *ret = _head;
    _head = ((Bin*)_head)->next;
    return ret;
}

VT_API void VtFixedAllocator::Free(void *addr)
{
    if (!addr) { return; }

    tbb::spin_mutex::scoped_lock lock(_mutex);
    ((Bin*)addr)->next = (Bin*)_head;
    _head = addr;
    --_in_use;
}

static VtFixedAllocator g_allocators[] = {
    { 0x8, 1 },
    { 0x8, 1 },
    { 0x8, 1 },
    { 0x8, 1 },
    { 0x10, 512 }, // 4
    { 0x20, 512 },
    { 0x40, 512 },
    { 0x80, 512 },
    { 0x100, 128 }, // 8
    { 0x200, 128 },
    { 0x400, 128 },
    { 0x800, 128 },
    { 0x1000, 32 },
    { 0x2000, 32 },
    { 0x4000, 32 },
    { 0x8000, 32 },
    { 0x10000, 8 }, // 16
    { 0x20000, 8 },
    { 0x40000, 8 },
    { 0x80000, 8 },
    { 0x100000, 1 },
    { 0x200000, 1 },
    { 0x400000, 1 },
    { 0x800000, 1 },
    { 0x1000000, 1 }, // 24
    { 0x2000000, 1 },
    { 0x4000000, 1 },
    { 0x8000000, 1 },
    { 0x10000000, 1 },
    { 0x20000000, 1 },
    { 0x40000000, 1 },
    { 0x80000000, 1 },
    { 0x100000000, 1 }, // 32
    { 0x200000000, 1 },
    { 0x400000000, 1 },
    { 0x800000000, 1 },
    { 0x1000000000, 1 }, // 36
};

static inline size_t _SizeToIndex(size_t size)
{
    unsigned long index;
    _BitScanReverse64(&index, size);
    return index + (__popcnt64(size) > 1 ? 1 : 0);
}


VT_API void* VtCachedMalloc(size_t size)
{
    return g_allocators[_SizeToIndex(size)].Allocate();
}

VT_API void VtCachedFree(size_t size, void *addr)
{
    return g_allocators[_SizeToIndex(size)].Free(addr);
}

