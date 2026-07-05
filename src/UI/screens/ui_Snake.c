#include "../ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#define COLS 20
#define ROWS 15
#define CELL 32
#define OX 32
#define OY 60
lv_obj_t *ui_Snake=NULL;static lv_obj_t *snake_canvas,*score_lbl;static lv_timer_t *tick;
static int sx[302],sy[302],len,dx,dy,fx,fy,score,over,hi,paused;
static void buzz(int ms){int fd=open("/sys/class/gpio/gpio111/value",O_WRONLY);if(fd>=0){write(fd,"1",1);usleep(ms*1000);write(fd,"0",1);close(fd);}}
static void draw(void){if(!snake_canvas)return;unsigned char *buf=lv_canvas_get_buf(snake_canvas);if(!buf)return;memset(buf,0xFF,COLS*CELL*ROWS*CELL*2);unsigned short *px=(unsigned short*)buf;
    for(int r=0;r<ROWS;r++)for(int c=0;c<COLS*CELL;c++)px[r*CELL*COLS*CELL+c]=0xC618;
    for(int c=0;c<COLS;c++)for(int r=0;r<ROWS*CELL;r++)px[r*COLS*CELL+c*CELL]=0xC618;
    for(int r=fy*CELL;r<(fy+1)*CELL;r++)for(int c=fx*CELL;c<(fx+1)*CELL;c++)if(r<ROWS*CELL&&c<COLS*CELL)px[r*COLS*CELL+c]=0xF800;
    for(int i=0;i<len;i++){int rr=sy[i]*CELL,cc=sx[i]*CELL;for(int r=rr;r<rr+CELL;r++)for(int c=cc;c<cc+CELL;c++)if(r<ROWS*CELL&&c<COLS*CELL)px[r*COLS*CELL+c]=0x07E0;}lv_obj_invalidate(snake_canvas);}
static void reset(void){len=3;for(int i=0;i<3;i++){sx[i]=10-i;sy[i]=7;}dx=1;dy=0;fx=15;fy=7;score=0;over=0;paused=0;buzz(10);draw();lv_label_set_text_fmt(score_lbl,"Score:%d Hi:%d",score,hi);}
static void tick_cb(lv_timer_t *t){(void)t;if(over||paused)return;int nx=sx[0]+dx,ny=sy[0]+dy;if(nx<0||nx>=COLS||ny<0||ny>=ROWS){over=1;buzz(200);lv_label_set_text_fmt(score_lbl,"Game Over! %d Hi:%d",score,hi);return;}for(int i=1;i<len;i++)if(sx[i]==nx&&sy[i]==ny){over=1;buzz(200);lv_label_set_text_fmt(score_lbl,"Game Over! %d Hi:%d",score,hi);return;}for(int i=len;i>0;i--){sx[i]=sx[i-1];sy[i]=sy[i-1];}sx[0]=nx;sy[0]=ny;if(nx==fx&&ny==fy){len++;score+=10;if(score>hi)hi=score;buzz(30);if(len>=300){over=1;lv_label_set_text_fmt(score_lbl,"You Win! %d Hi:%d",score,hi);draw();return;}int _b;do{_b=0;fx=rand()%COLS;fy=rand()%ROWS;for(int _i=0;_i<len;_i++)if(sx[_i]==fx&&sy[_i]==fy){_b=1;break;}}while(_b);lv_label_set_text_fmt(score_lbl,"Score:%d Hi:%d",score,hi);}draw();}
static void gest(lv_event_t *e){lv_dir_t d=lv_indev_get_gesture_dir(lv_indev_active());if(d==LV_DIR_LEFT&&dx!=1){dx=-1;dy=0;}else if(d==LV_DIR_RIGHT&&dx!=-1){dx=1;dy=0;}else if(d==LV_DIR_TOP&&dy!=1){dx=0;dy=-1;}else if(d==LV_DIR_BOTTOM&&dy!=-1){dx=0;dy=1;}}
static void rst(lv_event_t *e){(void)e;reset();}
static void paus(lv_event_t *e){(void)e;paused=!paused;lv_obj_t *c=lv_obj_get_child(lv_event_get_target(e),0);if(c)lv_label_set_text(c,paused?"▶":"⏸");}
static void back_cb(lv_event_t *e){(void)e;if(tick){lv_timer_del(tick);tick=NULL;}ui_load_game();}
static void sl(lv_event_t *e){(void)e;if(dx!=1){dx=-1;dy=0;}}static void sr(lv_event_t *e){(void)e;if(dx!=-1){dx=1;dy=0;}}
static void su(lv_event_t *e){(void)e;if(dy!=1){dx=0;dy=-1;}}static void sd(lv_event_t *e){(void)e;if(dy!=-1){dx=0;dy=1;}}
void ui_Snake_screen_init(void){
    srand(time(NULL));hi=0;
    ui_Snake=lv_obj_create(NULL);lv_obj_clear_flag(ui_Snake,LV_OBJ_FLAG_SCROLLABLE);lv_obj_set_style_bg_color(ui_Snake,lv_color_hex(0xfce4ec),0);lv_obj_set_style_bg_opa(ui_Snake,LV_OPA_COVER,0);
    lv_obj_t *back=lv_btn_create(ui_Snake);lv_obj_set_size(back,60,32);lv_obj_set_pos(back,10,10);lv_obj_set_style_bg_color(back,lv_color_hex(0x546E7A),0);lv_obj_set_style_radius(back,6,0);lv_obj_add_event_cb(back,back_cb,LV_EVENT_CLICKED,NULL);lv_obj_t *bl=lv_label_create(back);lv_label_set_text(bl,LV_SYMBOL_LEFT" Back");lv_obj_center(bl);
    score_lbl=lv_label_create(ui_Snake);lv_label_set_text(score_lbl,"Score:0 Hi:0");lv_obj_align(score_lbl,LV_ALIGN_TOP_MID,0,10);
    lv_obj_t *rbtn=lv_btn_create(ui_Snake);lv_obj_set_size(rbtn,70,30);lv_obj_align(rbtn,LV_ALIGN_TOP_RIGHT,-16,8);lv_obj_set_style_bg_color(rbtn,lv_color_hex(0x43A047),0);lv_obj_set_style_radius(rbtn,6,0);lv_obj_add_event_cb(rbtn,rst,LV_EVENT_CLICKED,NULL);lv_obj_t *rt=lv_label_create(rbtn);lv_label_set_text(rt,"Restart");lv_obj_center(rt);
    lv_obj_t *pbtn=lv_btn_create(ui_Snake);lv_obj_set_size(pbtn,70,30);lv_obj_align(pbtn,LV_ALIGN_TOP_RIGHT,-100,8);lv_obj_set_style_bg_color(pbtn,lv_color_hex(0xFFA726),0);lv_obj_set_style_radius(pbtn,6,0);lv_obj_add_event_cb(pbtn,paus,LV_EVENT_CLICKED,NULL);lv_obj_t *pbl=lv_label_create(pbtn);lv_label_set_text(pbl,"⏸ Pause");lv_obj_center(pbl);
    snake_canvas=lv_canvas_create(ui_Snake);lv_obj_set_size(snake_canvas,COLS*CELL,ROWS*CELL);lv_obj_set_pos(snake_canvas,OX,OY);static unsigned char sbuf[COLS*CELL*ROWS*CELL*2];lv_canvas_set_buffer(snake_canvas,sbuf,COLS*CELL,ROWS*CELL,LV_COLOR_FORMAT_RGB565);
    lv_obj_add_event_cb(ui_Snake,gest,LV_EVENT_GESTURE,NULL);lv_obj_add_event_cb(snake_canvas,gest,LV_EVENT_GESTURE,NULL);
    lv_obj_t *ul=lv_btn_create(ui_Snake);lv_obj_set_size(ul,44,44);lv_obj_set_pos(ul,OX+COLS*CELL/2-22,OY+ROWS*CELL+8);lv_obj_set_style_bg_color(ul,lv_color_hex(0x546E7A),0);lv_obj_set_style_radius(ul,6,0);lv_obj_add_event_cb(ul,sl,LV_EVENT_CLICKED,NULL);lv_obj_t *ut=lv_label_create(ul);lv_label_set_text(ut,LV_SYMBOL_LEFT);lv_obj_center(ut);
    lv_obj_t *ur=lv_btn_create(ui_Snake);lv_obj_set_size(ur,44,44);lv_obj_set_pos(ur,OX+COLS*CELL/2+34,OY+ROWS*CELL+8);lv_obj_set_style_bg_color(ur,lv_color_hex(0x546E7A),0);lv_obj_set_style_radius(ur,6,0);lv_obj_add_event_cb(ur,sr,LV_EVENT_CLICKED,NULL);lv_obj_t *urt=lv_label_create(ur);lv_label_set_text(urt,LV_SYMBOL_RIGHT);lv_obj_center(urt);
    lv_obj_t *uu=lv_btn_create(ui_Snake);lv_obj_set_size(uu,44,44);lv_obj_set_pos(uu,OX+COLS*CELL/2+6,OY+ROWS*CELL-34);lv_obj_set_style_bg_color(uu,lv_color_hex(0x546E7A),0);lv_obj_set_style_radius(uu,6,0);lv_obj_add_event_cb(uu,su,LV_EVENT_CLICKED,NULL);lv_obj_t *uut=lv_label_create(uu);lv_label_set_text(uut,LV_SYMBOL_UP);lv_obj_center(uut);
    lv_obj_t *ud=lv_btn_create(ui_Snake);lv_obj_set_size(ud,44,44);lv_obj_set_pos(ud,OX+COLS*CELL/2+6,OY+ROWS*CELL+50);lv_obj_set_style_bg_color(ud,lv_color_hex(0x546E7A),0);lv_obj_set_style_radius(ud,6,0);lv_obj_add_event_cb(ud,sd,LV_EVENT_CLICKED,NULL);lv_obj_t *udt=lv_label_create(ud);lv_label_set_text(udt,LV_SYMBOL_DOWN);lv_obj_center(udt);
    reset();tick=lv_timer_create(tick_cb,200,NULL);
}
void ui_Snake_screen_destroy(void){if(tick){lv_timer_del(tick);tick=NULL;}if(ui_Snake){lv_obj_del(ui_Snake);ui_Snake=NULL;}}
