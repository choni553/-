#ifndef _SQUARELINE_PROJECT_UI_H
#define _SQUARELINE_PROJECT_UI_H
#ifdef __cplusplus
extern "C" {
#endif
#include "lvgl/lvgl.h"
#include "img_logo.h"
#include "img_bg.h"
#include "splash_anim.h"
#include "gif_anim.h"
#include "bg_login.h"
extern lv_obj_t *ui_Splash,*ui_Login,*ui_Register,*ui_MainMenu,*ui_Game,*ui_Album,*ui_Music,*ui_Video,*ui_Recognition,*ui_Controls,*ui_Snake,*ui_2048;
void ui_Splash_screen_init(void);void ui_Splash_screen_destroy(void);
void ui_Login_screen_init(void);void ui_Login_screen_destroy(void);
void ui_Register_screen_init(void);void ui_Register_screen_destroy(void);
void ui_MainMenu_screen_init(void);void ui_MainMenu_screen_destroy(void);
void ui_Game_screen_init(void);void ui_Game_screen_destroy(void);
void ui_Album_screen_init(void);void ui_Album_screen_destroy(void);
void ui_Music_screen_init(void);void ui_Music_screen_destroy(void);
void ui_Video_screen_init(void);void ui_Video_screen_destroy(void);
void ui_Recognition_screen_init(void);void ui_Recognition_screen_destroy(void);
void ui_Controls_screen_init(void);void ui_Controls_screen_destroy(void);
void ui_Snake_screen_init(void);void ui_Snake_screen_destroy(void);
void ui_2048_screen_init(void);void ui_2048_screen_destroy(void);
void ui_load_login_screen(void);void ui_load_register_screen(void);void ui_load_main_menu(void);
void ui_load_game(void);void ui_load_album(void);void ui_load_music(void);
void ui_load_video(void);void ui_load_recognition(void);void ui_load_controls(void);
void ui_init(void);void ui_destroy(void);
#ifdef __cplusplus
}
#endif
#endif
