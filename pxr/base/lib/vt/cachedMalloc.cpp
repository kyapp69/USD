#include "pxr/base/arch/defines.h"
#include "pxr/base/vt/api.h"
#include "pxr/base/vt/cachedNew.h"

#include <cstdlib>
#include <tbb/spin_mutex.h>

// Size must be > sizeof(void*)
template<size_t Size, size_t BlockSize = 1024>
class VtCachedMallocImpl
{
public:
    typedef VtCachedMallocImpl this_t;
    static const size_t bin_size = Size;
    static const size_t block_size = BlockSize;

    static this_t& GetInstance()
    {
        static this_t s_instance;
        return s_instance;
    }

    void* Allocate()
    {
        tbb::spin_mutex::scoped_lock lock(_mutex);
        if (!_head) {
            _head = malloc(bin_size * block_size);
            auto *c = (char*)_head;
            for (size_t i = 0; i < block_size - 1; ++i) {
                ((Bin*)(c + bin_size*i))->next = (Bin*)(c + bin_size*(i + 1));
            }
            ((Bin*)(c + (bin_size*(block_size - 1))))->next = nullptr;
        }
        void *ret = _head;
        _head = ((Bin*)_head)->next;
        return ret;
    }

    void Free(void *addr)
    {
        tbb::spin_mutex::scoped_lock lock(_mutex);
        ((Bin*)addr)->next = (Bin*)_head;
        _head = addr;
    }

private:
    union Bin
    {
        Bin *next;
    };

    void *_head = nullptr;
    tbb::spin_mutex _mutex;
};

template<size_t Size> VT_API void* VtCachedMalloc()
{
    return VtCachedMallocImpl<Size>::GetInstance().Allocate();
}

template<size_t Size> VT_API void VtCachedFree(void *addr)
{
    VtCachedMallocImpl<Size>::GetInstance().Free(addr);
}

#define INSTANTIATE(Size)\
    template VT_API void* VtCachedMalloc<Size>();\
    template VT_API void VtCachedFree<Size>(void *addr);

INSTANTIATE(8)
INSTANTIATE(12)
INSTANTIATE(16)
INSTANTIATE(20)
INSTANTIATE(24)
INSTANTIATE(28)
INSTANTIATE(32)
INSTANTIATE(40)
INSTANTIATE(48)
INSTANTIATE(56)
INSTANTIATE(68)
INSTANTIATE(72)
INSTANTIATE(80)
INSTANTIATE(112)
INSTANTIATE(136)
INSTANTIATE(280)

#undef INSTANTIATE
