#include "ui.h"
#include "ui_helpers.h"
lv_obj_t *ui____initial_actions0;lv_obj_t *ui_keyboard=NULL;
#if LV_COLOR_DEPTH != 32
    #error "LV_COLOR_DEPTH should be 32bit"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "LV_COLOR_16_SWAP should be 0"
#endif
void logo_anim_cb(void *var,int32_t v){lv_image_set_scale((lv_obj_t*)var,(uint32_t)v);}
static void destroy_others(void){lv_obj_t *act=lv_scr_act();
    if(ui_Splash&&ui_Splash!=act){lv_obj_t*a=lv_obj_get_child(ui_Splash,1);if(a)lv_obj_del(a);lv_obj_del(ui_Splash);ui_Splash=NULL;}
    if(ui_Login&&ui_Login!=act){lv_obj_del(ui_Login);ui_Login=NULL;}
    if(ui_Register&&ui_Register!=act){lv_obj_del(ui_Register);ui_Register=NULL;}
    if(ui_MainMenu&&ui_MainMenu!=act){lv_obj_del(ui_MainMenu);ui_MainMenu=NULL;}
    if(ui_Game&&ui_Game!=act){lv_obj_del(ui_Game);ui_Game=NULL;}
    if(ui_Album&&ui_Album!=act){lv_obj_del(ui_Album);ui_Album=NULL;}
    if(ui_Music&&ui_Music!=act){lv_obj_del(ui_Music);ui_Music=NULL;}
    if(ui_Video&&ui_Video!=act){lv_obj_del(ui_Video);ui_Video=NULL;}
    if(ui_Recognition&&ui_Recognition!=act){lv_obj_del(ui_Recognition);ui_Recognition=NULL;}
    if(ui_Controls&&ui_Controls!=act){lv_obj_del(ui_Controls);ui_Controls=NULL;}}
#define LOAD(scr,init) do{init();lv_scr_load(scr);lv_obj_invalidate(scr);destroy_others();}while(0)
void ui_load_login_screen(void){LOAD(ui_Login,ui_Login_screen_init);}
void ui_load_register_screen(void){LOAD(ui_Register,ui_Register_screen_init);}
void ui_load_main_menu(void){LOAD(ui_MainMenu,ui_MainMenu_screen_init);}
void ui_load_game(void){LOAD(ui_Game,ui_Game_screen_init);}
void ui_load_album(void){LOAD(ui_Album,ui_Album_screen_init);}
void ui_load_music(void){LOAD(ui_Music,ui_Music_screen_init);}
void ui_load_video(void){LOAD(ui_Video,ui_Video_screen_init);}
void ui_load_recognition(void){LOAD(ui_Recognition,ui_Recognition_screen_init);}
void ui_load_controls(void){LOAD(ui_Controls,ui_Controls_screen_init);}
void ui_init(void){lv_disp_t*d=lv_disp_get_default();lv_theme_t*th=lv_theme_default_init(d,lv_palette_main(LV_PALETTE_BLUE),lv_palette_main(LV_PALETTE_RED),false,LV_FONT_DEFAULT);lv_disp_set_theme(d,th);ui____initial_actions0=lv_obj_create(NULL);ui_Splash_screen_init();lv_scr_load(ui_Splash);}
void ui_destroy(void){destroy_others();}
