#pragma once
#include <stdint.h>
#include <stddef.h>
#include "common.h"

// ===[ GMLString - Refcounted string ]===

typedef struct {
    int32_t refCount;
    char* data;
    int32_t length;
} GMLString;

GMLString* GMLString_create(const char* str);
void GMLString_incRef(GMLString* str);
// Decrement refCount. If it reaches 0, free the string data and struct.
void GMLString_decRef(GMLString* str);
// Deep copy. Source string is strdup'd, refCount is set to 1.
GMLString* GMLString_clone(GMLString* src);
