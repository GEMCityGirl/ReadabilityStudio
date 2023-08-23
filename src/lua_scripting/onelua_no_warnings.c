// Include Lua while temporarily suppressing its compiler warnings

// PUSH
#if defined(__clang__)
    #pragma clang diagnostic push
#elif defined(__gcc__)
    #pragma GCC diagnostic push
#elif defined(_MSC_VER)
    #pragma warning(push)
#endif

// Suppress warnings
#if defined(__clang__)
    // #pragma clang diagnostic ignored ""
#elif defined(__gcc__)
    // You need to not use -Wpedantic with GCC;
    // currently, you can't turn that off this way.
    // This is because 'ignored' can only ignore specific warnings,
    // not entire classes of warning like 'all' or 'pendantic'.
    // #pragma GCC diagnostic ignored ""
#elif defined(_MSC_VER)
    #pragma warning(disable: 4701 4702 4310 4244)
#endif

// don't build the interpretter, just the library
#define MAKE_LIB
#include "../lua/onelua.c"

// POP
#if defined(__clang__)
    #pragma clang diagnostic pop
#elif defined(__gcc__)
    #pragma GCC diagnostic pop
#elif defined(_MSC_VER)
    #pragma warning(pop)
#endif
