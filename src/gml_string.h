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
// Decrement refCount. If it reaches 0, free all inner RValues + row buffers + struct. Safe on nullptr.
void GMLString_decRef(GMLString* str);
// Deep copy. Every inner owned-string is strdup'd. Nested arrays have their refCount bumped (shared by default).
// New array starts at refCount=1, same shape as src, owner=newOwner.
GMLString* GMLString_clone(GMLString* src);
