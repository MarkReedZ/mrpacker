#ifndef COMMON_STRINGS_H
#define COMMON_STRINGS_H

#include "py_defines.h"

// Maximum string length we'll consider for interning
#define MAX_STRING_LENGTH 16

// String arrays organized by length for O(1) access
static const char * const len1_strings[] = {"c","d"};
static const char * const len2_strings[] = {"tl","id"};
static const char * const len3_strings[] = {"txt", "num", "rtg", "key", "url", "api"};
static const char * const len4_strings[] = {"text", "type", "name", "data", "code", "user", "size"};
static const char * const len5_strings[] = {"value", "error", "token", "email", "title"};
static const char * const len6_strings[] = {"status", "result", "format", "length"};
static const char * const len7_strings[] = {"message", "session", "version", "created", "updated"};
static const char * const len8_strings[] = {"password", "username", "encoding"};
static const char * const len9_strings[] = {"timestamp"};
static const char * const len11_strings[] = {"description"};

// Global lookup tables
static const char * const *strings_by_len[MAX_STRING_LENGTH + 1] = {NULL};
static int strings_count[MAX_STRING_LENGTH + 1] = {0};

// Flattened array for index-based lookup
static const char **all_common_strings = NULL;
static int total_common_strings = 0;

// Initialize common strings lookup tables
static void init_common_strings_lookup(void) {
    static int initialized = 0;
    if (initialized) return;
    
    // Set up length-indexed lookup
    strings_by_len[1] = len1_strings; strings_count[1] = sizeof(len1_strings) / sizeof(len1_strings[0]);
    strings_by_len[2] = len2_strings; strings_count[2] = sizeof(len2_strings) / sizeof(len2_strings[0]);
    strings_by_len[3] = len3_strings; strings_count[3] = sizeof(len3_strings) / sizeof(len3_strings[0]);
    strings_by_len[4] = len4_strings; strings_count[4] = sizeof(len4_strings) / sizeof(len4_strings[0]);
    strings_by_len[5] = len5_strings; strings_count[5] = sizeof(len5_strings) / sizeof(len5_strings[0]);
    strings_by_len[6] = len6_strings; strings_count[6] = sizeof(len6_strings) / sizeof(len6_strings[0]);
    strings_by_len[7] = len7_strings; strings_count[7] = sizeof(len7_strings) / sizeof(len7_strings[0]);
    strings_by_len[8] = len8_strings; strings_count[8] = sizeof(len8_strings) / sizeof(len8_strings[0]);
    strings_by_len[9] = len9_strings; strings_count[9] = sizeof(len9_strings) / sizeof(len9_strings[0]);
    strings_by_len[11] = len11_strings; strings_count[11] = sizeof(len11_strings) / sizeof(len11_strings[0]);
    
    // Calculate total count
    total_common_strings = 0;
    for (int len = 0; len <= MAX_STRING_LENGTH; len++) {
        total_common_strings += strings_count[len];
    }
    
    // Create flattened array for index lookup
    all_common_strings = (const char **)malloc(total_common_strings * sizeof(const char *));
    int idx = 0;
    for (int len = 0; len <= MAX_STRING_LENGTH; len++) {
        if (strings_count[len] > 0) {
            for (int i = 0; i < strings_count[len]; i++) {
                all_common_strings[idx++] = strings_by_len[len][i];
            }
        }
    }
    
    initialized = 1;
}

// Find index of a common string, returns -1 if not found
static int find_common_string_index(const char *str, Py_ssize_t len) {
    if (len > MAX_STRING_LENGTH || strings_count[len] == 0) {
        return -1;
    }
    
    const char * const *strings = strings_by_len[len];
    int base_index = 0;
    
    // Calculate base index for this length
    for (int l = 0; l < len; l++) {
        base_index += strings_count[l];
    }
    
    // Look for string in this length group
    for (int i = 0; i < strings_count[len]; i++) {
        if (memcmp(str, strings[i], len) == 0) {
            return base_index + i;
        }
    }
    
    return -1; // Not found
}

#endif // COMMON_STRINGS_H 