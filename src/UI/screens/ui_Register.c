#include "../ui.h"
#include "src/lib/auth.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
lv_obj_t *ui_Register=NULL;static lv_obj_t *ta_user,*ta_pwd,*ta_confirm,*reg_btn,*strength_lbl,*msg_lbl,*kb;
static int chk(const char *p){int len=strlen(p),d=0,a=0,s=0;for(int i=0;p[i];i++){if(isdigit(p[i]))d=1;else if(isalpha(p[i]))a=1;else s=1;}if(len<6)return 1;if(d&&a&&s)return 3;if(d&&a)return 2;return 1;}
static void upd(void){const char *u=lv_textarea_get_text(ta_user),*p=lv_textarea_get_text(ta_pwd),*c=lv_textarea_get_text(ta_confirm);if(u&&p&&c&&strlen(u)&&strlen(p)&&strlen(c))lv_obj_clear_state(reg_btn,LV_STATE_DISABLED);else lv_obj_add_state(reg_btn,LV_STATE_DISABLED);}
static void ta_cb(lv_event_t *e){(void)e;upd();const char *p=lv_textarea_get_text(ta_pwd);int s=chk(p);if(!strlen(p))lv_label_set_text(strength_lbl,"");else if(s==1){lv_label_set_text(strength_lbl,"Weak - add numbers/symbols");lv_obj_set_style_text_color(strength_lbl,lv_color_hex(0xE53935),0);}else if(s==2){lv_label_set_text(strength_lbl,"Medium");lv_obj_set_style_text_color(strength_lbl,lv_color_hex(0xFB8C00),0);}else{lv_label_set_text(strength_lbl,"Strong");lv_obj_set_style_text_color(strength_lbl,lv_color_hex(0x43A047),0);}const char *c=lv_textarea_get_text(ta_confirm);if(strlen(c)&&strcmp(p,c))lv_label_set_text(msg_lbl,"Passwords do not match!");else lv_label_set_text(msg_lbl,"");}
static void do_reg(void){const char *u=lv_textarea_get_text(ta_user),*p=lv_textarea_get_text(ta_pwd),*c=lv_textarea_get_text(ta_confirm);if(strcmp(p,c)){lv_label_set_text(msg_lbl,"Passwords mismatch!");return;}if(strlen(u)<1||strlen(p)<1)return;int r=auth_register(u,p);if(r==-2){lv_label_set_text(msg_lbl,"Username exists!");lv_textarea_set_text(ta_user,"");return;}if(r==-3){lv_label_set_text(msg_lbl,"Max 50 users!");return;}if(r!=0){lv_label_set_text(msg_lbl,"Register failed!");return;}FILE *f=fopen("/root/ljr/data/last_reg.txt","w");if(f){fprintf(f,"%s",u);fclose(f);}ui_load_login_screen();}
static void reg_cb(lv_event_t *e){(void)e;do_reg();}
static void back_cb(lv_event_t *e){const char *u=lv_textarea_get_text(ta_user),*p=lv_textarea_get_text(ta_pwd),*c=lv_textarea_get_text(ta_confirm);if((u&&strlen(u))||(p&&strlen(p))||(c&&strlen(c))){lv_obj_t *m=lv_msgbox_create(NULL);lv_msgbox_add_title(m,"Abandon?");lv_msgbox_add_text(m,"Unsaved input");lv_msgbox_add_close_button(m);lv_obj_center(m);}else ui_load_login_screen();}
static void ta_focus(lv_event_t *e){(void)e;lv_keyboard_set_textarea(kb,lv_event_get_target(e));lv_obj_clear_flag(kb,LV_OBJ_FLAG_HIDDEN);}
static void kb_hide(lv_event_t *e){(void)e;if(lv_event_get_target(e)==ui_Register)lv_obj_add_flag(kb,LV_OBJ_FLAG_HIDDEN);}

void ui_Register_screen_init(void){
    ui_Register=lv_obj_create(NULL);lv_obj_clear_flag(ui_Register,LV_OBJ_FLAG_SCROLLABLE);lv_obj_set_style_bg_color(ui_Register,lv_color_hex(0xfce4ec),0);lv_obj_set_style_bg_opa(ui_Register,LV_OPA_COVER,0);lv_obj_set_style_pad_all(ui_Register,0,0);
    /* Back — top-left */
    lv_obj_t *back=lv_btn_create(ui_Register);lv_obj_set_size(back,60,32);lv_obj_set_pos(back,16,12);lv_obj_set_style_bg_color(back,lv_color_hex(0x546E7A),0);lv_obj_set_style_radius(back,6,0);lv_obj_set_style_bg_color(back,lv_color_hex(0x37474F),LV_STATE_PRESSED);lv_obj_add_event_cb(back,back_cb,LV_EVENT_CLICKED,NULL);lv_obj_t *bl=lv_label_create(back);lv_label_set_text(bl,LV_SYMBOL_LEFT" Back");lv_obj_set_style_text_color(bl,lv_color_white(),0);lv_obj_set_style_text_font(bl,&lv_font_montserrat_14,0);lv_obj_center(bl);
    /* Logo */
    lv_obj_t *logo=lv_label_create(ui_Register);lv_label_set_text(logo,"Create Account");lv_obj_set_style_text_color(logo,lv_color_hex(0x2E7D32),0);lv_obj_set_style_text_font(logo,&lv_font_montserrat_24,0);lv_obj_align(logo,LV_ALIGN_TOP_MID,0,16);
    /* Card (560x380) — no Back inside */
    lv_obj_t *card=lv_obj_create(ui_Register);lv_obj_set_size(card,560,420);lv_obj_align(card,LV_ALIGN_TOP_MID,0,64);lv_obj_set_style_bg_color(card,lv_color_white(),0);lv_obj_set_style_radius(card,18,0);lv_obj_set_style_border_width(card,0,0);lv_obj_set_style_shadow_width(card,24,0);lv_obj_clear_flag(card,LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *ct=lv_label_create(card);lv_label_set_text(ct,"Register");lv_obj_set_style_text_font(ct,&lv_font_montserrat_18,0);lv_obj_align(ct,LV_ALIGN_TOP_LEFT,50,16);
    /* Username */
    lv_obj_t *ul=lv_label_create(card);lv_label_set_text(ul,"Username");lv_obj_set_style_text_color(ul,lv_color_hex(0x78909C),0);lv_obj_set_style_text_font(ul,&lv_font_montserrat_12,0);lv_obj_align(ul,LV_ALIGN_TOP_LEFT,50,54);
    ta_user=lv_textarea_create(card);lv_obj_set_size(ta_user,440,40);lv_obj_align(ta_user,LV_ALIGN_TOP_MID,0,76);lv_textarea_set_one_line(ta_user,true);lv_obj_set_style_radius(ta_user,8,0);lv_obj_set_style_border_color(ta_user,lv_color_hex(0xCCD),0);lv_obj_set_style_border_color(ta_user,lv_color_hex(0x2E7D32),LV_STATE_FOCUSED);lv_obj_add_event_cb(ta_user,ta_cb,LV_EVENT_VALUE_CHANGED,NULL);lv_obj_add_event_cb(ta_user,ta_focus,LV_EVENT_FOCUSED,NULL);
    /* Password */
    lv_obj_t *pl=lv_label_create(card);lv_label_set_text(pl,"Password");lv_obj_set_style_text_color(pl,lv_color_hex(0x78909C),0);lv_obj_set_style_text_font(pl,&lv_font_montserrat_12,0);lv_obj_align(pl,LV_ALIGN_TOP_LEFT,50,128);
    ta_pwd=lv_textarea_create(card);lv_obj_set_size(ta_pwd,440,40);lv_obj_align(ta_pwd,LV_ALIGN_TOP_MID,0,150);lv_textarea_set_one_line(ta_pwd,true);lv_textarea_set_password_mode(ta_pwd,true);lv_obj_set_style_radius(ta_pwd,8,0);lv_obj_set_style_border_color(ta_pwd,lv_color_hex(0xCCD),0);lv_obj_set_style_border_color(ta_pwd,lv_color_hex(0x2E7D32),LV_STATE_FOCUSED);lv_obj_add_event_cb(ta_pwd,ta_cb,LV_EVENT_VALUE_CHANGED,NULL);lv_obj_add_event_cb(ta_pwd,ta_focus,LV_EVENT_FOCUSED,NULL);
    /* Confirm */
    lv_obj_t *cl=lv_label_create(card);lv_label_set_text(cl,"Confirm Password");lv_obj_set_style_text_color(cl,lv_color_hex(0x78909C),0);lv_obj_set_style_text_font(cl,&lv_font_montserrat_12,0);lv_obj_align(cl,LV_ALIGN_TOP_LEFT,50,202);
    ta_confirm=lv_textarea_create(card);lv_obj_set_size(ta_confirm,440,40);lv_obj_align(ta_confirm,LV_ALIGN_TOP_MID,0,224);lv_textarea_set_one_line(ta_confirm,true);lv_textarea_set_password_mode(ta_confirm,true);lv_obj_set_style_radius(ta_confirm,8,0);lv_obj_set_style_border_color(ta_confirm,lv_color_hex(0xCCD),0);lv_obj_set_style_border_color(ta_confirm,lv_color_hex(0x2E7D32),LV_STATE_FOCUSED);lv_obj_add_event_cb(ta_confirm,ta_cb,LV_EVENT_VALUE_CHANGED,NULL);lv_obj_add_event_cb(ta_confirm,ta_focus,LV_EVENT_FOCUSED,NULL);
    /* Strength */
    strength_lbl=lv_label_create(card);lv_label_set_text(strength_lbl,"");lv_obj_set_style_text_font(strength_lbl,&lv_font_montserrat_12,0);lv_obj_align(strength_lbl,LV_ALIGN_TOP_LEFT,50,274);
    /* Register button — CENTERED, no Back inside card */
    reg_btn=lv_btn_create(card);lv_obj_set_size(reg_btn,240,46);lv_obj_align(reg_btn,LV_ALIGN_BOTTOM_MID,0,-36);lv_obj_set_style_bg_color(reg_btn,lv_color_hex(0x2E7D32),0);lv_obj_set_style_radius(reg_btn,10,0);lv_obj_set_style_bg_color(reg_btn,lv_color_hex(0x388E3C),LV_STATE_PRESSED);lv_obj_add_state(reg_btn,LV_STATE_DISABLED);lv_obj_add_event_cb(reg_btn,reg_cb,LV_EVENT_CLICKED,NULL);lv_obj_t *rl=lv_label_create(reg_btn);lv_label_set_text(rl,"REGISTER");lv_obj_set_style_text_color(rl,lv_color_white(),0);lv_obj_set_style_text_font(rl,&lv_font_montserrat_16,0);lv_obj_center(rl);
    /* Message */
    msg_lbl=lv_label_create(ui_Register);lv_label_set_text(msg_lbl,"");lv_obj_set_style_text_color(msg_lbl,lv_color_hex(0xE53935),0);lv_obj_align(msg_lbl,LV_ALIGN_TOP_MID,0,452);
    /* Keyboard */
    kb=lv_keyboard_create(ui_Register);lv_obj_set_size(kb,1024,220);lv_obj_align(kb,LV_ALIGN_BOTTOM_MID,0,0);lv_obj_add_flag(kb,LV_OBJ_FLAG_HIDDEN);lv_obj_add_event_cb(ui_Register,kb_hide,LV_EVENT_CLICKED,NULL);
}
void ui_Register_screen_destroy(void){if(ui_Register){lv_obj_del(ui_Register);ui_Register=NULL;}}
