// Include Lua while temporarily suppressing its compiler warnings

// PUSH
#if defined(__clang__)
    #pragma clang diagnostic push
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
#elif defined(_MSC_VER)
    #pragma warning(push)
#endif

// Suppress warnings
#if defined(__clang__)
    #pragma clang diagnostic ignored "-Wpedantic"
    #pragma clang diagnostic ignored "-Wgnu-label-as-value"
#elif defined(__GNUC__)
    // You need to not use -Wpedantic with GCC
    #pragma GCC diagnostic ignored "-Wpedantic"
#elif defined(_MSC_VER)
    #pragma warning(disable : 4701 4702 4310 4244 4334 4267)
#endif

// don't build the interpreter, just the library
#define MAKE_LIB
#include "../lua/onelua.c"

// POP
#if defined(__clang__)
    #pragma clang diagnostic pop
#elif defined(__GNUC__)
    #pragma GCC diagnostic pop
#elif defined(_MSC_VER)
    #pragma warning(pop)
#endif
