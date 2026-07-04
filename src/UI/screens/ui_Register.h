#ifndef UI_REGISTER_H
#define UI_REGISTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"

extern lv_obj_t * ui_Register;
extern void ui_Register_screen_init(void);
extern void ui_Register_screen_destroy(void);

#ifdef __cplusplus
}
#endif

#endif
