#ifndef UI_VIDEO_H
#define UI_VIDEO_H
#ifdef __cplusplus
extern "C" {
#endif
#include "lvgl/lvgl.h"
extern lv_obj_t * ui_Video;
extern void ui_Video_screen_init(void);
extern void ui_Video_screen_destroy(void);
#ifdef __cplusplus
}
#endif
#endif
