#ifndef PAWN_COMMON_H
#define PAWN_COMMON_H

#include <assert.h>
#include <tracy/Tracy.hpp>

#define YPL_TYPES_BY_TYPEDEF
#define YPL_TYPES_USING_EXACT
#include "ypl_types.h"

#include "allocator.h"
#include "array.h"
#include "math.h"

//
// --- Macros ---
//
#define For(x) for(int it = 0; it < x; it++)
#define ForFrom(x, n) for(int it = n; it < x; it++)

#define STRINGIFY_HELPER( a ) #a
#define STRINGIFY( a ) STRINGIFY_HELPER( a )
#define CONCAT_HELPER( a, b ) a##b
#define CONCAT( a, b ) CONCAT_HELPER( a, b )
#define COUNTER_NAME( x ) CONCAT( x, __COUNTER__ )
#define LINE_NAME( x ) CONCAT( x, __LINE__ )

#define defer const auto & COUNTER_NAME( __defer_ ) = DeferHelper() + [&]()

//
// --- Constants ---
//
const int HOT_MEMORY_ARENA_CAPACITY = 64 * 1024 * sizeof(u8); // 64KB
const int COLD_MEMORY_ARENA_CAPACITY = 256 * 1024 * sizeof(u8); // 256KB

//
// --- Structs ---
//
template <typename F>
struct ScopeExit {
    ScopeExit(F _f) : f(_f) { }
    ~ScopeExit() { f(); }
    F f;
};

struct DeferHelper {
    template <typename F>
    ScopeExit<F> operator+(F f) { return f; }
};

//
// --- Functions ---
//
void mem_set(void *data_pointer, int value, size_t bytes_size);
void mem_zero(void *data_pointer, size_t bytes_size);
int string_length(const char *text);

#endif /* PAWN_COMMON_H */
