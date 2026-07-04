#ifndef UI_GAME_H
#define UI_GAME_H
#ifdef __cplusplus
extern "C" {
#endif
#include "lvgl/lvgl.h"
extern lv_obj_t * ui_Game;
extern void ui_Game_screen_init(void);
extern void ui_Game_screen_destroy(void);
#ifdef __cplusplus
}
#endif
#endif
