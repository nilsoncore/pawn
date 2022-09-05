#include <stdio.h> // printf() in main()

#include <assert.h>
#include <stdlib.h> // malloc(), realloc(), free()

#include <string.h> // memcpy(), strlen()

#include "allocator.h"
#include "common.h"

// #define ALLOCATOR_DEBUG
#define ALLOCATOR_DEBUG_PRINT_BUFFER_SIZE 512

static char get_plural_bytes(s64 number) {
    return (number == 1) ? '\0' : 's';
}

#undef max
#define max(a, b) (a > b) ? a : b

static System_Allocator __sys_allocator;
Allocator *sys_allocator = &__sys_allocator;

//
// Allocator
//
u8 *Allocator::allocate(u64 count, u64 size, Caller_Info caller) {
    ZoneScoped;
    
    u8 *allocated = try_allocate(count, size, caller);
    allocations += 1;

#ifdef ALLOCATOR_DEBUG
    printf("\n----------- Allocation #%llu -----------\n", allocations);
    if (allocated) {
        printf("Allocated %lld elements of type %s at 0x%p (type size: %lld byte(s), total: %lld byte(s)).\n", count, caller.type, allocated, size, count * size);
    } else {
        printf("Failed to allocate %lld elements of type %s (type size: %lld byte(s), total: %lld byte(s)).\n", count, caller.type, size, count * size);
    }
    printf("Allocation site: '%s' (%s:%d).\n", caller.function, caller.file, caller.line);
    printf("----------------------------------------\n\n");
#endif

    assert(allocated != NULL && "Failed to allocate.");
    return allocated;
}

u8 *Allocator::reallocate(void *memory_pointer, u64 old_count, u64 new_count, u64 size, Caller_Info caller) {
    ZoneScoped;
    
    u8 *reallocated = try_reallocate(memory_pointer, old_count, new_count, size, caller);
    reallocations += 1;
    
#ifdef ALLOCATOR_DEBUG
    printf("\n---------- Reallocation #%llu ----------\n", reallocations);
    if (reallocated) {
        printf("Reallocated %lld elements to %lld elements of type %s at 0x%p (type size: %lld byte(s), old total: %lld byte(s), new total: %lld byte(s)).\n", old_count, new_count, caller.type, memory_pointer, size, old_count * size, new_count * size);
    } else {
        printf("Failed to reallocate %lld elements to %lld elements of type %s (type size: %lld byte(s), old total: %lld byte(s), new total: %lld byte(s)).\n", old_count, new_count, caller.type, size, old_count * size, new_count * size);
    }
    printf("Reallocation site: '%s' (%s:%d).\n", caller.function, caller.file, caller.line);
    printf("----------------------------------------\n\n");
#endif

    assert(reallocated != NULL && "Failed to reallocate.");
    return reallocated;
}

void Allocator::deallocate(void *memory_pointer, Caller_Info caller) {
    ZoneScoped;
    
    try_deallocate(memory_pointer, caller);
    deallocations += 1;
    
#ifdef ALLOCATOR_DEBUG
    printf("\n---------- Deallocation #%llu ----------\n", deallocations);
    printf("Deallocated 0x%p.\n", memory_pointer);
    printf("Deallocation site: '%s' (%s:%d).\n", caller.function, caller.file, caller.line);
    printf("----------------------------------------\n\n");
#endif
}

//
// System_Allocator
//
u8 *System_Allocator::try_allocate(u64 count, u64 size, Caller_Info caller) {
    ZoneScoped;
    
    u8 *allocated = (u8 *)malloc(count * size);
    return allocated;
}

u8 *System_Allocator::try_reallocate(void *memory_pointer, u64 old_count, u64 new_count, u64 size, Caller_Info caller) {
    ZoneScoped;
    
    u8 *reallocated = (u8 *)realloc(memory_pointer, new_count * size);
    return reallocated;
}

void System_Allocator::try_deallocate(void *memory_pointer, Caller_Info caller) {
    ZoneScoped;
    
    free(memory_pointer);
}

//
// Linear_Allocator
//
void Linear_Allocator::init(void *memory_pointer, u64 size) {
    ZoneScoped;
    
    assert(memory_pointer != NULL && "Trying to initialize 'Linear_Allocator' with NULL pointer.");
    
    memory_start = (u8 *)memory_pointer;
    memory_end = memory_start + size;
    cursor = memory_start;
    cursor_max = cursor;

    allocated = size;
}

void Linear_Allocator::deinit() {
    ZoneScoped;
    
    assert(memory_start != NULL && "Allocator is not initialized or already deinitialized.");
    free(memory_start);
    
    memory_start = NULL;
    memory_end = NULL;
}

void Linear_Allocator::clear(bool zero_memory) {
    ZoneScoped;

    assert(memory_start != NULL && "Allocator is not initialized or already deinitialized.");
    cursor = memory_start;
    // maybe keep max usage info on clear?
    // cursor_max = cursor;

    if (zero_memory)  memset(memory_start, 0, allocated * sizeof(u8));
}

u64 Linear_Allocator::occupied() {
    return cursor - memory_start;
}

static bool power_of_2(u64 x) {
    // u64 x   = 4
    //     x   = 0x100
    //     x-1 = 0x011
    // x & x-1 = 0x000
    //         => x (4) is power of 2.
    return (x & (x-1)) == 0;
}

u8 *Linear_Allocator::try_allocate(u64 count, u64 size, Caller_Info caller) {
    ZoneScoped;
    
    assert(power_of_2(size) && "Allocation size (alignment) must be power of 2.");
    u8 *aligned = (u8 *)(((u64)(cursor + size - 1)) & ~(size - 1));
    if (aligned + count > memory_end)  return NULL;

    cursor = aligned + count;
    cursor_max = max(cursor, cursor_max);
    return aligned;
}

u8 *Linear_Allocator::try_reallocate(void *memory_pointer, u64 old_count, u64 new_count, u64 size, Caller_Info caller) {
    ZoneScoped;
    
    if (!memory_pointer)  return try_allocate(new_count, size, caller);

    if (memory_pointer == cursor - old_count && (u64)memory_pointer % size == 0) {
        u8 *new_cursor = cursor - old_count + new_count;
        
        if (new_cursor > memory_end)  return NULL;
        if (new_cursor < cursor)      assert(false && "New cursor is below the current cursor for some reason.");

        cursor = new_cursor;
        cursor_max = max(cursor, cursor_max);
        return (u8 *)memory_pointer;
    }

    u8 *new_memory_pointer = try_allocate(new_count, size, caller);
    if (!new_memory_pointer)  return NULL;

    memcpy(new_memory_pointer, memory_pointer, old_count);
    return new_memory_pointer;
}

void Linear_Allocator::try_deallocate(void *memory_pointer, Caller_Info caller) {
    ZoneScoped;
    
    if (!memory_pointer) {
        clear(false);
        return;
    }
    
#ifdef ALLOCATOR_DEBUG
    printf("Warning: trying to deallocate memory pointer %p using Arena_Allocator!\n", memory_pointer);
    printf("Deallocation site: '%s' (%s:%d).\n", caller.function, caller.file, caller.line);
#endif

    assert(false && "Trying to deallocate with 'Linear_Allocator'.");
}

void test_allocators() {
    ZoneScoped;
    
    char *buffer = ALLOC(sys_allocator, 16, char);
    buffer = REALLOC(sys_allocator, buffer, 16, 32, char);
    FREE(sys_allocator, buffer);

    Linear_Allocator linear_allocator;
    u8 *arena_memory = ALLOC(sys_allocator, 1024, u8);
    linear_allocator.init(arena_memory, 1024);
    
    Allocator *a = &linear_allocator;

    char *buffer1 = ALLOC(a, 32, char);
    const char *msg1 = "Hello, world!";
    memcpy(buffer1, msg1, strlen(msg1)+1);

    printf("1 - buffer1 pointer: 0x%p\n", buffer1);
    printf("1 - buffer1  string: '%s'\n", buffer1);

    char *buffer2 = ALLOC(a, 32, char);
    const char *msg2 = "Goodbye, sadness!";
    memcpy(buffer2, msg2, strlen(msg2)+1);

    printf("2 - buffer2 pointer: 0x%p\n", buffer2);
    printf("2 - buffer2  string: '%s'\n", buffer2);
    printf("2 - diff(buffer1, buffer2): %lld\n", (s64)(buffer1 - buffer2));
    
    buffer1 = REALLOC(a, buffer1, 32, 64, char);
    const char *msg3 = "What's up, people!";
    memcpy(buffer1, msg3, strlen(msg3)+1);

    printf("3 - buffer1 pointer: 0x%p\n", buffer1);
    printf("3 - buffer1  string: '%s'\n", buffer1);
    printf("3 - diff(buffer1, buffer2): %lld\n", (s64)(buffer1 - buffer2));

    linear_allocator.clear(false);

    _Allocator _sys_allocator2;
    _Allocator *sys_allocator2 = &_sys_allocator2;
    char *buffer3 = ALLOC(sys_allocator2, 64, char);

    printf("4 - buffer3 pointer: 0x%p\n", buffer3);
    printf("4 - buffer3  string: '%s'\n", buffer3);

    buffer3 = REALLOC(sys_allocator2, buffer3, 64, 128, char);

    printf("5 - buffer3 pointer: 0x%p\n", buffer3);
    printf("5 - buffer3  string: '%s'\n", buffer3);

    FREE(sys_allocator2, buffer3);
}

/*
    u64 size = 4
    u64 alignment = 16
    cursor = 0000_01F8_6B95_2430

    u8 *aligned = (u8 *) ((cursor + alignment - 1) & ~(alignment - 1))

                                       cursor  = 0x0000_0000_0000_0000__0000_0001_1111_1000__0110_1011_1001_0101__0010_0100_0011_0000

                                    alignment  = 0x0000_0000_0000_0000__0000_0000_0000_0000__0000_0000_0000_0000__0000_0000_0001_0000

                                alignment - 1  = 0x0000_0000_0000_0000__0000_0000_0000_0000__0000_0000_0000_0000__0000_0000_0000_1111

                       cursor + alignment - 1  = 0x0000_0000_0000_0000__0000_0001_1111_1000__0110_1011_1001_0101__0010_0100_0011_1111

                              ~(alignment - 1) = 0x1111_1111_1111_1111__1111_1111_1111_1111__1111_1111_1111_1111__1111_1111_1111_0000
   
   (cursor + alignment - 1) & ~(alignment - 1) = 0x0000_0000_0000_0000__0000_0001_1111_1000__0110_1011_1001_0101__0010_0100_0011_0000
*/

//
// Allocator with function pointers
//
u8 *system_allocate(u64 count, u64 size, Caller_Info caller) {
    ZoneScoped;

    u8 *allocated = (u8 *)malloc(count * size);
    return allocated;
}

u8 *system_reallocate(void *memory_pointer, u64 old_count, u64 new_count, u64 size, Caller_Info caller) {
    ZoneScoped;

    u8 *reallocated = (u8 *)realloc(memory_pointer, new_count * size);
    return reallocated;
}

void system_deallocate(void *memory_pointer, Caller_Info caller) {
    ZoneScoped;

    free(memory_pointer);
}

//
// _Allocator
//
u8 *_Allocator::allocate(u64 count, u64 size, Caller_Info caller) {
    ZoneScoped;

    u8 *allocated = allocate_proc(count, size, caller);
    allocations += 1;

#ifdef ALLOCATOR_DEBUG
    printf("\n----------- Allocation #%llu -----------\n", allocations);
    if (allocated) {
        printf("Allocated %lld elements of type %s at 0x%p (type size: %lld byte(s), total: %lld byte(s)).\n", count, caller.type, allocated, size, count * size);
    } else {
        printf("Failed to allocate %lld elements of type %s (type size: %lld byte(s), total: %lld byte(s)).\n", count, caller.type, size, count * size);
    }
    printf("Allocation site: '%s' (%s:%d).\n", caller.function, caller.file, caller.line);
    printf("----------------------------------------\n\n");
#endif

    assert(allocated != NULL && "Failed to allocate.");
    return allocated;
}

u8 *_Allocator::reallocate(void *memory_pointer, u64 old_count, u64 new_count, u64 size, Caller_Info caller) {
    ZoneScoped;

    u8 *reallocated = reallocate_proc(memory_pointer, old_count, new_count, size, caller);
    reallocations += 1;

#ifdef ALLOCATOR_DEBUG
    printf("\n---------- Reallocation #%llu ----------\n", reallocations);
    if (reallocated) {
        printf("Reallocated %lld elements to %lld elements of type %s at 0x%p (type size: %lld byte(s), old total: %lld byte(s), new total: %lld byte(s)).\n", old_count, new_count, caller.type, memory_pointer, size, old_count * size, new_count * size);
    } else {
        printf("Failed to reallocate %lld elements to %lld elements of type %s (type size: %lld byte(s), old total: %lld byte(s), new total: %lld byte(s)).\n", old_count, new_count, caller.type, size, old_count * size, new_count * size);
    }
    printf("Reallocation site: '%s' (%s:%d).\n", caller.function, caller.file, caller.line);
    printf("----------------------------------------\n\n");
#endif

    assert(reallocated != NULL && "Failed to reallocate.");
    return reallocated;
}

void _Allocator::deallocate(void *memory_pointer, Caller_Info caller) {
    ZoneScoped;

    deallocate_proc(memory_pointer, caller);
    deallocations += 1;

#ifdef ALLOCATOR_DEBUG
    printf("\n---------- Deallocation #%llu ----------\n", deallocations);
    printf("Deallocated 0x%p.\n", memory_pointer);
    printf("Deallocation site: '%s' (%s:%d).\n", caller.function, caller.file, caller.line);
    printf("----------------------------------------\n\n");
#endif
}
