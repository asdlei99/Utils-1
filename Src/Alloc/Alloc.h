#pragma once

#include <cstdlib>

#include "../Misc/Common.h"
#include "Malloc.h"

namespace AGZ {

/*
    concept Allocator
    {
        // Support any 2-pow-bytes alignment?
        static constexpr bool AnyAlign;
        static constexpr bool ThreadSafe;

        // Throw bad_alloc if failed
        static void *Malloc(size_t size);
        static void *Malloc(size_t size, size_t align);

        static void Free(void *ptr);
        static void FreeAligned(void *ptr);
    }
*/

/*
    基于C run-time library实现的内存分配器
*/
class CRTAllocator
{
public:

    static constexpr bool AnyAlign   = true;
    static constexpr bool ThreadSafe = true;

    static void *Malloc(size_t size)
    {
        return alloc_throw<void>(std::malloc, size);
    }

    static void *Malloc(size_t size, size_t align)
    {
        return alloc_throw<void>(AGZ::aligned_alloc, align, size);
    }

    static void Free(void *ptr)
    {
        std::free(ptr);
    }

    static void FreeAligned(void *ptr)
    {
        AGZ::aligned_free(ptr);
    }
};

using DefaultAllocator = CRTAllocator;

} // namespace AGZ
