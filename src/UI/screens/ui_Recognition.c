#include "../ui.h"
#include "src/lib/camera.h"
#include "src/lib/face_detect.h"
#include <stdio.h>
#include <string.h>
lv_obj_t *ui_Recognition=NULL;static lv_obj_t *recog_canvas=NULL,*recog_timer=NULL,*recog_label=NULL;
static unsigned char rgb_buf[640*480*3];
static void recog_timer_cb(lv_timer_t *t){(void)t;if(!recog_canvas||!Camera_Is_Started())return;Camera_Show();
    unsigned char *yuv=Camera_Get_Buffer();if(!yuv)return;
    for(int i=0;i<640*480;i+=2){int y0=yuv[0],u=yuv[1]-128,y1=yuv[2],v=yuv[3]-128;yuv+=4;
        for(int j=0;j<2;j++){int yy=j?y1:y0;int r=yy+((1436*v)>>10),g=yy-((352*u+731*v)>>10),b=yy+((1814*u)>>10);
            if(r>255)r=255;else if(r<0)r=0;if(g>255)g=255;else if(g<0)g=0;if(b>255)b=255;else if(b<0)b=0;
            rgb_buf[i*3+j*3]=r;rgb_buf[i*3+j*3+1]=g;rgb_buf[i*3+j*3+2]=b;}}
    face_box_t bx[5];int n=face_detect_run(rgb_buf,640,480,bx,5);
    if(n>0&&recog_label)lv_label_set_text_fmt(recog_label,"%s %.0f%%",bx[0].name,bx[0].conf*100);}
static void start_recog(lv_event_t *e){(void)e;if(!Camera_Is_Started()){Camera_Init("/dev/video9");Camera_Start();}face_detect_init("/root/ljr/models/yolov8n.rknn");recog_timer=lv_timer_create(recog_timer_cb,100,NULL);}
static void capture_recog(lv_event_t *e){(void)e;if(!Camera_Is_Started())return;face_box_t bx[5];int n=face_detect_run(rgb_buf,640,480,bx,5);if(n>0&&recog_label)lv_label_set_text_fmt(recog_label,"%s %.0f%%",bx[0].name,bx[0].conf*100);}
static void back_cb(lv_event_t *e){(void)e;if(recog_timer){lv_timer_del(recog_timer);recog_timer=NULL;}if(Camera_Is_Started())Camera_Quit();ui_load_main_menu();}
void ui_Recognition_screen_init(void){
    face_detect_init("/root/ljr/models/yolov8n.rknn");
    ui_Recognition=lv_obj_create(NULL);lv_obj_clear_flag(ui_Recognition,LV_OBJ_FLAG_SCROLLABLE);lv_obj_set_style_bg_color(ui_Recognition,lv_color_hex(0xfce4ec),0);lv_obj_set_style_bg_opa(ui_Recognition,LV_OPA_COVER,0);lv_obj_set_style_pad_all(ui_Recognition,0,0);
    lv_obj_t *back=lv_btn_create(ui_Recognition);lv_obj_set_size(back,60,32);lv_obj_set_pos(back,16,12);lv_obj_set_style_bg_color(back,lv_color_hex(0x546E7A),0);lv_obj_set_style_radius(back,6,0);lv_obj_add_event_cb(back,back_cb,LV_EVENT_CLICKED,NULL);lv_obj_t *bl=lv_label_create(back);lv_label_set_text(bl,LV_SYMBOL_LEFT" Back");lv_obj_center(bl);
    lv_obj_t *t=lv_label_create(ui_Recognition);lv_label_set_text(t,"Image Recognition");lv_obj_set_style_text_font(t,&lv_font_montserrat_24,0);lv_obj_align(t,LV_ALIGN_TOP_MID,0,20);
    int cx=(1024-640)/2,cy=50;
    recog_canvas=lv_canvas_create(ui_Recognition);lv_obj_set_size(recog_canvas,640,430);lv_obj_set_pos(recog_canvas,cx,cy);lv_canvas_set_buffer(recog_canvas,Camera_Get_Buffer(),640,480,LV_COLOR_FORMAT_RGB565);
    recog_label=lv_label_create(ui_Recognition);lv_label_set_text(recog_label,"Ready");lv_obj_align(recog_label,LV_ALIGN_TOP_MID,0,490);
    lv_obj_t *btnR=lv_btn_create(ui_Recognition);lv_obj_set_size(btnR,160,34);lv_obj_align(btnR,LV_ALIGN_BOTTOM_LEFT,200,0);lv_obj_set_style_bg_color(btnR,lv_color_hex(0x1E88E5),0);lv_obj_set_style_radius(btnR,6,0);lv_obj_add_event_cb(btnR,start_recog,LV_EVENT_CLICKED,NULL);lv_obj_t *rt=lv_label_create(btnR);lv_label_set_text(rt,"Real-Time");lv_obj_center(rt);
    lv_obj_t *btnC=lv_btn_create(ui_Recognition);lv_obj_set_size(btnC,160,34);lv_obj_align(btnC,LV_ALIGN_BOTTOM_RIGHT,-200,0);lv_obj_set_style_bg_color(btnC,lv_color_hex(0x43A047),0);lv_obj_set_style_radius(btnC,6,0);lv_obj_add_event_cb(btnC,capture_recog,LV_EVENT_CLICKED,NULL);lv_obj_t *ct=lv_label_create(btnC);lv_label_set_text(ct,"Capture");lv_obj_center(ct);}
void ui_Recognition_screen_destroy(void){if(recog_timer){lv_timer_del(recog_timer);recog_timer=NULL;}if(Camera_Is_Started())Camera_Quit();if(ui_Recognition){lv_obj_del(ui_Recognition);ui_Recognition=NULL;}}
