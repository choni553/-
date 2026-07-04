#include "../ui.h"
#include "src/lib/album_mgr.h"
#include <stdio.h>
#include <string.h>
lv_obj_t *ui_Album=NULL;static lv_obj_t *list_label[20],*info_label;static int page=0;
static void refresh_list(void);
static void back_cb(lv_event_t *e){(void)e;ui_load_main_menu();}
static void gesture_cb(lv_event_t *e){lv_dir_t d=lv_indev_get_gesture_dir(lv_indev_active());if(d==LV_DIR_RIGHT)ui_load_game();else if(d==LV_DIR_LEFT)ui_load_music();}
static void prev_cb(lv_event_t *e){(void)e;album_prev();refresh_list();}
static void next_cb(lv_event_t *e){(void)e;album_next();refresh_list();}
static void del_cb(lv_event_t *e){(void)e;album_delete_current();refresh_list();}

/* ---- Fullscreen album_viewer ---- */
static lv_obj_t *album_album_viewer=NULL,*album_vimg=NULL;
static void album_viewer_close(lv_event_t *e){(void)e;if(album_viewer){lv_obj_del(album_viewer);album_viewer=album_vimg=NULL;}lv_scr_load(ui_Album);}
static void album_viewer_next(lv_event_t *e){(void)e;album_next();const char *p=album_current();if(p&&album_vimg)lv_image_set_src(album_vimg,p);}
static void album_viewer_prev(lv_event_t *e){(void)e;album_prev();const char *p=album_current();if(p&&album_vimg)lv_image_set_src(album_vimg,p);}
static void open_album_viewer(int idx){
    const char *p=album_get(idx);if(!p)return;
    album_scan(NULL);for(int i=0;i<idx;i++)album_next();
    album_viewer=lv_obj_create(NULL);lv_obj_clear_flag(album_viewer,LV_OBJ_FLAG_SCROLLABLE);lv_obj_set_style_bg_color(album_viewer,lv_color_black(),0);lv_obj_set_style_bg_opa(album_viewer,LV_OPA_COVER,0);
    album_vimg=lv_image_create(album_viewer);lv_image_set_src(album_vimg,p);lv_obj_center(album_vimg);lv_image_set_scale(album_vimg,384);
    lv_obj_t *close=lv_btn_create(album_viewer);lv_obj_set_size(close,60,32);lv_obj_set_pos(close,16,12);lv_obj_set_style_bg_color(close,lv_color_hex(0x546E7A),0);lv_obj_set_style_radius(close,6,0);lv_obj_add_event_cb(close,album_viewer_close,LV_EVENT_CLICKED,NULL);lv_obj_t *cl=lv_label_create(close);lv_label_set_text(cl,LV_SYMBOL_LEFT" Back");lv_obj_set_style_text_color(cl,lv_color_white(),0);lv_obj_center(cl);
    lv_obj_t *pn=lv_btn_create(album_viewer);lv_obj_set_size(pn,60,80);lv_obj_align(pn,LV_ALIGN_LEFT_MID,16,0);lv_obj_set_style_bg_opa(pn,LV_OPA_30,0);lv_obj_set_style_radius(pn,8,0);lv_obj_add_event_cb(pn,album_viewer_prev,LV_EVENT_CLICKED,NULL);lv_obj_t *pl=lv_label_create(pn);lv_label_set_text(pl,LV_SYMBOL_LEFT);lv_obj_center(pl);
    lv_obj_t *nn=lv_btn_create(album_viewer);lv_obj_set_size(nn,60,80);lv_obj_align(nn,LV_ALIGN_RIGHT_MID,-16,0);lv_obj_set_style_bg_opa(nn,LV_OPA_30,0);lv_obj_set_style_radius(nn,8,0);lv_obj_add_event_cb(nn,album_viewer_next,LV_EVENT_CLICKED,NULL);lv_obj_t *nl=lv_label_create(nn);lv_label_set_text(nl,LV_SYMBOL_RIGHT);lv_obj_center(nl);
    lv_obj_t *info=lv_label_create(album_viewer);lv_label_set_text_fmt(info,"%d/%d",album_get_index()+1,album_count());lv_obj_set_style_text_color(info,lv_color_white(),0);lv_obj_align(info,LV_ALIGN_BOTTOM_MID,0,-20);
    lv_scr_load(album_viewer);lv_obj_invalidate(album_viewer);}

static void item_click_cb(lv_event_t *e){int i=(int)(intptr_t)lv_event_get_user_data(e);open_album_viewer(page*20+i);}
static void refresh_list(void){
    album_scan(NULL);int total=album_count(),max_page=(total+19)/20;if(total==0){lv_label_set_text(info_label,"No photos yet - go take some!");for(int i=0;i<20;i++)if(list_label[i])lv_label_set_text(list_label[i],"");return;}
    if(page>=max_page)page=max_page-1;if(page<0)page=0;
    for(int i=0;i<20;i++){if(list_label[i]){int idx=page*20+i;lv_label_set_text(list_label[i],idx<total?strrchr(album_get(idx),'/')+1:"");}}
    lv_label_set_text_fmt(info_label,"%d photos  Page %d/%d",total,page+1,max_page);}
void ui_Album_screen_init(void){
    ui_Album=lv_obj_create(NULL);lv_obj_clear_flag(ui_Album,LV_OBJ_FLAG_SCROLLABLE);lv_obj_set_style_bg_color(ui_Album,lv_color_hex(0xfce4ec),0);lv_obj_set_style_bg_opa(ui_Album,LV_OPA_COVER,0);lv_obj_set_style_pad_all(ui_Album,0,0);
    lv_obj_t *back=lv_btn_create(ui_Album);lv_obj_set_size(back,60,32);lv_obj_set_pos(back,16,12);lv_obj_set_style_bg_color(back,lv_color_hex(0x546E7A),0);lv_obj_set_style_radius(back,6,0);lv_obj_set_style_bg_color(back,lv_color_hex(0x37474F),LV_STATE_PRESSED);lv_obj_add_event_cb(back,back_cb,LV_EVENT_CLICKED,NULL);lv_obj_t *bl=lv_label_create(back);lv_label_set_text(bl,LV_SYMBOL_LEFT" Back");lv_obj_set_style_text_color(bl,lv_color_white(),0);lv_obj_center(bl);
    lv_obj_t *t=lv_label_create(ui_Album);lv_label_set_text(t,"Album");lv_obj_set_style_text_color(t,lv_color_hex(0x37474F),0);lv_obj_set_style_text_font(t,&lv_font_montserrat_24,0);lv_obj_align(t,LV_ALIGN_TOP_MID,0,20);
    /* Card */
    lv_obj_t *card=lv_obj_create(ui_Album);lv_obj_set_size(card,580,480);lv_obj_align(card,LV_ALIGN_TOP_MID,0,60);lv_obj_set_style_bg_color(card,lv_color_white(),0);lv_obj_set_style_radius(card,12,0);lv_obj_set_style_border_width(card,0,0);lv_obj_clear_flag(card,LV_OBJ_FLAG_SCROLLABLE);
    /* File list */
    for(int i=0;i<20;i++){lv_obj_t *lb=lv_label_create(card);lv_obj_set_pos(lb,16,12+i*22);lv_obj_set_size(lb,520,20);lv_obj_set_style_text_font(lb,&lv_font_montserrat_14,0);lv_obj_add_flag(lb,LV_OBJ_FLAG_CLICKABLE);lv_obj_add_event_cb(lb,item_click_cb,LV_EVENT_CLICKED,(void*)(intptr_t)i);list_label[i]=lb;}
    /* Bottom bar */
    info_label=lv_label_create(ui_Album);lv_obj_align(info_label,LV_ALIGN_BOTTOM_MID,0,-50);lv_obj_set_style_text_color(info_label,lv_color_hex(0x78909C),0);lv_obj_set_style_text_font(info_label,&lv_font_montserrat_12,0);
    lv_obj_t *prev_btn=lv_btn_create(ui_Album);lv_obj_set_size(prev_btn,80,32);lv_obj_align(prev_btn,LV_ALIGN_BOTTOM_LEFT,160,-50);lv_obj_set_style_bg_color(prev_btn,lv_color_hex(0x1E88E5),0);lv_obj_set_style_radius(prev_btn,6,0);lv_obj_add_event_cb(prev_btn,prev_cb,LV_EVENT_CLICKED,NULL);lv_obj_t *pt=lv_label_create(prev_btn);lv_label_set_text(pt,"Prev");lv_obj_center(pt);
    lv_obj_t *next_btn=lv_btn_create(ui_Album);lv_obj_set_size(next_btn,80,32);lv_obj_align(next_btn,LV_ALIGN_BOTTOM_RIGHT,-160,-50);lv_obj_set_style_bg_color(next_btn,lv_color_hex(0x1E88E5),0);lv_obj_set_style_radius(next_btn,6,0);lv_obj_add_event_cb(next_btn,next_cb,LV_EVENT_CLICKED,NULL);lv_obj_t *nt=lv_label_create(next_btn);lv_label_set_text(nt,"Next");lv_obj_center(nt);
    lv_obj_t *del_btn=lv_btn_create(ui_Album);lv_obj_set_size(del_btn,80,32);lv_obj_align(del_btn,LV_ALIGN_BOTTOM_MID,0,-50);lv_obj_set_style_bg_color(del_btn,lv_color_hex(0xE53935),0);lv_obj_set_style_radius(del_btn,6,0);lv_obj_add_event_cb(del_btn,del_cb,LV_EVENT_CLICKED,NULL);lv_obj_t *dt=lv_label_create(del_btn);lv_label_set_text(dt,"Delete");lv_obj_center(dt);
    lv_obj_t *sw=lv_label_create(ui_Album);lv_label_set_text(sw,"Swipe left/right to switch");lv_obj_set_style_text_color(sw,lv_color_hex(0x90A4AE),0);lv_obj_set_style_text_font(sw,&lv_font_montserrat_12,0);lv_obj_align(sw,LV_ALIGN_BOTTOM_MID,0,-10);lv_obj_add_event_cb(ui_Album,gesture_cb,LV_EVENT_GESTURE,NULL);
    page=0;refresh_list();}
void ui_Album_screen_destroy(void){if(album_viewer){lv_obj_del(album_viewer);album_viewer=album_vimg=NULL;}if(ui_Album){lv_obj_del(ui_Album);ui_Album=NULL;}}
