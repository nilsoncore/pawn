#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#define YPL_TYPES_BY_TYPEDEF
#define YPL_TYPES_USING_EXACT
#include "ypl_types.h" // u8, u64

#define __ALLOCATOR_CALLER Caller_Info { "(none)", __FUNCSIG__, __FILE__, __LINE__ }
#define __ALLOCATOR_TYPE_CALLER(T) Caller_Info { "'" #T "'", __FUNCSIG__, __FILE__, __LINE__ }

#define ALLOC(allocator, count, T) (T *)allocator->allocate(count, sizeof(T), __ALLOCATOR_TYPE_CALLER(T))
#define REALLOC(allocator, memory_pointer, old_count, new_count, T) (T *)allocator->reallocate(memory_pointer, old_count, new_count, sizeof(T), __ALLOCATOR_TYPE_CALLER(T))
#define FREE(allocator, memory_pointer) allocator->deallocate(memory_pointer, __ALLOCATOR_CALLER)

struct Caller_Info {
    const char *type;
    const char *function;
    const char *file;
    int line;
};

struct Allocator {
    u64 allocations = 0;
    u64 reallocations = 0;
    u64 deallocations = 0;

    u8 *allocate(u64 count, u64 size, Caller_Info caller);
    u8 *reallocate(void *memory_pointer, u64 old_count, u64 new_count, u64 size, Caller_Info caller);
    void deallocate(void *memory_pointer, Caller_Info caller);

    virtual u8 *try_allocate(u64 count, u64 size, Caller_Info caller) = 0;
    virtual u8 *try_reallocate(void *memory_pointer, u64 old_count, u64 new_count, u64 size, Caller_Info caller) = 0;
    virtual void try_deallocate(void *memory_pointer, Caller_Info caller) = 0;
};

extern Allocator *sys_allocator;

struct System_Allocator : Allocator {
    u8 *try_allocate(u64 count, u64 size, Caller_Info caller);
    u8 *try_reallocate(void *memory_pointer, u64 old_count, u64 new_count, u64 size, Caller_Info caller);
    void try_deallocate(void *memory_pointer, Caller_Info caller);
};

struct Linear_Allocator : Allocator {
    u8 *memory_start = NULL;
    u8 *memory_end = NULL;
    u8 *cursor = NULL;
    u8 *cursor_max = NULL;
    
    u64 allocated = 0;

    u8 *try_allocate(u64 count, u64 size, Caller_Info caller);
    u8 *try_reallocate(void *memory_pointer, u64 old_count, u64 new_count, u64 size, Caller_Info caller);
    void try_deallocate(void *memory_pointer, Caller_Info caller);

    void init(void *memory_pointer, u64 size);
    void clear(bool zero_memory);
    void deinit();

    u64 occupied();
};

struct Temp_Allocator : Allocator {
    u8 *try_allocate(u64 count, u64 size, Caller_Info caller);
    u8 *try_reallocate(void *memory_pointer, u64 old_count, u64 new_count, u64 size, Caller_Info caller);
    void try_deallocate(void *memory_pointer, Caller_Info caller);
};

void test_allocators();

typedef u8 *(*Allocate_Proc)(u64 /* count */, u64 /* size */, Caller_Info /* caller */);
typedef u8 *(*Reallocate_Proc)(void * /* memory_pointer */, u64 /* old_count */, u64 /* new_count */, u64 /* size */, Caller_Info /* caller */);
typedef void (*Deallocate_Proc)(void * /* memory_pointer */, Caller_Info /* caller */);

u8 *system_allocate(u64 count, u64 size, Caller_Info caller);
u8 *system_reallocate(void *memory_pointer, u64 old_count, u64 new_count, u64 size, Caller_Info caller);
void system_deallocate(void *memory_pointer, Caller_Info caller);

struct _Allocator {
    u64 allocations = 0;
    u64 reallocations = 0;
    u64 deallocations = 0;

    u8 *allocate(u64 count, u64 size, Caller_Info caller);
    u8 *reallocate(void *memory_pointer, u64 old_count, u64 new_count, u64 size, Caller_Info caller);
    void deallocate(void *memory_pointer, Caller_Info caller);

    Allocate_Proc allocate_proc = system_allocate;
    Reallocate_Proc reallocate_proc = system_reallocate;
    Deallocate_Proc deallocate_proc = system_deallocate;
};

#endif /* ALLOCATOR_H */
