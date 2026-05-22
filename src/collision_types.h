#pragma once
#include "real_type.h" // Where GMLReal is defined

typedef struct {
    GMLReal left, right, top, bottom;
    bool valid;
} InstanceBBox;
