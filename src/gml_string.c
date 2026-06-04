#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "gml_string.h"
#include "utils.h"

GMLString* GMLString_create(const char* str) {
    if (!str) return NULL;

    size_t len = strlen(str);
    if (len > INT32_MAX) return NULL; 

    GMLString* gmlstr = safeMalloc(sizeof(GMLString));
    gmlstr->data = safeMalloc(len + 1);

    memcpy(gmlstr->data, str, len + 1);

    gmlstr->refCount = 1;
    gmlstr->length = (int32_t)len;

    return gmlstr;
}

void GMLString_incRef(GMLString* str) {
    if (str) str->refCount++;
}

void GMLString_decRef(GMLString* str) {
    if (str && --str->refCount == 0) {
        free(str->data);
        free(str);
    }
}

GMLString* GMLString_clone(GMLString* src) {
    if (!src) return NULL;

    GMLString* dst = safeMalloc(sizeof(GMLString));

    size_t allocSize = (size_t)src->length + 1;
    dst->data = safeMalloc(allocSize);

    memcpy(dst->data, src->data, allocSize);
    dst->refCount = 1;
    dst->length = src->length;
    return dst;
}
