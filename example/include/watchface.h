#ifndef WATCHFACE_H_
#define WATCHFACE_H_

#include "ugui/ugui.h"
#include <stdint.h>

ugui_window_t* watchface_create(uint32_t w, uint32_t h);
void watchface_destroy();

#endif // WATCHFACE_H_
