#pragma once

#include "common.h"

// CONFIG: Change the size of a pixel.
//
// 32-bit: 0xAARRGGBB
// 16-bit: 0b0RRRRRGGGGGBBBBB
// 8-bit:  0bBBGGGRRR
//#define PIXEL_SIZE 32
#define PIXEL_SIZE 16
//#define PIXEL_SIZE 8

#if defined(__GNUC__) && (__GNUC__ >= 3 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 96))
#define LIKELY(cond)   __builtin_expect(!!(cond), 1)
#define UNLIKELY(cond) __builtin_expect(!!(cond), 0)
#else
#define LIKELY(cond) (cond)
#define UNLIKELY(cond) (cond)
#endif

#define UNUSED __attribute__ ((unused))
#define FORCE_INLINE static inline __attribute__((always_inline))

