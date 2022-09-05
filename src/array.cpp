#include "array.h"

#include <stdio.h>

void array_test() {
    Array<char> str1 = new_array<char>(sys_allocator, (s64)(2+1));
    assert(add(&str1, 'h'));
    assert(add(&str1, 'i'));
    assert(add(&str1, '\0'));
    assert(!add(&str1, 'm'));

    printf("1 - str1: '%s'\n", str1.data);

    resize(&str1, 6+1);
    str1.data[str1.size-1] = ' ';
    assert(add(&str1, 'm'));
    assert(add(&str1, 'a'));
    assert(add(&str1, 'n'));
    assert(add(&str1, '\0'));
    assert(!add(&str1, 's'));

    printf("2 - str1: '%s'\n", str1.data);

    clear(&str1);

    // Array<Board_Square> squares123 = new_array<Board_Square>(sys_allocator, 8 * 8);
    // Array<Board_Player> players123 = new_array<Board_Player>(sys_allocator, 2);
}
