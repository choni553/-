#include "../ui.h"
#include <stdio.h>
lv_obj_t *ui_RecogPage=NULL;
static void back_cb(lv_event_t *e){(void)e;ui_load_main_menu();}
static void capture_cb(lv_event_t *e){
    (void)e;
    lv_obj_t *lbl=lv_event_get_user_data(e);
    lv_label_set_text(lbl,"Analyzing...");
}
static void gesture_cb(lv_event_t *e){
    lv_dir_t d=lv_indev_get_gesture_dir(lv_indev_active());
    if(d==LV_DIR_RIGHT)ui_load_video();
    else if(d==LV_DIR_LEFT)ui_load_game();
}
void ui_RecogPage_screen_init(void){
    ui_RecogPage=lv_obj_create(NULL);
    lv_obj_clear_flag(ui_RecogPage,LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_RecogPage,lv_color_white(),0);
    lv_obj_set_style_bg_opa(ui_RecogPage,LV_OPA_COVER,0);
    lv_obj_set_style_pad_all(ui_RecogPage,0,0);
    lv_obj_t *t=lv_label_create(ui_RecogPage);lv_label_set_text(t,"Image Recognition");
    lv_obj_set_style_text_color(t,lv_color_hex(0x1A237E),0);lv_obj_set_style_text_font(t,&lv_font_montserrat_28,0);
    lv_obj_align(t,LV_ALIGN_TOP_MID,0,20);
    lv_obj_t *back=lv_btn_create(ui_RecogPage);lv_obj_set_size(back,60,32);
    lv_obj_align(back,LV_ALIGN_TOP_LEFT,16,14);lv_obj_set_style_bg_color(back,lv_color_hex(0x546E7A),0);
    lv_obj_set_style_radius(back,6,0);lv_obj_add_event_cb(back,back_cb,LV_EVENT_CLICKED,NULL);
    lv_obj_t *bl=lv_label_create(back);lv_label_set_text(bl,LV_SYMBOL_LEFT" Back");
    lv_obj_set_style_text_color(bl,lv_color_white(),0);lv_obj_set_style_text_font(bl,&lv_font_montserrat_14,0);lv_obj_center(bl);
    lv_obj_t *cap=lv_btn_create(ui_RecogPage);lv_obj_set_size(cap,200,50);
    lv_obj_align(cap,LV_ALIGN_CENTER,0,-30);lv_obj_set_style_bg_color(cap,lv_color_hex(0x1A237E),0);
    lv_obj_set_style_radius(cap,8,0);
    lv_obj_t *cl=lv_label_create(cap);lv_label_set_text(cl,"Capture & Recognize");
    lv_obj_set_style_text_color(cl,lv_color_white(),0);lv_obj_center(cl);
    lv_obj_t *status=lv_label_create(ui_RecogPage);lv_label_set_text(status,"Press button to start");
    lv_obj_set_style_text_color(status,lv_color_hex(0x78909C),0);lv_obj_align(status,LV_ALIGN_CENTER,0,40);
    lv_obj_add_event_cb(cap,capture_cb,LV_EVENT_CLICKED,(void*)status);
    lv_obj_t *hint=lv_label_create(ui_RecogPage);lv_label_set_text(hint,"Swipe left/right to switch");
    lv_obj_set_style_text_color(hint,lv_color_hex(0x90A4AE),0);lv_obj_set_style_text_font(hint,&lv_font_montserrat_12,0);
    lv_obj_align(hint,LV_ALIGN_BOTTOM_MID,0,-10);
    lv_obj_add_event_cb(ui_RecogPage,gesture_cb,LV_EVENT_GESTURE,NULL);
}
void ui_RecogPage_screen_destroy(void){if(ui_RecogPage){lv_obj_del(ui_RecogPage);ui_RecogPage=NULL;}}
