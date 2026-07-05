#include "../ui.h"
#include "src/lib/album_mgr.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define COLS 6
#define ROWS 3
#define PER_PAGE (COLS*ROWS)
lv_obj_t *ui_Album=NULL;static lv_obj_t *thumb_c[PER_PAGE],*thumb_l[PER_PAGE],*info_label;static int page=0;
static lv_obj_t *viewer=NULL,*vcanvas=NULL;static int vidx=0;
static void open_viewer(int idx);

static unsigned char *bmp_read(const char *path,int *w,int *h){
    FILE *f=fopen(path,"rb");if(!f)return NULL;unsigned char hdr[54];fread(hdr,1,54,f);
    int bw=*(int*)(hdr+18),bh=*(int*)(hdr+22),off=*(int*)(hdr+10);
    if(hdr[0]!='B'||hdr[1]!='M'){fclose(f);return NULL;}
    if(w)*w=bw;if(h)*h=abs(bh);
    int row_pad=((bw*3)+3)&~3,img_sz=bw*abs(bh)*3;unsigned char *buf=malloc(img_sz);
    if(!buf){fclose(f);return NULL;}fseek(f,off,SEEK_SET);unsigned char *row=malloc(row_pad);
    for(int y=0;y<abs(bh);y++){int dst_y=(bh>0)?(abs(bh)-1-y):y;fread(row,row_pad,1,f);
        for(int x=0;x<bw;x++){int di=(dst_y*bw+x)*3;buf[di+2]=row[x*3];buf[di+1]=row[x*3+1];buf[di]=row[x*3+2];}}
    free(row);fclose(f);return buf;}

static void bmp2cv(lv_obj_t *cv,const char *path,int cw,int ch){
    int bw,bh;unsigned char *rgb=bmp_read(path,&bw,&bh);if(!rgb)return;
    unsigned char *cb=lv_canvas_get_buf(cv);if(!cb){free(rgb);return;}memset(cb,0xFF,cw*ch*2);
    int sx=(bw<<8)/cw,sy=(bh<<8)/ch;
    for(int ty=0;ty<ch;ty++){int ssy=(ty*sy)>>8;if(ssy>=bh)ssy=bh-1;
        for(int tx=0;tx<cw;tx++){int ssx=(tx*sx)>>8;if(ssx>=bw)ssx=bw-1;
            int si=(ssy*bw+ssx)*3;unsigned char r=rgb[si],g=rgb[si+1],b=rgb[si+2];
            ((unsigned short*)cb)[ty*cw+tx]=((r>>3)<<11)|((g>>2)<<5)|(b>>3);}}free(rgb);lv_obj_invalidate(cv);}

/* ---- Viewer ---- */
static void vclose(lv_event_t *e){(void)e;if(viewer){lv_obj_del(viewer);viewer=vcanvas=NULL;}lv_scr_load(ui_Album);refresh_list();}
static void vprev(lv_event_t *e){(void)e;vidx--;if(vidx<0)vidx=album_count()-1;bmp2cv(vcanvas,album_get(vidx),640,480);}
static void vnext(lv_event_t *e){(void)e;vidx++;if(vidx>=album_count())vidx=0;bmp2cv(vcanvas,album_get(vidx),640,480);}
static void vdel(lv_event_t *e){(void)e;album_scan(NULL);for(int i=0;i<=vidx;i++)album_next();album_delete_current();lv_obj_del(viewer);viewer=vcanvas=NULL;lv_scr_load(ui_Album);refresh_list();}

static void open_viewer(int idx){
    const char *p=album_get(idx);if(!p)return;vidx=idx;album_scan(NULL);for(int i=0;i<idx;i++)album_next();
    viewer=lv_obj_create(NULL);lv_obj_clear_flag(viewer,LV_OBJ_FLAG_SCROLLABLE);lv_obj_set_style_bg_color(viewer,lv_color_hex(0x37474F),0);lv_obj_set_style_bg_opa(viewer,LV_OPA_COVER,0);
    vcanvas=lv_canvas_create(viewer);lv_obj_set_size(vcanvas,640,480);lv_obj_align(vcanvas,LV_ALIGN_CENTER,0,-20);static unsigned char vbuf[640*480*2];lv_canvas_set_buffer(vcanvas,vbuf,640,480,LV_COLOR_FORMAT_RGB565);
    bmp2cv(vcanvas,p,640,480);
    lv_obj_t *close=lv_btn_create(viewer);lv_obj_set_size(close,60,32);lv_obj_set_pos(close,16,12);lv_obj_set_style_bg_color(close,lv_color_hex(0x546E7A),0);lv_obj_set_style_radius(close,6,0);lv_obj_add_event_cb(close,vclose,LV_EVENT_CLICKED,NULL);lv_obj_t *cl=lv_label_create(close);lv_label_set_text(cl,LV_SYMBOL_LEFT" Back");lv_obj_center(cl);
    lv_obj_t *pn=lv_btn_create(viewer);lv_obj_set_size(pn,44,70);lv_obj_align(pn,LV_ALIGN_LEFT_MID,8,0);lv_obj_set_style_bg_opa(pn,LV_OPA_30,0);lv_obj_set_style_radius(pn,8,0);lv_obj_add_event_cb(pn,vprev,LV_EVENT_CLICKED,NULL);lv_obj_t *pl=lv_label_create(pn);lv_label_set_text(pl,LV_SYMBOL_LEFT);lv_obj_center(pl);
    lv_obj_t *nn=lv_btn_create(viewer);lv_obj_set_size(nn,44,70);lv_obj_align(nn,LV_ALIGN_RIGHT_MID,-8,0);lv_obj_set_style_bg_opa(nn,LV_OPA_30,0);lv_obj_set_style_radius(nn,8,0);lv_obj_add_event_cb(nn,vnext,LV_EVENT_CLICKED,NULL);lv_obj_t *nl=lv_label_create(nn);lv_label_set_text(nl,LV_SYMBOL_RIGHT);lv_obj_center(nl);
    lv_obj_t *del=lv_btn_create(viewer);lv_obj_set_size(del,80,36);lv_obj_align(del,LV_ALIGN_BOTTOM_MID,0,-8);lv_obj_set_style_bg_color(del,lv_color_hex(0xE53935),0);lv_obj_set_style_radius(del,6,0);lv_obj_add_event_cb(del,vdel,LV_EVENT_CLICKED,NULL);lv_obj_t *dt=lv_label_create(del);lv_label_set_text(dt,"Delete");lv_obj_center(dt);
    lv_scr_load(viewer);lv_obj_invalidate(viewer);}

/* ---- Thumbnails: 3 rows x 6 cols ---- */
static void thumb_click(lv_event_t *e){int *pi=(int*)lv_event_get_user_data(e);open_viewer(page*PER_PAGE+*pi);}
static void make_thumb(const char *path,int idx){
    int x=40+(idx%COLS)*162,y=70+(idx/COLS)*130;
    lv_obj_t *c=lv_canvas_create(ui_Album);lv_obj_set_size(c,130,100);lv_obj_set_pos(c,x,y);lv_obj_set_style_bg_color(c,lv_color_hex(0xDDD),0);
    static unsigned char tb[PER_PAGE][130*100*2];lv_canvas_set_buffer(c,tb[idx],130,100,LV_COLOR_FORMAT_RGB565);thumb_c[idx]=c;
    bmp2cv(c,path,130,100);lv_obj_add_flag(c,LV_OBJ_FLAG_CLICKABLE);
    int *pi=malloc(sizeof(int));*pi=idx;lv_obj_add_event_cb(c,thumb_click,LV_EVENT_CLICKED,pi);
    lv_obj_t *lb=lv_label_create(ui_Album);const char *n=strrchr(path,'/');lv_label_set_text(lb,n?n+1:path);lv_obj_set_pos(lb,x,y+100);lv_obj_set_size(lb,130,16);lv_obj_set_style_text_font(lb,&lv_font_montserrat_12,0);thumb_l[idx]=lb;}

static void refresh_list(void){
    album_scan(NULL);int total=album_count(),max=(total+PER_PAGE-1)/PER_PAGE;if(page>=max&&max>0)page=max-1;if(page<0)page=0;
    for(int i=0;i<PER_PAGE;i++){if(thumb_c[i]){lv_obj_del(thumb_c[i]);thumb_c[i]=NULL;}if(thumb_l[i]){lv_obj_del(thumb_l[i]);thumb_l[i]=NULL;}}
    for(int i=0;i<PER_PAGE;i++){int idx=page*PER_PAGE+i;if(idx<total)make_thumb(album_get(idx),i);}
    lv_label_set_text_fmt(info_label,"%d photos  Page %d/%d",total,page+1,max>0?max:1);}
static void back_cb(lv_event_t *e){(void)e;ui_load_main_menu();}
static void gesture_cb(lv_event_t *e){lv_dir_t d=lv_indev_get_gesture_dir(lv_indev_active());if(d==LV_DIR_RIGHT)ui_load_game();else if(d==LV_DIR_LEFT)ui_load_music();}
static void prev_page(lv_event_t *e){(void)e;if(page>0)page--;refresh_list();}
static void next_page(lv_event_t *e){(void)e;int total=album_count(),max=(total+PER_PAGE-1)/PER_PAGE;if(page<max-1)page++;refresh_list();}

void ui_Album_screen_init(void){
    ui_Album=lv_obj_create(NULL);lv_obj_clear_flag(ui_Album,LV_OBJ_FLAG_SCROLLABLE);lv_obj_set_style_bg_color(ui_Album,lv_color_hex(0xfce4ec),0);lv_obj_set_style_bg_opa(ui_Album,LV_OPA_COVER,0);lv_obj_set_style_pad_all(ui_Album,0,0);
    lv_obj_t *back=lv_btn_create(ui_Album);lv_obj_set_size(back,60,32);lv_obj_set_pos(back,16,12);lv_obj_set_style_bg_color(back,lv_color_hex(0x546E7A),0);lv_obj_set_style_radius(back,6,0);lv_obj_add_event_cb(back,back_cb,LV_EVENT_CLICKED,NULL);lv_obj_t *bl=lv_label_create(back);lv_label_set_text(bl,LV_SYMBOL_LEFT" Back");lv_obj_center(bl);
    lv_obj_t *t=lv_label_create(ui_Album);lv_label_set_text(t,"Album");lv_obj_set_style_text_color(t,lv_color_hex(0x37474F),0);lv_obj_set_style_text_font(t,&lv_font_montserrat_24,0);lv_obj_align(t,LV_ALIGN_TOP_MID,0,20);
    info_label=lv_label_create(ui_Album);lv_obj_align(info_label,LV_ALIGN_BOTTOM_MID,0,-40);
    lv_obj_t *prev=lv_btn_create(ui_Album);lv_obj_set_size(prev,60,28);lv_obj_align(prev,LV_ALIGN_BOTTOM_LEFT,200,-40);lv_obj_set_style_bg_color(prev,lv_color_hex(0x1E88E5),0);lv_obj_set_style_radius(prev,6,0);lv_obj_add_event_cb(prev,prev_page,LV_EVENT_CLICKED,NULL);lv_obj_t *pt=lv_label_create(prev);lv_label_set_text(pt,"Prev");lv_obj_center(pt);
    lv_obj_t *next=lv_btn_create(ui_Album);lv_obj_set_size(next,60,28);lv_obj_align(next,LV_ALIGN_BOTTOM_RIGHT,-200,-40);lv_obj_set_style_bg_color(next,lv_color_hex(0x1E88E5),0);lv_obj_set_style_radius(next,6,0);lv_obj_add_event_cb(next,next_page,LV_EVENT_CLICKED,NULL);lv_obj_t *nt=lv_label_create(next);lv_label_set_text(nt,"Next");lv_obj_center(nt);
    lv_obj_t *sw=lv_label_create(ui_Album);lv_label_set_text(sw,"Swipe left/right to switch");lv_obj_set_style_text_color(sw,lv_color_hex(0x90A4AE),0);lv_obj_set_style_text_font(sw,&lv_font_montserrat_12,0);lv_obj_align(sw,LV_ALIGN_BOTTOM_MID,0,-10);lv_obj_add_event_cb(ui_Album,gesture_cb,LV_EVENT_GESTURE,NULL);
    memset(thumb_c,0,sizeof(thumb_c));memset(thumb_l,0,sizeof(thumb_l));page=0;refresh_list();}
void ui_Album_screen_destroy(void){if(viewer){lv_obj_del(viewer);viewer=vcanvas=NULL;}if(ui_Album){lv_obj_del(ui_Album);ui_Album=NULL;}}
