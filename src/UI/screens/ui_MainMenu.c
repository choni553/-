#include "../ui.h"
#include <string.h>
lv_obj_t *ui_MainMenu=NULL;
static void btn_cb(lv_event_t *e){const char *l=(const char*)lv_event_get_user_data(e);
    if(strcmp(l,"Camera")==0)ui_load_video();else if(strcmp(l,"Game")==0)ui_load_game();
    else if(strcmp(l,"Controls")==0)ui_load_controls();else if(strcmp(l,"Album")==0)ui_load_album();
    else if(strcmp(l,"Recognition")==0)ui_load_recognition();else if(strcmp(l,"Logout")==0)ui_load_login_screen();}
static lv_obj_t *mk(lv_obj_t *p,const char *e,const char *t,const char *s,lv_coord_t x,lv_coord_t y,lv_color_t c){
    lv_obj_t *b=lv_btn_create(p);lv_obj_set_size(b,220,160);lv_obj_set_pos(b,x,y);lv_obj_set_style_bg_color(b,c,0);lv_obj_set_style_radius(b,16,0);lv_obj_set_style_bg_opa(b,LV_OPA_90,0);lv_obj_set_style_shadow_width(b,12,0);lv_obj_set_style_bg_opa(b,LV_OPA_COVER,LV_STATE_PRESSED);lv_obj_add_event_cb(b,btn_cb,LV_EVENT_CLICKED,(void*)t);
    lv_obj_t *ic=lv_label_create(b);lv_label_set_text(ic,e);lv_obj_set_style_text_font(ic,&lv_font_montserrat_36,0);lv_obj_align(ic,LV_ALIGN_TOP_MID,0,18);
    lv_obj_t *tl=lv_label_create(b);lv_label_set_text(tl,t);lv_obj_set_style_text_font(tl,&lv_font_montserrat_18,0);lv_obj_set_style_text_color(tl,lv_color_hex(0x333),0);lv_obj_align(tl,LV_ALIGN_CENTER,0,6);
    lv_obj_t *sl=lv_label_create(b);lv_label_set_text(sl,s);lv_obj_set_style_text_font(sl,&lv_font_montserrat_12,0);lv_obj_set_style_text_color(sl,lv_color_hex(0x999),0);lv_obj_align(sl,LV_ALIGN_BOTTOM_MID,0,-12);return b;}
void ui_MainMenu_screen_init(void){
    ui_MainMenu=lv_obj_create(NULL);lv_obj_clear_flag(ui_MainMenu,LV_OBJ_FLAG_SCROLLABLE);lv_obj_set_style_bg_color(ui_MainMenu,lv_color_hex(0xfce4ec),0);lv_obj_set_style_bg_opa(ui_MainMenu,LV_OPA_COVER,0);lv_obj_set_style_pad_all(ui_MainMenu,0,0);
    lv_obj_t *t=lv_label_create(ui_MainMenu);lv_label_set_text(t,"Main Menu");lv_obj_set_style_text_color(t,lv_color_hex(0x37474F),0);lv_obj_set_style_text_font(t,&lv_font_montserrat_26,0);lv_obj_align(t,LV_ALIGN_TOP_MID,0,16);
    lv_obj_t *gear=lv_btn_create(ui_MainMenu);lv_obj_set_size(gear,46,46);lv_obj_set_pos(gear,16,10);lv_obj_set_style_bg_color(gear,lv_color_hex(0x546E7A),0);lv_obj_set_style_radius(gear,23,0);lv_obj_add_event_cb(gear,btn_cb,LV_EVENT_CLICKED,"Controls");lv_obj_t *gl=lv_label_create(gear);lv_label_set_text(gl,"⚙");lv_obj_set_style_text_font(gl,&lv_font_montserrat_22,0);lv_obj_center(gl);
    mk(ui_MainMenu,"📷","Camera","Preview & Snap",82,62,lv_color_hex(0xE3F2FD));
    mk(ui_MainMenu,"🎮","Game","Snake & 2048",342,62,lv_color_hex(0xFFF3E0));
    mk(ui_MainMenu,"💡","Controls","LED & Buzzer",602,62,lv_color_hex(0xFFFDE7));
    mk(ui_MainMenu,"🖼","Album","Photos",82,248,lv_color_hex(0xE8F5E9));
    mk(ui_MainMenu,"👤","Recognition","Face Detect",342,248,lv_color_hex(0xF3E5F5));
    mk(ui_MainMenu,"🚪","Logout","Exit to Login",602,248,lv_color_hex(0xFFEBEE));}
void ui_MainMenu_screen_destroy(void){if(ui_MainMenu){lv_obj_del(ui_MainMenu);ui_MainMenu=NULL;}}
