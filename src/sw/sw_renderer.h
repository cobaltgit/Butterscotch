#pragma once

#include "renderer.h"

Renderer* SWRenderer_create(void);

void SWRenderer_clearFrameBuffer(Renderer* renderer, uint32_t color);
