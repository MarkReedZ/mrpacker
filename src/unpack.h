#pragma once

#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#if defined(_MSC_VER)
#define inline __inline
#endif

// String interning functions
void init_string_interning(void);
void cleanup_string_interning(void);
