#pragma  once


// implement in cpp file to prevent each dll have it's own instance.
template<size_t Size> VT_API void* VtCachedMalloc();
template<size_t Size> VT_API void VtCachedFree(void *addr);

#ifdef USD_ENABLE_CACHED_NEW
    #define VT_DEFINE_CACHED_OPERATOR_NEW(T)\
        void* operator new (size_t) { return VtCachedMalloc<sizeof(T)>(); }\
        void operator delete (void* addr) { VtCachedFree<sizeof(T)>(addr); }
#else
    #define VT_DEFINE_CACHED_OPERATOR_NEW(T)
#endif
