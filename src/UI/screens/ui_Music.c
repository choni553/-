#include "../ui.h"
#include <stdlib.h>
#include <unistd.h>
lv_obj_t *ui_Music=NULL;
static void back_cb(lv_event_t *e){(void)e;system("killall aplay 2>/dev/null");ui_load_main_menu();}
static void gesture_cb(lv_event_t *e){lv_dir_t d=lv_indev_get_gesture_dir(lv_indev_active());if(d==LV_DIR_RIGHT)ui_load_video();else if(d==LV_DIR_LEFT)ui_load_album();}
static void play_cb(lv_event_t *e){(void)e;system("aplay -q /root/music/song1.wav &");}
static void stop_cb(lv_event_t *e){(void)e;system("killall aplay 2>/dev/null");}
void ui_Music_screen_init(void){
    ui_Music=lv_obj_create(NULL);lv_obj_clear_flag(ui_Music,LV_OBJ_FLAG_SCROLLABLE);lv_obj_set_style_bg_color(ui_Music,lv_color_hex(0xfce4ec),0);lv_obj_set_style_bg_opa(ui_Music,LV_OPA_COVER,0);lv_obj_set_style_pad_all(ui_Music,0,0);
    lv_obj_t *back=lv_btn_create(ui_Music);lv_obj_set_size(back,60,32);lv_obj_set_pos(back,16,12);lv_obj_set_style_bg_color(back,lv_color_hex(0x546E7A),0);lv_obj_set_style_radius(back,6,0);lv_obj_set_style_bg_color(back,lv_color_hex(0x37474F),LV_STATE_PRESSED);lv_obj_add_event_cb(back,back_cb,LV_EVENT_CLICKED,NULL);lv_obj_t *bl=lv_label_create(back);lv_label_set_text(bl,LV_SYMBOL_LEFT" Back");lv_obj_set_style_text_color(bl,lv_color_white(),0);lv_obj_center(bl);
    lv_obj_t *t=lv_label_create(ui_Music);lv_label_set_text(t,"Music Player");lv_obj_set_style_text_color(t,lv_color_hex(0x37474F),0);lv_obj_set_style_text_font(t,&lv_font_montserrat_24,0);lv_obj_align(t,LV_ALIGN_TOP_MID,0,20);
    lv_obj_t *card=lv_obj_create(ui_Music);lv_obj_set_size(card,500,300);lv_obj_align(card,LV_ALIGN_CENTER,0,0);lv_obj_set_style_bg_color(card,lv_color_white(),0);lv_obj_set_style_radius(card,12,0);lv_obj_set_style_border_width(card,0,0);lv_obj_clear_flag(card,LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *info=lv_label_create(card);lv_label_set_text(info,"Place .wav files in /root/music/");lv_obj_set_style_text_color(info,lv_color_hex(0x999),0);lv_obj_align(info,LV_ALIGN_CENTER,0,-30);
    lv_obj_t *play=lv_btn_create(card);lv_obj_set_size(play,100,100);lv_obj_align(play,LV_ALIGN_CENTER,0,30);lv_obj_set_style_bg_color(play,lv_color_hex(0x4CAF50),0);lv_obj_set_style_radius(play,50,0);lv_obj_set_style_bg_color(play,lv_color_hex(0x388E3C),LV_STATE_PRESSED);lv_obj_add_event_cb(play,play_cb,LV_EVENT_CLICKED,NULL);lv_obj_t *pl=lv_label_create(play);lv_label_set_text(pl,LV_SYMBOL_PLAY);lv_obj_set_style_text_font(pl,&lv_font_montserrat_36,0);lv_obj_set_style_text_color(pl,lv_color_white(),0);lv_obj_center(pl);
    lv_obj_t *stop=lv_btn_create(card);lv_obj_set_size(stop,80,50);lv_obj_align(stop,LV_ALIGN_BOTTOM_MID,0,-20);lv_obj_set_style_bg_color(stop,lv_color_hex(0xE53935),0);lv_obj_set_style_radius(stop,10,0);lv_obj_set_style_bg_color(stop,lv_color_hex(0xC62828),LV_STATE_PRESSED);lv_obj_add_event_cb(stop,stop_cb,LV_EVENT_CLICKED,NULL);lv_obj_t *sl=lv_label_create(stop);lv_label_set_text(sl,LV_SYMBOL_STOP);lv_obj_set_style_text_color(sl,lv_color_white(),0);lv_obj_center(sl);
    lv_obj_t *sw=lv_label_create(ui_Music);lv_label_set_text(sw,"Swipe left/right to switch");lv_obj_set_style_text_color(sw,lv_color_hex(0x90A4AE),0);lv_obj_set_style_text_font(sw,&lv_font_montserrat_12,0);lv_obj_align(sw,LV_ALIGN_BOTTOM_MID,0,-10);lv_obj_add_event_cb(ui_Music,gesture_cb,LV_EVENT_GESTURE,NULL);
}
void ui_Music_screen_destroy(void){system("killall aplay 2>/dev/null");if(ui_Music){lv_obj_del(ui_Music);ui_Music=NULL;}}
