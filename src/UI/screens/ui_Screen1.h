#ifndef UI_SCREEN1_H
#define UI_SCREEN1_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"

extern void ui_Screen1_screen_init(void);
extern void ui_Screen1_screen_destroy(void);
extern lv_obj_t * ui_Screen1;
extern lv_obj_t * ui_username_ta;
extern lv_obj_t * ui_password_ta;
extern lv_obj_t * ui_login_btn;
extern lv_obj_t * ui_logo_img;
extern lv_obj_t * ui_status_label;

#ifdef __cplusplus
}
#endif

#endif
