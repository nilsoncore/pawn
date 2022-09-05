/*

    Simple string imlementation in C++ with C-style code.

    Available functional:

    [ ] - to string
    [x] - create new string from C char array/by copy -- new_string() / allocate_string()
    [x]   - create new string from another's chunk -- new_string(from, start, end)
    [x] - clone string (create reference) from another string -- clone_string() / reference_string()
    [x] - compare
    [x]   - equal -> bool -- strings_equal()
    [x]   - equal (ignore case) -> bool -- strings_equal_ignore_case()
    [x]   - difference -> int -- strings_difference()
    [x]   - difference (ignore case) -> int -- strings_difference_ignore_case()
    [x] - contains -- string_contains()
    [x] - starts/ends with (prefix/suffix) -- string_starts_with() / string_ends_with()
    [x] - split -- split_string()
    [x] - join -- join_strings()
    [x] - String_Array -- new_string_array()
    [x]   - add (single/multiple) -- add_string() / add_strings()
    [x]   - pop (single/multiple) -- pop_string() / pop_strings()
    [ ]   - insert
    [ ]   - remove (removes string at a given index and shifts whole array to close the gap)
    [?]   - clear
    [ ]   - sort
    [x]   - swap (example: string 1 put at string 2 index and vice versa) -- swap_strings()
    [ ] - replace
    [ ]   - first match
    [ ]   - all matches
    [ ] - delete
    [ ]   - first match
    [ ]   - all matches
    [ ] - count all matches
    [~] - set case
    [x]   - upper -- string_uppercase() / string_to_uppercase()
    [x]   - lower -- string_lowercase() / string_to_lowercase()
    [x]   - title -- string_titlecase() / string_to_titlecase()
    [ ]   - sentence (First letter after a specified character with whitespace is uppercase)
    [x] - remove leading/tailing whitespace -- trim_leading_whitespaces() / trim_tailing_whitespaces() / trim_whitespaces()
    [ ] - format (format what ???) ((format_int(), format_float(), ...))
    [ ] - remove matching char/char array/string
    [ ] - cut/remove string from X to Y area
    [ ] - insert char/char array/string at X position
    [ ] - clear

    TODO:

    [ ] - remove all functions with pointers. String already contains pointer to C char array,
          so there's no need to pass by pointer (except cases when we modify its size).
*/

#include <stdlib.h>
#include <stdio.h>

const char ALPHABET_UPPERCASE[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
const char ALPHABET_LOWERCASE[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };

struct string {
    int size = 0;
    char *data = NULL;
};

struct String_Array {
    string *data = NULL;
    int size = 0;
    int capacity = 0;
};

void inline print(string *s) {
    printf("string(%p): { data: \"%s\", size: %d }\n", s, s->data, s->size);
}

void inline print(String_Array *array) {
    printf("String_Array(%p): { data: [", array);

    if (array->size > 0) {
        printf("\"%s\"", array->data[0].data);
        for (int i = 1; i < array->size; i++) {
            printf(", \"%s\"", array->data[i].data);
        }
    }
    printf("], size: %d, capacity: %d }\n", array->size, array->capacity);
}

// WARNING: This function can calculate the size of a char array
// only if it's initialized! Non-initialized char array may contain
// the NULL-terminating symbol '\0' in the middle of it and, as a result,
// will break the algorithm.
int char_array_size(const char *array) {
    int i = 0;
    while (array[i] != '\0') // NULL == '\0'
        i++;

    return ++i;
}

int char_array_size(char *array) {
    int i = 0;
    while (array[i] != '\0') // NULL == '\0'
        i++;

    return ++i;
}

// This function is a-like  fill_char_array()  one with additional
// size information in the arguments. Size information is added specifically
// to initialize a char array which has garbage values, otherwise the
// function couldn't fill the array because of size calculation errors.
// SEE: char_array_size()
void init_char_array(char *array, int array_size, const char *data) {
    if (array_size < 1) return;

    int data_size = char_array_size(data);
    if (data_size < 1) return;

    int end = (data_size > array_size) ? array_size : data_size;

    for (int i = 0; i < end; i++)
        array[i] = data[i];
}

// This function assumes that the array which will be filled is properly 
// initialized and have NULL-terminating symbol '\0' at the end.
void fill_char_array(char *array, const char *data) {
    int array_size = char_array_size(array);
    if (array_size < 1) return;

    int data_size = char_array_size(data);
    if (data_size < 1) return;

    int end = (data_size > array_size) ? array_size : data_size;

    for (int i = 0; i < end; i++)
        array[i] = data[i];
}

void fill_char_array(char *array, const char *data, int data_start) {
    int array_size = char_array_size(array);
    if (array_size < 1) return;

    int data_end = char_array_size(data);
    int data_region_size = data_end - data_start;
    for (int i = 0; i < data_region_size; i++)
        array[i] = data[data_start++];
}

void fill_char_array(char *array, const char *data, int data_start, int data_end) {
    int array_size = char_array_size(array);
    if (array_size < 1) return;

    int data_size = char_array_size(data);
    if (data_size < 1) return;

    int data_region_size = data_end - data_start;
    for (int i = 0; i < data_region_size; i++)
        array[i] = data[data_start++];
}

void fill_char_array(char *array, char *data, int data_start, int data_end) {
    int array_size = char_array_size(array);
    if (array_size < 1) return;

    int data_size = char_array_size(data);
    if (data_size < 1) return;

    int data_region_size = data_end - data_start;
    for (int i = 0; i < data_region_size; i++)
        array[i] = data[data_start++];
}

string *allocate_string(char *data) {
    int size = char_array_size(data);
    string *result = (string *) malloc(sizeof(string));
    // We allocated memory for a string, but it doesn't imply
    // that it will be initialized, so we have to initialize it manually.
    if (size < 1) {
        result->data = NULL;
        result->size = 0;
    } else {
        result->data = data;
        result->size = size;
    }

    return result;
}

string *allocate_string(const char *data) {
    int size = char_array_size(data);
    string *result = (string *) malloc(sizeof(string));
    // We allocated memory for a string, but it doesn't imply
    // that it will be initialized, so we have to initialize it manually.
    if (size < 1) {
        result->data = NULL;
        result->size = 0;
    } else {
        // char *new_data = (char *) malloc(size * sizeof(char));
        // init_char_array(new_data, size, data);
        // result->data = new_data;
        result->data = (char *) data;
        result->size = size;
    }

    return result;
}

string *allocate_string(string from) {
    int size = from.size;
    string *result = (string *) malloc(sizeof(string));
    if (size < 1) {
        result->data = NULL;
        result->size = 0;
    } else {
        char *new_data = (char *) malloc(size * sizeof(char));
        init_char_array(new_data, size, from.data);
        result->data = new_data;
        result->size = size;
    }

    return result;
}

string *allocate_string(string *from) {
    int size = from->size;
    string *result = (string *) malloc(sizeof(string));
    if (size < 0) {
        result->data = NULL;
        result->size = 0;
    } else {
        char *new_data = (char *) malloc(size * sizeof(char));
        init_char_array(new_data, size, from->data);
        result->data = new_data;
        result->size = size;
    }

    return result;
}

string new_string(char *data) {
    int size = char_array_size(data);  
    string result;
    if (size > 1) {
        // char *new_data = (char *) malloc(size * sizeof(char));
        // init_char_array(new_data, size, data);
        // result.data = new_data;
        result.data = data;
        result.size = size;
    }

    return result;
}

string new_string(const char *data) {
    int size = char_array_size(data);
    string result;
    if (size > 1) {
        char *new_data = (char *) malloc(size * sizeof(char));
        init_char_array(new_data, size, data);
        result.data = new_data;
        // result.data = (char *) data;
        result.size = size;
    }

    return result;
}

string new_string(string from) {
    int size = from.size;
    string result;
    if (size > 1) {
        char *new_data = (char *) malloc(size * sizeof(char));
        init_char_array(new_data, size, from.data);
        result.data = new_data;
        result.size = size;
    }

    return result;
}

string new_string(string *from) {
    int size = from->size;
    string result;
    if (size > 0) {
        char *new_data = (char *) malloc(size * sizeof(char));
        init_char_array(new_data, size, from->data);
        result.data = new_data;
        result.size = size;
    }

    return result;
}

string clone_string(string from) {
    string result;
    result.data = from.data;
    result.size = from.size;
    return result;
}

/*
string clone_string(string *from) {
    string result;
    result.data = from->data;
    result.size = from->size;
    return result;
}
*/

string *reference_string(string from) {
    string *result = (string *) malloc(sizeof(string));
    result->data = from.data;
    result->size = from.size;
    return result;
}

/*
string *reference_string(string *from) {
    string *result = (string *) malloc(sizeof(string));
    result->data = from->data;
    result->size = from->size;
    return result;
}
*/

/*
string copy_string_region(string from, int start, int end = from.size-1) { 
    string result;
    if (begin < from.size) {
        int region_size = start - end;
        if (region_size < 0)
            region_size = -region_size; // Make it positive.

        char *new_data = (char *) malloc(region_size * sizeof(char));
        fill_char_array(new_data,
            // !!!
    }

    return result;
}
*/

void init_string(string *s, char *data) {
    s->data = data;
    s->size = char_array_size(data);
}

string string_lowercase(string from) {
    for (int i = 0; i < from.size-1; i++)
        for (int j = 0; j < 26-1; j++)
            if (from.data[i] == ALPHABET_UPPERCASE[j])
                from.data[i] = ALPHABET_LOWERCASE[j];

    return from;
}

string string_lowercase(string *from) {
    string result = new_string(from);

    for (int i = 0; i < result.size-1; i++)
        for (int j = 0; j < 26-1; j++)
            if (result.data[i] == ALPHABET_UPPERCASE[j])
                result.data[i] = ALPHABET_LOWERCASE[j];

    return result;
}

void string_to_lowercase(string *target) {
    for (int i = 0; i < target->size-1; i++)
        for (int j = 0; j < 26-1; j++)
            if (target->data[i] == ALPHABET_UPPERCASE[j])
                target->data[i] = ALPHABET_LOWERCASE[j];
}

string string_uppercase(string from) {
    for (int i = 0; i < from.size-1; i++)
        for (int j = 0; j < 26-1; j++)
            if (from.data[i] == ALPHABET_LOWERCASE[j])
                from.data[i] = ALPHABET_UPPERCASE[j];

    return from;
}

string string_uppercase(string *from) {
    string result = new_string(*from);

    for (int i = 0; i < result.size-1; i++)
        for (int j = 0; j < 26-1; j++)
            if (result.data[i] == ALPHABET_LOWERCASE[j])
                result.data[i] = ALPHABET_UPPERCASE[j];

    return result;
}

void string_to_uppercase(string *target) {
    for (int i = 0; i < target->size-1; i++)
        for (int j = 0; j < 26-1; j++)
            if (target->data[i] == ALPHABET_LOWERCASE[j])
                target->data[i] = ALPHABET_UPPERCASE[j];
}

char uppercase_letter(char of) {
    for (int i = 0; i < 26-1; i++)
        if (ALPHABET_LOWERCASE[i] == of)
            return ALPHABET_UPPERCASE[i];

    return NULL; // '\0'
}

char lowercase_letter(char of) {
    for (int i = 0; i < 26-1; i++)
        if (ALPHABET_UPPERCASE[i] == of)
            return ALPHABET_LOWERCASE[i];

    return NULL; // '\0'
}

bool is_lowercase_letter(char letter) {
    for (int i = 0; i < 26-1; i++)
        if (ALPHABET_LOWERCASE[i] == letter)
            return true;

    return false;
}

bool is_uppercase_letter(char letter) {
    for (int i = 0; i < 26-1; i++)
        if (ALPHABET_UPPERCASE[i] == letter)
            return true;

    return false;
}

string string_titlecase(string from) {
    string result = string_lowercase(from);

    // First letter is always uppercase,
    // so we set it manually.
    if (is_lowercase_letter(result.data[0]))
        result.data[0] = uppercase_letter(result.data[0]);

    for (int i = 1; i < result.size-1; i++)
        if (result.data[i-1] == ' ')
            result.data[i] = uppercase_letter(result.data[i]);

    return result;
}

string string_titlecase(string *from) {
    string result = string_lowercase(*from);

    // First letter is always uppercase,
    // so we set it manually.
    if (is_lowercase_letter(result.data[0]))
        result.data[0] = uppercase_letter(result.data[0]);

    for (int i = 1; i < result.size-1; i++)
        if (result.data[i-1] == ' ')
            result.data[i] = uppercase_letter(result.data[i]);

    return result;
}

void string_to_titlecase(string *target) {
    // First letter is always uppercase,
    // so we set it manually.
    if (is_lowercase_letter(target->data[0]))
        target->data[0] = uppercase_letter(target->data[0]);

    for (int i = 1; i < target->size-1; i++)
        if (target->data[i-1] == ' ')
            target->data[i] = uppercase_letter(target->data[i]);
}

string copy_from_string(string *from, int start, int end) {
    string result;
    result.data = (char *) malloc(64 * sizeof(char));
    int j = end - start;

    for (int i = 0; i != j; i++)
        result.data[i] = from->data[start++];

    return result;
}

void copy_from_string_to_string(string *from, string *to) {
    to->data = from->data;
    to->size = from->size;
}

void copy_from_string_to_string(string *from, string *to, int start, int end) {
    int size = end - start;
    for (int i = 0; i != size; i++) {
        to->data[i] = from->data[start];
        start++;
    }
    to->size = char_array_size(to->data);
}

bool strings_equal(string first, string second) {
    if (first.size != second.size) return false;

    for (int i = 0; i < first.size-1; i++)
        if (first.data[i] != second.data[i])
            return false;

    return true;
}

bool strings_equal(string *first, string *second) {
    if (first->size != second->size) return false;

    for (int i = 0; i < first->size-1; i++)
        if (first->data[i] != second->data[i])
            return false;

    return true;
}

bool strings_equal_ignore_case(string first, string second) {
    if (first.size != second.size) return false;

    string first_lowercase = string_lowercase(first);
    string second_lowercase = string_lowercase(second);

    for (int i = 0; i < first.size-1; i++)
        if (first.data[i] != second.data[i])
            return false;

    return true;
}

bool strings_equal_ignore_case(string *first, string *second) {
    if (first->size != second->size) return false;

    string first_lowercase = string_lowercase(first);
    string second_lowercase = string_lowercase(second);

    for (int i = 0; i < first_lowercase.size-1; i++)
        if (first_lowercase.data[i] != second_lowercase.data[i])
            return false;

    return true;
}

int strings_difference(string first, string second) {
    int size_difference = first.size - second.size;
    if (size_difference < 0)
        size_difference = -size_difference; // Make it positive.

    int value_difference = 0;
    for (int i = 0; i < size_difference-1; i++) {
        if (first.data[i] != second.data[i])
            value_difference++;
    }

    return value_difference + size_difference;
}

int strings_difference(string *first, string *second) {
    int size_difference = first->size - second->size;
    int min_string_size = 0;
    if (size_difference < 0) {
        min_string_size = first->size;
        size_difference = -size_difference; // Make it positive.
    } else {
        min_string_size = second->size;
    }

    int value_difference = 0;
    for (int i = 0; i < min_string_size-1; i++) {
        if (first->data[i] != second->data[i])
            value_difference++;
    }

    return value_difference + size_difference;
}

int strings_difference_ignore_case(string first, string second) {
    int size_difference = first.size - second.size;
    if (size_difference < 0)
        size_difference = -size_difference; // Make it positive.

    string first_lowercase = string_lowercase(first);
    string second_lowercase = string_lowercase(second);

    int value_difference = 0;
    for (int i = 0; i < size_difference-1; i++)
        if (first.data[i] != second.data[i])
            value_difference++;

    return value_difference + size_difference;
}

int strings_difference_ignore_case(string *first, string *second) {
    int size_difference = first->size - second->size;
    int min_string_size = 0;
    if (size_difference < 0) {
        min_string_size = first->size;
        size_difference = -size_difference; // Make it positive.
    } else {
        min_string_size = second->size;
    }

    string first_lowercase = string_lowercase(first);
    string second_lowercase = string_lowercase(second);

    int value_difference = 0;
    for (int i = 0; i < min_string_size-1; i++)
        if (first_lowercase.data[i] != second_lowercase.data[i])
            value_difference++;

    return value_difference + size_difference;
}

string trim_leading_whitespaces(string from) {
    int whitespaces_count = 0;
    for (int i = 0; i < from.size-1; i++)
        if (from.data[i] == ' ') whitespaces_count++;
        else break;

    if (whitespaces_count == 0) {
        return from;
    } else {
        string result;
        int new_size = from.size - whitespaces_count;
        char *new_data = (char *) malloc(new_size * sizeof(char));
        fill_char_array(new_data, from.data, whitespaces_count, from.size);
        result.data = new_data;
        result.size = new_size;
        return result;
    }
}

string trim_leading_whitespaces(string *from) {
    int whitespaces_count = 0;
    for (int i = 0; i < from->size-1; i++)
        if (from->data[i] == ' ') whitespaces_count++;
        else break;

    if (whitespaces_count == 0) {
        return *from;
    } else {
        string result;
        int new_size = from->size - whitespaces_count;
        char *new_data = (char *) malloc(new_size * sizeof(char));
        fill_char_array(new_data, from->data, whitespaces_count, from->size);
        result.data = new_data;
        result.size = new_size;
        return result;
    }
}

string trim_tailing_whitespaces(string from) {
    int whitespaces_count = 0;
    for (int i = from.size-2; i > -1; i--)
        if (from.data[i] == ' ') whitespaces_count++;
        else break;

    if (whitespaces_count == 0) {
        return from;
    } else {
        string result;
        int new_size = from.size - whitespaces_count;
        char *new_data = (char *) malloc(new_size * sizeof(char));
        fill_char_array(new_data, from.data, 0, from.size - whitespaces_count);
        // Here we add NULL-terminating symbol '\0' at the end manually,
        // because if a string has tailing whitespaces, the last character
        // is always going to be whitespace.
        new_data[new_size-1] = NULL;
        result.data = new_data;
        result.size = new_size;
        return result;
    }
}

string trim_tailing_whitespaces(string *from) {
    int whitespaces_count = 0;
    for (int i = from->size-2; i > -1; i--)
        if (from->data[i] == ' ') whitespaces_count++;
        else break;

    if (whitespaces_count == 0) {
        return *from;
    } else {
        string result;
        int new_size = from->size - whitespaces_count;
        char *new_data = (char *) malloc(new_size * sizeof(char));
        fill_char_array(new_data, from->data, 0, from->size - whitespaces_count);
        // Here we add NULL-terminating symbol '\0' at the end manually,
        // because if a string has tailing whitespaces, the last character
        // is always going to be whitespace.
        new_data[new_size-1] = NULL;
        result.data = new_data;
        result.size = new_size;
        return result;
    }
}

string trim_whitespaces(string from) {
    int leading_whitespaces = 0;
    int tailing_whitespaces = 0;

    // Count leading whitespaces.
    for (int i = 0; i < from.size-1; i++)
        if (from.data[i] == ' ') leading_whitespaces++;
        else break;

    // Count tailing whitespaces.
    for (int i = from.size-2; i > -1; i--)
        if (from.data[i] == ' ') tailing_whitespaces++;
        else break;

    if (leading_whitespaces == 0 && tailing_whitespaces == 0) {
        return from;
    } else {
        string result;
        int new_size = from.size - leading_whitespaces - tailing_whitespaces;
        char *new_data = (char *) malloc(new_size * sizeof(char));
        fill_char_array(new_data, from.data, leading_whitespaces, from.size - tailing_whitespaces);
        // Here we add NULL-terminating symbol '\0' at the end manually,
        // because if a string has tailing whitespaces, the last character
        // is always going to be whitespace.
        new_data[new_size-1] = NULL;
        result.data = new_data;
        result.size = new_size;
        return result;
    }
}

string trim_whitespaces(string *from) {
    int leading_whitespaces = 0;
    int tailing_whitespaces = 0;

    // Count leading whitespaces.
    for (int i = 0; i < from->size-1; i++)
        if (from->data[i] == ' ') leading_whitespaces++;
        else break;

    // Count tailing whitespaces.
    for (int i = from->size-2; i > -1; i--)
        if (from->data[i] == ' ') tailing_whitespaces++;
        else break;

    if (leading_whitespaces == 0 && tailing_whitespaces == 0) {
        return *from;
    } else {
        string result;
        int new_size = from->size - leading_whitespaces - tailing_whitespaces;
        char *new_data = (char *) malloc(new_size * sizeof(char));
        fill_char_array(new_data, from->data, leading_whitespaces, from->size - tailing_whitespaces);
        // Here we add NULL-terminating symbol '\0' at the end manually,
        // because if a string has tailing whitespaces, the last character
        // is always going to be whitespace.
        new_data[new_size-1] = NULL;
        result.data = new_data;
        result.size = new_size;
        return result;
    }
}

// --- Prefix ---
bool string_starts_with(string from, string prefix) {
    for (int i = 0; i < prefix.size-1; i++)
        if (from.data[i] != prefix.data[i])
            return false;

    return true;
}

// --- Suffix ---
// In functions below we use size-2 for 'for loop' end,
// because:
//  -1 because of array indexing (starts from 0, ends with size-1);
//  -1 because NULL terminating symbol will always be the same.
// (although not checking NULL symbol may cause some problems in strings
// which don't contain it).
bool string_ends_with(string target, string suffix) {
    int cursor_position = target.size - suffix.size;
    for (int i = 0; i < suffix.size-2; i++, cursor_position++)
        if (target.data[cursor_position] != suffix.data[i])
            return false;

    return true;
}

bool string_contains(string target, string chunk) {
    if (chunk.size > target.size) return false;

    for (int i = 0, j = 0; i < target.size-1; i++) {
        if (target.data[i] == chunk.data[j]) j++;
        else j = 0;

        if (j+1 == chunk.size) return true;
    }

    return false;
}

void fill_string(string *target, char *array, int array_size) {
    if (array_size < 1) return;

    for (int i = 0; i < array_size; i++) {
        target->data[i] = array[i];
    }
}

String_Array new_string_array(int capacity) {
    String_Array result;
    if (capacity < 1) return result;

    result.data = (string *) malloc(capacity * sizeof(string));
    result.size = 0;
    result.capacity = capacity;

    return result;
}

String_Array new_string_array(string *array, int size) {
    String_Array result;
    if (size < 1) return result;

    result.data = array;
    result.size = size;
    result.capacity = size;

    return result;
}

void add_string(String_Array *array, string *target) {
    if (array->capacity < array->size+1) {
        array->data = (string *) realloc(array->data, 2 * array->capacity * sizeof(string));
        array->capacity = 2 * array->capacity;
    }

    array->data[array->size] = *target;
    array->size++;
}

void add_string(String_Array *array, string target) {
    if (array->capacity < array->size+1) {
        array->data = (string *) realloc(array->data, 2 * array->capacity * sizeof(string));
        array->capacity = 2 * array->capacity;
    }

    array->data[array->size] = target;
    array->size++;
}

void add_strings(String_Array *array, string *pointer, int size) {
    while (array->capacity < array->size + size) {
        array->data = (string *) realloc(array->data, 2 * array->capacity * sizeof(string));
        array->capacity = 2 * array->capacity;
    }

    for (int i = 0; i < size; i++) {
        array->data[array->size] = pointer[i];
        array->size++;
    }
}

void pop_string(String_Array *array) {
    if (array->size < 1) return;

    array->size--;
    array->data[array->size].data = NULL;
    array->data[array->size].size = 0;
}

void pop_strings(String_Array *array, int amount) {
    int to_index = array->size - amount;

    if (to_index < 0) to_index = 0;

    for (int i = array->size; i > to_index; i--) {
        array->size--;
        array->data[array->size].data = NULL;
        array->data[array->size].size = 0;
    }
}

string new_string(string *from, int start, int end) {
    int size = end - start + 1; // +1 for '\0'
    string result;
    if (size > 0) {
        char *new_data = (char *) malloc(size * sizeof(char));
        // init_char_array(new_data, size, from->data);
        fill_char_array(new_data, from->data, start, end);
        new_data[size-1] = '\0';
        result.data = new_data;
        result.size = size;
    }

    return result;
}

// Probably the slowest splitting algorithm ever.
// Please rework it if possible.
String_Array split_string(string *target, char separator = ' ') {
    int separators_found = 0;
    for (int i = 0; i < target->size; i++) {
        if (target->data[i] == separator) {
            separators_found++;
        }
    }

    unsigned short *separators_indexes = (unsigned short *) malloc(separators_found * sizeof(separators_indexes));
    int separators_found_second = 0;
    for (int i = 0; i < target->size; i++) {
        if (separators_found == separators_found_second) break;
        if (target->data[i] == separator) {
            separators_indexes[separators_found_second] = i;
            separators_found_second++;
        }
    }

    String_Array result;
    // If no separators found, we just put the target string
    // into result string array.
    if (separators_found < 1) {
        result = new_string_array(1);
        add_string(&result, target);
        free(separators_indexes);
        return result;
    }

    result = new_string_array(separators_found+1);

    // First iteration.
    int start = 0;
    int end = separators_indexes[0];
    string chunk = new_string(target, start, end);
    add_string(&result, chunk);

    int i = 0;
    for (; i < separators_found-1; i++) {
        start = separators_indexes[i]+1;
        end = separators_indexes[i+1];
        chunk = new_string(target, start, end);
        add_string(&result, chunk);
    }

    // Last iteration.
    start = separators_indexes[i]+1;
    end = target->size;
    chunk = new_string(target, start, end);
    add_string(&result, chunk);

    free(separators_indexes);
    return result;
}

void swap_strings(String_Array *array, int first_index, int second_index) {
    if (array->size < 2) return;
    if (first_index < array->size && first_index > array->size) return;
    if (second_index < array->size && second_index > array->size) return;

    string temp = array->data[first_index];
    array->data[first_index] = array->data[second_index];
    array->data[second_index] = temp;
}

// Currently, it works only for strings that were
// initialized with heap data (not const char pointer).
// String struct probably needs flags variable, which
// contains information whether its pointer points
// to a const data or not. 
void clear_string_array(String_Array *array) {
    if (array->size < 1) return;

    for (int i = 0; i < array->size; i++) {
        free(array->data[i].data);
    }

    array->size = 0;
}

/*
bool string_contains(string target, string chunk) {
    if (chunk.size > target.size) return false;

    for (int i = 0, j = 0; i < target.size-1; i++) {
        if (target.data[i] == chunk.data[j]) j++;
        else j = 0;

        if (j+1 == chunk.size) return true;
    }

    return false;
}
*/

string new_string(int size) {
    string result;
    if (size < 1) return result;

    result.data = (char *) malloc(size * sizeof(char));
    result.size = size;

    return result;
}

// @Incomplete
string join_strings(string first, string second) {
    string result = new_string(first.size + second.size - 1); // -1 for '\0'

    int i = 0;
    int j = 0;
    for (; i < first.size-1; i++)
        result.data[i] = first.data[i];

    if (first.size > 0) i = first.size-1;
    else i = 0;

    for (; j < second.size; i++, j++)
        result.data[i] = second.data[j];

    return result;
}

void init_char_array_from_end(char *array, int array_size, const char *data) {
    if (array_size < 1) return;

    int data_size = char_array_size(data);
    if (data_size < 1) return;

    for (int i = array_size-1, j = data_size-1; i >= 0; i--, j--)
        array[i] = data[j];
}

string remove_first_match(string *from, string chunk) {
    if (chunk.size > from->size) return *from;

    string result;
    for (int i = 0, cursor = 0, chunk_start = 0; i < from->size; i++) {
        if (from->data[i] == chunk.data[cursor]) {
            cursor++;
            if (chunk_start == 0) chunk_start = i;
        } else {
        cursor = 0;
        }

        if (cursor+1 == chunk.size) {
            char *lead = (char *) malloc(chunk_start * sizeof(char));
            init_char_array(lead, chunk_start, from->data);

            int tail_size = from->size - (chunk_start + cursor);
            char *tail = (char *) malloc(tail_size * sizeof(char));
            init_char_array_from_end(tail, tail_size, from->data);

            result = new_string(chunk_start + tail_size);

            for (int i = 0; i < tail_size; i++, cursor++)
                result.data[cursor] = tail[i];

            free(lead);
            free(tail);
            return result;
        }
    }

    return result;
}

// @Incomplete
string join_strings(String_Array *string_array) {
    string result;
    if (string_array->size < 1) return result;
    if (string_array->size == 1) return string_array->data[0];

    for (int i = 1; i < string_array->size; i++) {
        result = join_strings(result, string_array->data[i]);
    }

    return result;
}

/*
int main() {
    string str = new_string("Hello world");
    string joined = join_strings(str, new_string(", Goodbye sadness"));
    String_Array str_arr = new_string_array(5);
    add_string(&str_arr, str);
    add_string(&str_arr, joined);
    add_string(&str_arr, new_string("test string"));

    print(&str);
    print(&joined);
    print(&str_arr);

    string str_arr_joined = join_strings(&str_arr);
    print(&str_arr_joined);
}
*/

void string_test_initialization();
void string_test_equal();
void string_test_difference();
void string_test_lowercase_uppercase();
void string_test_difference_ignore_case();
void string_test_set_case();
void string_test_prefix_suffix();
void string_test_trim_leading_and_tailing_whitespaces();
void string_test_clone_and_reference();
void string_test_contains();
void string_test_string_array();
void string_test_split();
void string_test_clear_string_array();
void string_test_join_strings();

void string_do_tests() {
    string_test_initialization();
    string_test_equal();
    string_test_difference();
    string_test_lowercase_uppercase();
    string_test_difference_ignore_case();
    string_test_set_case();
    string_test_prefix_suffix();
    string_test_trim_leading_and_tailing_whitespaces();
    string_test_clone_and_reference();
    string_test_contains();
    string_test_string_array();
    string_test_split();
    string_test_clear_string_array();
    string_test_join_strings();
}

/* --- INITIALIZATION --- */
void string_test_initialization() {
    printf("[String initialization]\n");
    string string_from_const_data_no_size = new_string("Hello");
    string string_from_const_data_with_size = new_string("world");
    string string_from_string_const_data_no_size = new_string(string_from_const_data_no_size);
    string string_from_string_const_data_with_size = new_string(string_from_const_data_with_size);

    // print_string() -> print()
    print(&string_from_const_data_no_size);
    print(&string_from_const_data_with_size);
    print(&string_from_string_const_data_no_size);
    print(&string_from_string_const_data_with_size);

    printf("\n");

    char *s1_data = (char *) malloc(8 * sizeof(char));
    char *s2_data = (char *) malloc(10 * sizeof(char));
    fill_char_array(s1_data, "Goodbye", 0, 8);
    fill_char_array(s2_data, "emptiness", 0, 10);

    string string_from_data_no_size = new_string(s1_data);
    string string_from_data_with_size = new_string(s2_data);
    string string_from_string_data_no_size = new_string(string_from_data_no_size);
    string string_from_string_data_with_size = new_string(string_from_data_with_size);

    // print_string() -> print()
    print(&string_from_data_no_size);
    print(&string_from_data_with_size);
    print(&string_from_string_data_no_size);
    print(&string_from_string_data_with_size);

    printf("\n");

    string *heap_string_from_const_data_no_size = allocate_string("Hello");
    string *heap_string_from_const_data_with_size = allocate_string("world");
    string *heap_string_from_string_const_data_no_size = allocate_string(&string_from_const_data_no_size);
    string *heap_string_from_string_const_data_with_size = allocate_string(&string_from_const_data_with_size);

    // print_string() -> print()
    print(heap_string_from_const_data_no_size);
    print(heap_string_from_const_data_with_size);
    print(heap_string_from_string_const_data_no_size);
    print(heap_string_from_string_const_data_with_size);

    printf("\n");

    char *heap_s1_data = (char *) malloc(8 * sizeof(char));
    char *heap_s2_data = (char *) malloc(10 * sizeof(char));
    fill_char_array(heap_s1_data, "Goodbye", 0 , 8);
    fill_char_array(heap_s2_data, "emptiness", 0, 10);

    string *heap_string_from_data_no_size = allocate_string(heap_s1_data);
    string *heap_string_from_data_with_size = allocate_string(heap_s2_data);
    string *heap_string_from_string_data_no_size = allocate_string(heap_string_from_data_no_size);
    string *heap_string_from_string_data_with_size = allocate_string(heap_string_from_data_with_size);

    // print_string() -> print()
    print(heap_string_from_data_no_size);
    print(heap_string_from_data_with_size);
    print(heap_string_from_string_data_no_size);
    print(heap_string_from_string_data_with_size);
}

 /* --- EQUAL --- */
 void string_test_equal() {
    printf("[String equal]\n");
    string s1 = new_string("helllo");
    string s2 = new_string("world");
    string s3 = new_string(s1);
    string *s4 = allocate_string(&s2);
    string *s5 = allocate_string("world");
    string *s6 = allocate_string("world\0");
    char *s7_data = (char *) malloc(6 * sizeof(char));
    fill_char_array(s7_data, "world");
    string *s7 = allocate_string(s7_data);
    string *s8 = allocate_string(s7_data);

    bool s1_s2_equal = strings_equal(&s1, &s2);
    bool s1_s3_equal = strings_equal(&s1, &s3);
    bool s2_s4_equal = strings_equal(&s2, s4);
    bool s2_s5_equal = strings_equal(&s2, s5);
    bool s2_s6_equal = strings_equal(&s2, s6);
    bool s2_s7_equal = strings_equal(&s2, s7);
    bool s2_s8_equal = strings_equal(&s2, s8);

    printf("s1_s2_equal: %d\n", (int) s1_s2_equal);
    printf("s1_s3_equal: %d\n", (int) s1_s3_equal);
    printf("s2_s4_equal: %d\n", (int) s2_s4_equal);
    printf("s2_s5_equal: %d\n", (int) s2_s5_equal);
    printf("s2_s6_equal: %d\n", (int) s2_s6_equal);
    printf("s2_s7_equal: %d\n", (int) s2_s7_equal);
    printf("s2_s8_equal: %d\n", (int) s2_s8_equal);
 }

 /* --- DIFFERENCE --- */
 void string_test_difference() {
    printf("[String difference]\n");
    string *text1 = allocate_string("ab c");
    string *text2 = allocate_string("abcd");
    string *text3 = allocate_string("abcdefg");
    string *text4 = allocate_string(text1);
    string *text5 = allocate_string("AB c");

    printf("text1 and text2 difference: %d\n", strings_difference(text1, text2));
    printf("text1 and text3 difference: %d\n", strings_difference(text1, text3));
    printf("text1 and text4 difference: %d\n", strings_difference(text1, text4));
    printf("text1 and text5 difference: %d\n", strings_difference(text1, text5));
 }

/* --- LOWERCASE/UPPERCASE --- */
void string_test_lowercase_uppercase() {
    printf("[String lowercase uppercase]\n");
    string text1 = new_string("Hello World");
    string text2 = new_string("Goodbye Emptiness");

    // print_string() -> print()
    print(&text1);
    print(&text2);

    printf("\n");

    string text1_lowercase_by_value = string_lowercase(text1);
    string text1_lowercase_by_pointer = string_lowercase(&text1);
    string text1_lowercase_by_set = new_string(&text1);
    string_to_lowercase(&text1_lowercase_by_set);

    string text2_uppercase_by_value = string_lowercase(text2);
    string text2_uppercase_by_pointer = string_lowercase(&text2);
    string text2_uppercase_by_set = new_string(&text2);
    string_to_uppercase(&text2_uppercase_by_set);

    // print_string() -> print()
    print(&text1);
    print(&text1_lowercase_by_value);
    print(&text1_lowercase_by_pointer);
    print(&text1_lowercase_by_set);

    printf("\n");

    // print_string() -> print()
    print(&text2);
    print(&text2_uppercase_by_value);
    print(&text2_uppercase_by_pointer);
    print(&text2_uppercase_by_set);
}

/* --- DIFFERENCE IGNORE CASE --- */
void string_test_difference_ignore_case() {
    printf("[String difference ignore case]\n");
    string text1 = new_string("Hello World");
    string text2 = new_string("Goodbye Emptiness");

    // print_string() -> print()
    print(&text1);
    print(&text2);

    printf("\n");

    // print_string() -> print()
    string text1_lowercase = string_lowercase(&text1);
    print(&text1);
    string text1_uppercase = string_uppercase(&text1);
    print(&text1);
    string text1_titlecase = new_string("Hello World");
    print(&text1);

    // print_string() -> print()
    string text2_lowercase = string_lowercase(&text2);
    print(&text2);
    string text2_uppercase = string_uppercase(&text2);
    print(&text2);
    string text2_titlecase = new_string("Goodbye Emptiness");
    print(&text2);

    int text1_lowercase_difference = strings_difference_ignore_case(text1, text1_lowercase);
    int text1_uppercase_difference = strings_difference_ignore_case(text1, text1_uppercase);
    int text1_titlecase_difference = strings_difference_ignore_case(text1, text1_titlecase);

    int text2_lowercase_difference = strings_difference_ignore_case(text2, text2_lowercase);
    int text2_uppercase_difference = strings_difference_ignore_case(text2, text2_uppercase);
    int text2_titlecase_difference = strings_difference_ignore_case(text2, text2_titlecase);

    // print_string() -> print()
    print(&text1);
    printf("Lowercase difference: %d\n", text1_lowercase_difference);
    printf("Uppercase difference: %d\n", text1_uppercase_difference);
    printf("Titlecase difference: %d\n", text1_titlecase_difference);

    printf("\n");

    // print_string() -> print()
    print(&text2);
    printf("Lowercase difference: %d\n", text2_lowercase_difference);
    printf("Uppercase difference: %d\n", text2_uppercase_difference);
    printf("Titlecase difference: %d\n", text2_titlecase_difference);
}

/* --- CASES --- */
void string_test_set_case() {
    printf("[String set case]\n");
    string text = new_string("This is a test message.");
    // PRINT_STRING_POINTERS() -> print()
    print(&text);

    printf("\n");

    string text_return_uppercase = string_uppercase(&text);
    string text_return_lowercase = string_lowercase(&text);
    string text_return_titlecase = string_titlecase(&text);

    string text_set_uppercase = new_string(&text);
    string_to_uppercase(&text_set_uppercase);
    string text_set_lowercase = new_string(&text);
    string_to_lowercase(&text_set_lowercase);
    string text_set_titlecase = new_string(&text);
    string_to_titlecase(&text_set_titlecase);

    // PRINT_STRING_POINTERS() -> print()
    print(&text_return_uppercase);
    print(&text_return_lowercase);
    print(&text_return_titlecase);

    printf("\n");

    // PRINT_STRING_POINTERS() -> print()
    print(&text_set_uppercase);
    print(&text_set_lowercase);
    print(&text_set_titlecase);
}

/* --- PREFIX / SUFFIX --- */
void string_test_prefix_suffix() {
    printf("[String prefix suffix]\n");
    string text = new_string("Hello World!");
    // print_string() -> print()
    print(&text);

    printf("\n");

    string prefix_stack = new_string("Hello");
    string *prefix_heap = allocate_string("Hello");
    printf("prefix_stack: %s\n", prefix_stack.data);
    printf("prefix_heap: %s\n", prefix_heap->data);

    bool prefix_value_value = string_starts_with(text, prefix_stack);
    // bool prefix_pointer_value = string_starts_with(&text, prefix_stack);
    bool prefix_value_pointer = string_starts_with(text, *prefix_heap);
    // bool prefix_pointer_pointer = string_starts_with(&text, prefix_heap);

    printf("\n");

    string suffix_stack = new_string("World!");
    string *suffix_heap = allocate_string("World!");
    printf("suffix_stack: %s\n", suffix_stack.data);
    printf("suffix_heap: %s\n", suffix_heap->data);

    bool suffix_value_value = string_ends_with(text, suffix_stack);
    // bool suffix_pointer_value = string_ends_with(&text, suffix_stack);
    bool suffix_value_pointer = string_ends_with(text, *suffix_heap);
    // bool suffix_pointer_pointer = string_ends_with(&text, suffix_heap);

    printf("\n");

    printf("prefix_value_value: %d\n", prefix_value_value);
    // printf("prefix_pointer_value: %d\n", prefix_pointer_value);
    printf("prefix_value_pointer: %d\n", prefix_value_pointer);
    // printf("prefix_pointer_pointer: %d\n", prefix_pointer_pointer);

    printf("\n");

    printf("suffix_value_value: %d\n", suffix_value_value);
    // printf("suffix_pointer_value: %d\n", suffix_pointer_value);
    printf("suffix_value_pointer: %d\n", suffix_value_pointer);
    // printf("suffix_pointer_pointer: %d\n", suffix_pointer_pointer);
}

/* --- TRIM LEADING / TAILING WHITESPACES --- */
void string_test_trim_leading_and_tailing_whitespaces() {
    printf("[String trim leading and tailing whitespaces\n");
    string text1 = new_string("     Hello World!");
    string text2 = new_string("Goodbye Emptiness!     ");
    string text3 = new_string("     Hello World! Goodbye Emptiness!     ");

    // PRINT_STRING() -> print()
    print(&text1);
    print(&text2);
    print(&text3);

    printf("\n");

    string text1_no_leading = trim_leading_whitespaces(text1);
    string text2_no_tailing = trim_tailing_whitespaces(text2);
    string text3_no_leading_tailing = trim_whitespaces(text3);

    // PRINT_STRING() -> print()
    print(&text1_no_leading);
    print(&text2_no_tailing);
    print(&text3_no_leading_tailing);
}

/* --- CLONE & REFERENCE --- */
void string_test_clone_and_reference() {
    printf("[String clone and reference]\n");
    string hello = new_string("Hello");
    string *world = allocate_string("World");
    // PRINT_STRING_POINTERS() -> print()
    print(&hello);
    print(world);

    printf("\n");

    string hello_new_value = new_string(hello);
    string hello_new_pointer = new_string(&hello);
    string *world_new_value = allocate_string(*world);  
    string *world_new_pointer = allocate_string(world);

    // PRINT_STRING_POINTERS() -> print()
    print(&hello_new_value);
    print(&hello_new_pointer);
    print(world_new_value);
    print(world_new_pointer);

    printf("\n");

    string hello_clone_value = clone_string(hello);
    string hello_clone_pointer = clone_string(hello);
    string *world_reference_value = reference_string(*world);
    string *world_reference_pointer = reference_string(*world);

    // PRINT_STRING_POINTERS() -> print()
    print(&hello_clone_value);
    print(&hello_clone_pointer);
    print(world_reference_value);
    print(world_reference_pointer);

    printf("\n");

    hello = new_string("New Hello");
    world = allocate_string("New World");
    // PRINT_STRING_POINTERS() -> print()
    print(&hello);
    print(world);

    printf("\n");

    // PRINT_STRING_POINTERS() -> print()
    print(&hello_clone_value);
    print(&hello_clone_pointer);
    print(world_reference_value);
    print(world_reference_pointer);
}

/* --- CONTAIN --- */
void string_test_contains() {
    printf("[String contains]\n");
    string text = new_string("aabbcc");

    string text1 = new_string("bbcc");
    string text2 = new_string("aabb");
    string text3 = new_string("aabbcc");
    string text4 = new_string("aacc");
    string text5 = new_string("aabbccd");
    string text6 = new_string("dd");

    // Results and what are they should be -> bool (int).
    bool contains1 = string_contains(text, text1); // true  (1)
    bool contains2 = string_contains(text, text2); // true  (1)
    bool contains3 = string_contains(text, text3); // true  (1)
    bool contains4 = string_contains(text, text4); // false (0)
    bool contains5 = string_contains(text, text5); // false (0)
    bool contains6 = string_contains(text, text6); // false (0)

    printf("contains1: %d\n", contains1);
    printf("contains2: %d\n", contains2);
    printf("contains3: %d\n", contains3);
    printf("contains4: %d\n", contains4);
    printf("contains5: %d\n", contains5);
    printf("contains6: %d\n", contains6);
}

/* --- STRING ARRAY --- */
void string_test_string_array() {
    printf("[String string array]\n");
    String_Array empty_string_array = new_string_array(5);
    string *not_struct_array = (string *) malloc(3 * sizeof(string));
    not_struct_array[0] = new_string("hello");
    not_struct_array[1] = new_string("world");
    not_struct_array[2] = new_string("message");
    String_Array string_array = new_string_array(not_struct_array, 3);
    // print_string_array() -> print()
    print(&string_array);
    print(&empty_string_array);

    // print_string_array() -> print()
    add_string(&string_array, allocate_string("heap string"));
    print(&string_array);
    add_string(&string_array, new_string("stack string"));
    print(&string_array);
    add_string(&string_array, new_string("test"));
    print(&string_array);
    add_string(&string_array, new_string("ONE MORE MESSAGE"));
    print(&string_array);

    pop_string(&string_array);
    // print_string_array() -> print()
    print(&string_array);

    // print_string_array() -> print()
    add_strings(&empty_string_array, not_struct_array, 3);
    print(&empty_string_array);
    add_strings(&empty_string_array, not_struct_array, 3);
    print(&empty_string_array);
    pop_strings(&empty_string_array, 7); 
    print(&empty_string_array);
}

/* --- SPLIT STRING --- */
void string_test_split() {
    printf("[String split]\n");
    string str_with_separators = new_string("This is a test message.");
    // print_string_info() -> print()
    print(&str_with_separators);
    String_Array multi_split = split_string(&str_with_separators);
    // print_string_array() -> print()n
    print(&multi_split);

    printf("\n");

    string str_no_separators = new_string("ThisIsATestMessage.");
    // print_string_info() -> print()
    print(&str_no_separators);
    String_Array single_split = split_string(&str_no_separators);
    // print_string_array() -> print()
    print(&single_split);

    printf("\n");

    string str_commas = new_string("This, is, a, test, message.");
    // print_string_info() -> print()
    print(&str_commas);
    String_Array comma_split = split_string(&str_commas, ',');
    // print_string_array() -> print()
    print(&comma_split);

    printf("\n");

    string str_leading_tailing = new_string("--This-Is-A-Test-Message.--");
    // print_string_info() -> print()
    print(&str_leading_tailing);
    String_Array leading_tailing_split = split_string(&str_leading_tailing, '-');
    // print_string_array() -> print()
    print(&leading_tailing_split);

    printf("\n");

    string str_separator = new_string(" ");
    // print_string_inf() -> print()
    print(&str_separator);
    String_Array separator_split = split_string(&str_separator);
    // print_string_array() -> print()
    print(&separator_split);
}

/* --- CLEAR STRING ARRAY --- */
void string_test_clear_string_array() {
    printf("[String clear string array]\n");
    String_Array arr1 = new_string_array(1);
    add_string(&arr1, new_string("Stack string, Const char"));
    print(&arr1);

    String_Array arr2 = new_string_array(1);
    add_string(&arr2, allocate_string("Heap string, Const char"));
    print(&arr2);

    char *str_data1 = (char *) malloc(24 * sizeof(char));
    init_char_array(str_data1, 24, "Stack string, Heap char");  
    printf("str_data1: %s\n", str_data1);

    String_Array arr3 = new_string_array(1);
    add_string(&arr3, new_string(str_data1));
    print(&arr3);

    char *str_data2 = (char *) malloc(23 * sizeof(char));
    init_char_array(str_data2, 23, "Heap string, Heap char");
    printf("str_data2: %s\n", str_data2);  

    String_Array arr4 = new_string_array(1);
    add_string(&arr4, allocate_string(str_data2));
    print(&arr4);

    printf("\n");

    // fail
    // clear_string_array(&arr1);
    print(&arr1);
    // fail
    // clear_string_array(&arr2);
    print(&arr2);
    clear_string_array(&arr3);
    print(&arr3);
    clear_string_array(&arr4);
    print(&arr4);
}

/* --- JOINS STRINGS --- */
void string_test_join_strings() {
    printf("[String join strings]\n");
    string test1 = new_string("This is a");
    string test2 = new_string(" test message.");
    string join_test = join_strings(test1, test2);
    print(&join_test);

    string lead_empty1 = new_string("");
    string lead_empty2 = new_string("Leading is empty.");
    string join_lead_empty = join_strings(lead_empty1, lead_empty2);
    print(&join_lead_empty);

    string tail_empty1 = new_string("Tailing is empty.");
    string tail_empty2 = new_string("");
    string join_tail_empty = join_strings(tail_empty1, tail_empty2);
    print(&join_tail_empty);
}