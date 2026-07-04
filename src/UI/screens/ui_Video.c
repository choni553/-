#include "../ui.h"
#include "src/lib/camera.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#define W 640
#define H 480
lv_obj_t *ui_Video=NULL;static lv_obj_t *canvas=NULL;static lv_timer_t *ctimer=NULL;
static void cam_cb(lv_timer_t *t){(void)t;Camera_Show();if(canvas)lv_obj_invalidate(canvas);}
static void snap_cb(lv_event_t *e){(void)e;if(!Camera_Is_Started())return;mkdir("/root/ljr/album",0755);static int cnt=0;char n[128];snprintf(n,sizeof(n),"/root/ljr/album/snap_%04d.bmp",cnt++);Camera_Get_Pic(n);printf("Snap: %s\n",n);}
static void start_cb(lv_event_t *e){(void)e;if(!Camera_Is_Started()){Camera_Init("/dev/video9");Camera_Start();}}
static void stop_cb(lv_event_t *e){(void)e;if(Camera_Is_Started())Camera_Quit();}
static void back_cb(lv_event_t *e){(void)e;if(ctimer){lv_timer_del(ctimer);ctimer=NULL;}if(Camera_Is_Started())Camera_Quit();ui_load_main_menu();}
static void gesture_cb(lv_event_t *e){lv_dir_t d=lv_indev_get_gesture_dir(lv_indev_active());if(d==LV_DIR_RIGHT)ui_load_music();else if(d==LV_DIR_LEFT)ui_load_recognition();}
void ui_Video_screen_init(void){
    ui_Video=lv_obj_create(NULL);lv_obj_clear_flag(ui_Video,LV_OBJ_FLAG_SCROLLABLE);lv_obj_set_style_bg_color(ui_Video,lv_color_hex(0xfce4ec),0);lv_obj_set_style_bg_opa(ui_Video,LV_OPA_COVER,0);lv_obj_set_style_pad_all(ui_Video,0,0);
    lv_obj_t *back=lv_btn_create(ui_Video);lv_obj_set_size(back,60,32);lv_obj_set_pos(back,16,12);lv_obj_set_style_bg_color(back,lv_color_hex(0x546E7A),0);lv_obj_set_style_radius(back,6,0);lv_obj_set_style_bg_color(back,lv_color_hex(0x37474F),LV_STATE_PRESSED);lv_obj_add_event_cb(back,back_cb,LV_EVENT_CLICKED,NULL);lv_obj_t *bl=lv_label_create(back);lv_label_set_text(bl,LV_SYMBOL_LEFT" Back");lv_obj_set_style_text_color(bl,lv_color_white(),0);lv_obj_center(bl);
    lv_obj_t *t=lv_label_create(ui_Video);lv_label_set_text(t,"Video Player");lv_obj_set_style_text_color(t,lv_color_hex(0x37474F),0);lv_obj_set_style_text_font(t,&lv_font_montserrat_20,0);lv_obj_align(t,LV_ALIGN_TOP_MID,0,14);
    int cx=(1024-W)/2,cy=52;canvas=lv_canvas_create(ui_Video);lv_obj_set_size(canvas,W,H);lv_obj_set_pos(canvas,cx,cy);lv_canvas_set_buffer(canvas,Camera_Get_Buffer(),W,H,LV_COLOR_FORMAT_RGB565);lv_obj_set_style_border_width(canvas,4,0);lv_obj_set_style_border_color(canvas,lv_color_white(),0);
    int by=H+cy+12,bw=110,bh=38,gap=14,bx=cx;
/* View button top-right */
lv_obj_t *view=lv_btn_create(ui_Video);lv_obj_set_size(view,64,32);lv_obj_set_pos(view,940,8);lv_obj_set_style_bg_color(view,lv_color_hex(0x1E88E5),0);lv_obj_set_style_radius(view,6,0);lv_obj_t *vw=lv_label_create(view);lv_label_set_text(vw,"View");lv_obj_center(vw);(void)view;(void)vw;
    lv_obj_t *snap=lv_btn_create(ui_Video);lv_obj_set_size(snap,bw,bh);lv_obj_set_pos(snap,bx,by);lv_obj_set_style_bg_color(snap,lv_color_hex(0x43A047),0);lv_obj_set_style_radius(snap,6,0);lv_obj_set_style_bg_color(snap,lv_color_hex(0x388E3C),LV_STATE_PRESSED);lv_obj_add_event_cb(snap,snap_cb,LV_EVENT_CLICKED,NULL);lv_obj_t *sl=lv_label_create(snap);lv_label_set_text(sl,"Snap");lv_obj_set_style_text_color(sl,lv_color_white(),0);lv_obj_center(sl);
    bx+=bw+gap;lv_obj_t *start=lv_btn_create(ui_Video);lv_obj_set_size(start,bw,bh);lv_obj_set_pos(start,bx,by);lv_obj_set_style_bg_color(start,lv_color_hex(0x1E88E5),0);lv_obj_set_style_radius(start,6,0);lv_obj_set_style_bg_color(start,lv_color_hex(0x1565C0),LV_STATE_PRESSED);lv_obj_add_event_cb(start,start_cb,LV_EVENT_CLICKED,NULL);lv_obj_t *stl=lv_label_create(start);lv_label_set_text(stl,"Start");lv_obj_set_style_text_color(stl,lv_color_white(),0);lv_obj_center(stl);
    bx+=bw+gap;lv_obj_t *stop=lv_btn_create(ui_Video);lv_obj_set_size(stop,bw,bh);lv_obj_set_pos(stop,bx,by);lv_obj_set_style_bg_color(stop,lv_color_hex(0xE53935),0);lv_obj_set_style_radius(stop,6,0);lv_obj_set_style_bg_color(stop,lv_color_hex(0xC62828),LV_STATE_PRESSED);lv_obj_add_event_cb(stop,stop_cb,LV_EVENT_CLICKED,NULL);lv_obj_t *spl=lv_label_create(stop);lv_label_set_text(spl,"Stop");lv_obj_set_style_text_color(spl,lv_color_white(),0);lv_obj_center(spl);
    lv_obj_t *sw=lv_label_create(ui_Video);lv_label_set_text(sw,"Swipe left/right to switch");lv_obj_set_style_text_color(sw,lv_color_hex(0x90A4AE),0);lv_obj_set_style_text_font(sw,&lv_font_montserrat_12,0);lv_obj_align(sw,LV_ALIGN_BOTTOM_MID,0,-5);lv_obj_add_event_cb(ui_Video,gesture_cb,LV_EVENT_GESTURE,NULL);
    ctimer=lv_timer_create(cam_cb,22,NULL);
}
void ui_Video_screen_destroy(void){if(ctimer){lv_timer_del(ctimer);ctimer=NULL;}if(Camera_Is_Started())Camera_Quit();if(ui_Video){lv_obj_del(ui_Video);ui_Video=NULL;}}
