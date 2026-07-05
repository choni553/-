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

static void destroy_all(void){
    ui_Splash_screen_destroy();ui_Login_screen_destroy();ui_Register_screen_destroy();
    ui_MainMenu_screen_destroy();ui_Game_screen_destroy();
    ui_Album_screen_destroy();ui_Music_screen_destroy();ui_Video_screen_destroy();
    ui_Recognition_screen_destroy();ui_Controls_screen_destroy();}

/* 呼吸切页：遮罩从小变大再消失，100ms */
static void breath_x(void *var, int32_t v){lv_obj_set_width ((lv_obj_t *)var, v);}
static void breath_y(void *var, int32_t v){lv_obj_set_height((lv_obj_t *)var, v);}
static void breath_done(lv_anim_t *a){lv_obj_del((lv_obj_t *)a->var);}
#define LOAD(scr,init) do{\
    destroy_all();init();lv_scr_load(scr);lv_obj_invalidate(scr);\
    lv_refr_now(lv_disp_get_default());\
    lv_obj_t *_b = lv_obj_create(scr);\
    lv_obj_set_style_bg_color(_b, lv_color_hex(0xfce4ec), 0);\
    lv_obj_set_style_bg_opa(_b, LV_OPA_COVER, 0);\
    lv_obj_set_style_border_width(_b, 0, 0);\
    lv_obj_set_style_radius(_b, 0, 0);\
    lv_obj_set_size(_b, 0, 0);lv_obj_center(_b);\
    lv_anim_t _bx;lv_anim_init(&_bx);lv_anim_set_var(&_bx, _b);\
    lv_anim_set_values(&_bx, 0, 1024);lv_anim_set_time(&_bx, 100);\
    lv_anim_set_exec_cb(&_bx, (lv_anim_exec_xcb_t)breath_x);lv_anim_start(&_bx);\
    lv_anim_t _by;lv_anim_init(&_by);lv_anim_set_var(&_by, _b);\
    lv_anim_set_values(&_by, 0, 600);lv_anim_set_time(&_by, 100);\
    lv_anim_set_exec_cb(&_by, (lv_anim_exec_xcb_t)breath_y);\
    lv_anim_set_ready_cb(&_by, breath_done);lv_anim_start(&_by);\
}while(0)

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
void ui_destroy(void){destroy_all();}
