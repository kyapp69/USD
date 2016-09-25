#include <boost/container/vector.hpp>
#include "pxr/base/arch/defines.h"
#include "pxr/base/vt/api.h"
#include "pxr/base/vt/alignedAllocator.h"

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
