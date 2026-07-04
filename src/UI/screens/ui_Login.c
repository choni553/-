#include "../ui.h"
#include "src/lib/auth.h"
#include <stdio.h>
#include <string.h>
lv_obj_t *ui_Login=NULL;
static lv_obj_t *ta_user,*ta_pwd,*login_btn,*err_label,*lock_label,*kb;
static lv_obj_t *cb_remember,*cb_auto;static int err_count=0,lock_sec=0;static lv_timer_t *lock_timer=NULL;
static void upd(void){const char *u=lv_textarea_get_text(ta_user),*p=lv_textarea_get_text(ta_pwd);if(u&&p&&strlen(u)&&strlen(p)&&lock_sec==0)lv_obj_clear_state(login_btn,LV_STATE_DISABLED);else lv_obj_add_state(login_btn,LV_STATE_DISABLED);}
static void lock_cb(lv_timer_t *t){(void)t;if(lock_sec>0){lock_sec--;char b[32];snprintf(b,sizeof(b),"Locked %ds",lock_sec);lv_label_set_text(lock_label,b);if(lock_sec==0){lv_label_set_text(lock_label,"");lv_timer_del(lock_timer);lock_timer=NULL;upd();}}}
static void do_login(void){const char *u=lv_textarea_get_text(ta_user),*p=lv_textarea_get_text(ta_pwd);if(!u||!p||!strlen(u)||!strlen(p))return;if(auth_login(u,p)==0){err_count=0;lv_label_set_text(err_label,"");if(lv_obj_has_state(cb_remember,LV_STATE_CHECKED))auth_save_remembered(u,p);else auth_clear_remembered();ui_load_main_menu();}else{err_count++;if(err_count>=3){lock_sec=30;lv_obj_add_state(login_btn,LV_STATE_DISABLED);if(!lock_timer)lock_timer=lv_timer_create(lock_cb,1000,NULL);lv_label_set_text(err_label,"Locked 30s");}else{lv_label_set_text(err_label,"Wrong username or password");}}}
static void login_cb(lv_event_t *e){(void)e;do_login();}
static void reg_cb(lv_event_t *e){(void)e;ui_load_register_screen();}
static void forgot_cb(lv_event_t *e){(void)e;lv_label_set_text(err_label,"Not implemented");}
static void eye_cb(lv_event_t *e){(void)e;static int s=0;s=!s;lv_textarea_set_password_mode(ta_pwd,!s);}
static void ta_cb(lv_event_t *e){(void)e;upd();}
static void ta_focus(lv_event_t *e){(void)e;lv_keyboard_set_textarea(kb,lv_event_get_target(e));lv_obj_clear_flag(kb,LV_OBJ_FLAG_HIDDEN);}
static void kb_hide(lv_event_t *e){(void)e;if(lv_event_get_target(e)==ui_Login)lv_obj_add_flag(kb,LV_OBJ_FLAG_HIDDEN);}

/* ---- Press animation helper ---- */
static void btn_press_cb(lv_event_t *e){lv_obj_t *b=lv_event_get_target(e);lv_anim_t a;lv_anim_init(&a);lv_anim_set_var(&a,b);lv_anim_set_values(&a,256,240);lv_anim_set_time(&a,80);lv_anim_set_exec_cb(&a,(lv_anim_exec_xcb_t)lv_image_set_scale_x);lv_anim_start(&a);lv_anim_set_values(&a,240,256);lv_anim_set_time(&a,80);lv_anim_set_ready_cb(&a,NULL);/* no ready cb, one-shot press */}

void ui_Login_screen_init(void){
    auth_init();
    ui_Login=lv_obj_create(NULL);lv_obj_clear_flag(ui_Login,LV_OBJ_FLAG_SCROLLABLE);lv_obj_set_style_bg_color(ui_Login,lv_color_hex(0xfce4ec),0);lv_obj_set_style_bg_opa(ui_Login,LV_OPA_COVER,0);lv_obj_set_style_pad_all(ui_Login,0,0);
    /* Logo */
    lv_obj_t *logo=lv_label_create(ui_Login);lv_label_set_text(logo,"Face Recognition System");lv_obj_set_style_text_color(logo,lv_color_hex(0x1A237E),0);lv_obj_set_style_text_font(logo,&lv_font_montserrat_24,0);lv_obj_align(logo,LV_ALIGN_TOP_MID,0,16);
    /* Card (bigger: 560x340) */
    lv_obj_t *card=lv_obj_create(ui_Login);lv_obj_set_size(card,560,380);lv_obj_align(card,LV_ALIGN_TOP_MID,0,64);lv_obj_set_style_bg_color(card,lv_color_white(),0);lv_obj_set_style_radius(card,18,0);lv_obj_set_style_border_width(card,0,0);lv_obj_set_style_shadow_width(card,24,0);lv_obj_clear_flag(card,LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *ct=lv_label_create(card);lv_label_set_text(ct,"Login");lv_obj_set_style_text_font(ct,&lv_font_montserrat_18,0);lv_obj_align(ct,LV_ALIGN_TOP_LEFT,50,16);
    /* Username */
    lv_obj_t *ul=lv_label_create(card);lv_label_set_text(ul,"Username");lv_obj_set_style_text_color(ul,lv_color_hex(0x78909C),0);lv_obj_set_style_text_font(ul,&lv_font_montserrat_12,0);lv_obj_align(ul,LV_ALIGN_TOP_LEFT,50,54);
    ta_user=lv_textarea_create(card);lv_obj_set_size(ta_user,440,40);lv_obj_align(ta_user,LV_ALIGN_TOP_MID,0,76);lv_textarea_set_one_line(ta_user,true);lv_obj_set_style_radius(ta_user,8,0);lv_obj_set_style_border_color(ta_user,lv_color_hex(0xCCD),0);lv_obj_set_style_border_color(ta_user,lv_color_hex(0x1A237E),LV_STATE_FOCUSED);lv_obj_add_event_cb(ta_user,ta_cb,LV_EVENT_VALUE_CHANGED,NULL);lv_obj_add_event_cb(ta_user,ta_focus,LV_EVENT_FOCUSED,NULL);
    /* Password */
    lv_obj_t *pl=lv_label_create(card);lv_label_set_text(pl,"Password");lv_obj_set_style_text_color(pl,lv_color_hex(0x78909C),0);lv_obj_set_style_text_font(pl,&lv_font_montserrat_12,0);lv_obj_align(pl,LV_ALIGN_TOP_LEFT,50,128);
    ta_pwd=lv_textarea_create(card);lv_obj_set_size(ta_pwd,440,40);lv_obj_align(ta_pwd,LV_ALIGN_TOP_MID,0,150);lv_textarea_set_one_line(ta_pwd,true);lv_textarea_set_password_mode(ta_pwd,true);lv_obj_set_style_radius(ta_pwd,8,0);lv_obj_set_style_border_color(ta_pwd,lv_color_hex(0xCCD),0);lv_obj_set_style_border_color(ta_pwd,lv_color_hex(0x1A237E),LV_STATE_FOCUSED);lv_obj_add_event_cb(ta_pwd,ta_cb,LV_EVENT_VALUE_CHANGED,NULL);lv_obj_add_event_cb(ta_pwd,ta_focus,LV_EVENT_FOCUSED,NULL);
    /* Eye */
    lv_obj_t *eye=lv_btn_create(card);lv_obj_set_size(eye,40,40);lv_obj_align(eye,LV_ALIGN_TOP_RIGHT,-12,148);lv_obj_set_style_bg_opa(eye,LV_OPA_TRANSP,0);lv_obj_set_style_shadow_width(eye,0,0);lv_obj_add_event_cb(eye,eye_cb,LV_EVENT_CLICKED,NULL);lv_obj_t *el=lv_label_create(eye);lv_label_set_text(el,LV_SYMBOL_EYE_OPEN);lv_obj_center(el);
    /* Checkboxes */
    cb_remember=lv_checkbox_create(card);lv_checkbox_set_text(cb_remember,"Remember Me");lv_obj_align(cb_remember,LV_ALIGN_TOP_LEFT,50,206);lv_obj_set_style_transform_pivot_x(cb_remember,0,0);
    cb_auto=lv_checkbox_create(card);lv_checkbox_set_text(cb_auto,"Auto Login");lv_obj_align(cb_auto,LV_ALIGN_TOP_LEFT,240,206);
    /* Buttons — 48px apart from text */
    login_btn=lv_btn_create(card);lv_obj_set_size(login_btn,210,46);lv_obj_align(login_btn,LV_ALIGN_BOTTOM_LEFT,50,-38);lv_obj_set_style_bg_color(login_btn,lv_color_hex(0x1A237E),0);lv_obj_set_style_radius(login_btn,10,0);lv_obj_set_style_bg_color(login_btn,lv_color_hex(0x283593),LV_STATE_PRESSED);lv_obj_add_state(login_btn,LV_STATE_DISABLED);lv_obj_add_event_cb(login_btn,login_cb,LV_EVENT_CLICKED,NULL);lv_obj_t *lbl=lv_label_create(login_btn);lv_label_set_text(lbl,"LOGIN");lv_obj_set_style_text_color(lbl,lv_color_white(),0);lv_obj_set_style_text_font(lbl,&lv_font_montserrat_16,0);lv_obj_center(lbl);
    lv_obj_t *reg_btn=lv_btn_create(card);lv_obj_set_size(reg_btn,210,46);lv_obj_align(reg_btn,LV_ALIGN_BOTTOM_RIGHT,-50,-38);lv_obj_set_style_bg_color(reg_btn,lv_color_hex(0xECEFF1),0);lv_obj_set_style_radius(reg_btn,10,0);lv_obj_set_style_bg_color(reg_btn,lv_color_hex(0xCFD8DC),LV_STATE_PRESSED);lv_obj_add_event_cb(reg_btn,reg_cb,LV_EVENT_CLICKED,NULL);lv_obj_t *rbl=lv_label_create(reg_btn);lv_label_set_text(rbl,"Register >");lv_obj_set_style_text_color(rbl,lv_color_hex(0x555),0);lv_obj_set_style_text_font(rbl,&lv_font_montserrat_16,0);lv_obj_center(rbl);
    /* Error labels */
    err_label=lv_label_create(ui_Login);lv_label_set_text(err_label,"");lv_obj_set_style_text_color(err_label,lv_color_hex(0xE53935),0);lv_obj_align(err_label,LV_ALIGN_TOP_MID,0,412);
    lock_label=lv_label_create(ui_Login);lv_label_set_text(lock_label,"");lv_obj_set_style_text_color(lock_label,lv_color_hex(0xE53935),0);lv_obj_align(lock_label,LV_ALIGN_TOP_MID,0,432);
    /* Forgot password */
    lv_obj_t *fg=lv_label_create(ui_Login);lv_label_set_text(fg,"Forgot password?");lv_obj_set_style_text_color(fg,lv_color_hex(0x999),0);lv_obj_set_style_text_font(fg,&lv_font_montserrat_12,0);lv_obj_align(fg,LV_ALIGN_TOP_MID,0,454);lv_obj_add_flag(fg,LV_OBJ_FLAG_CLICKABLE);lv_obj_add_event_cb(fg,forgot_cb,LV_EVENT_CLICKED,NULL);
    /* Keyboard */
    kb=lv_keyboard_create(ui_Login);lv_obj_set_size(kb,1024,220);lv_obj_align(kb,LV_ALIGN_BOTTOM_MID,0,0);lv_obj_add_flag(kb,LV_OBJ_FLAG_HIDDEN);lv_obj_add_event_cb(ui_Login,kb_hide,LV_EVENT_CLICKED,NULL);
    /* Auto-fill */
    char ru[40]={0},rp[40]={0};if(auth_get_remembered(ru,rp,40)==0){lv_textarea_set_text(ta_user,ru);lv_textarea_set_text(ta_pwd,rp);lv_obj_add_state(cb_remember,LV_STATE_CHECKED);upd();}
    err_count=0;lock_sec=0;
}
void ui_Login_screen_destroy(void){if(lock_timer){lv_timer_del(lock_timer);lock_timer=NULL;}if(ui_Login){lv_obj_del(ui_Login);ui_Login=NULL;}}
