#ifndef UI_SPLASH_H
#define UI_SPLASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"

extern lv_obj_t * ui_Splash;
extern void ui_Splash_screen_init(void);
extern void ui_Splash_screen_destroy(void);

#ifdef __cplusplus
}
#endif

#endif
