#include "../ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#define SZ 4
#define CS 100
#define GX 192
#define GY 80
lv_obj_t *ui_2048=NULL;static lv_obj_t *g_canvas,*g_score;static int grid[SZ][SZ],score=0,hi=0;
static unsigned int colors[]={0xCDC1B4,0xEEE4DA,0xEDE0C8,0xF2B179,0xF59563,0xF67C5F,0xF65E3B,0xEDCF72,0xEDCC61,0xEDC850,0xEDC53F,0xEDC22E};
static void buzz(int ms){int fd=open("/sys/class/gpio/gpio111/value",O_WRONLY);if(fd>=0){write(fd,"1",1);usleep(ms*1000);write(fd,"0",1);close(fd);}}
static void draw(void){if(!g_canvas)return;unsigned char *buf=lv_canvas_get_buf(g_canvas);unsigned short *px=(unsigned short*)buf;int stride=SZ*CS;
    for(int r=0;r<SZ*CS;r++)for(int c=0;c<SZ*CS;c++)px[r*stride+c]=0xBBADA0;
    for(int i=0;i<SZ;i++)for(int j=0;j<SZ;j++){int v=grid[i][j],ci=v==0?0:(v<=2048?__builtin_ctz(v):11);if(ci>11)ci=11;for(int r=i*CS+4;r<(i+1)*CS-4;r++)for(int c=j*CS+4;c<(j+1)*CS-4;c++)px[r*stride+c]=colors[ci];}lv_obj_invalidate(g_canvas);}
static void add(void){int e[SZ*SZ][2],n=0;for(int r=0;r<SZ;r++)for(int c=0;c<SZ;c++)if(!grid[r][c]){e[n][0]=r;e[n][1]=c;n++;}if(n){int *p=e[rand()%n];grid[p[0]][p[1]]=(rand()%10<9)?2:4;}}
static int slide_row(int *r){int ch=0,p=0,l=0;for(int i=0;i<SZ;i++){if(!r[i])continue;if(l==r[i]){r[p-1]*=2;score+=r[p-1];l=0;ch=1;}else{r[p++]=r[i];l=r[i];}}for(int i=p;i<SZ;i++)r[i]=0;return ch||(p!=SZ);}
static void move(int dr,int dc){int mv=0;
    if(dr){for(int c=0;c<SZ;c++){int col[SZ];for(int r=0;r<SZ;r++)col[r]=grid[dr>0?SZ-1-r:r][c];if(slide_row(col))mv=1;for(int r=0;r<SZ;r++)grid[dr>0?SZ-1-r:r][c]=col[r];}}
    else{for(int r=0;r<SZ;r++){int row[SZ];for(int c=0;c<SZ;c++)row[c]=grid[r][dc>0?SZ-1-c:c];if(slide_row(row))mv=1;for(int c=0;c<SZ;c++)grid[r][dc>0?SZ-1-c:c]=row[c];}}
    if(mv){add();buzz(15);draw();lv_label_set_text_fmt(g_score,"Score:%d Hi:%d",score,hi);if(score>hi)hi=score;}}
static void gest(lv_event_t *e){lv_dir_t d=lv_indev_get_gesture_dir(lv_indev_active());if(d==LV_DIR_LEFT)move(0,-1);else if(d==LV_DIR_RIGHT)move(0,1);else if(d==LV_DIR_TOP)move(-1,0);else if(d==LV_DIR_BOTTOM)move(1,0);}
static void reset(void){memset(grid,0,sizeof(grid));score=0;add();add();draw();lv_label_set_text_fmt(g_score,"Score:%d Hi:%d",score,hi);}
static void rst(lv_event_t *e){(void)e;reset();}
static void back_cb(lv_event_t *e){(void)e;ui_load_game();}
void ui_2048_screen_init(void){
    srand(time(NULL));hi=0;
    ui_2048=lv_obj_create(NULL);lv_obj_clear_flag(ui_2048,LV_OBJ_FLAG_SCROLLABLE);lv_obj_set_style_bg_color(ui_2048,lv_color_hex(0xfce4ec),0);lv_obj_set_style_bg_opa(ui_2048,LV_OPA_COVER,0);
    lv_obj_t *back=lv_btn_create(ui_2048);lv_obj_set_size(back,60,32);lv_obj_set_pos(back,10,10);lv_obj_set_style_bg_color(back,lv_color_hex(0x546E7A),0);lv_obj_set_style_radius(back,6,0);lv_obj_add_event_cb(back,back_cb,LV_EVENT_CLICKED,NULL);lv_obj_t *bl=lv_label_create(back);lv_label_set_text(bl,LV_SYMBOL_LEFT" Back");lv_obj_center(bl);
    g_score=lv_label_create(ui_2048);lv_label_set_text(g_score,"Score:0 Hi:0");lv_obj_align(g_score,LV_ALIGN_TOP_MID,0,10);
    lv_obj_t *rbtn=lv_btn_create(ui_2048);lv_obj_set_size(rbtn,70,30);lv_obj_align(rbtn,LV_ALIGN_TOP_RIGHT,-16,8);lv_obj_set_style_bg_color(rbtn,lv_color_hex(0x43A047),0);lv_obj_set_style_radius(rbtn,6,0);lv_obj_add_event_cb(rbtn,rst,LV_EVENT_CLICKED,NULL);lv_obj_t *rt=lv_label_create(rbtn);lv_label_set_text(rt,"Restart");lv_obj_center(rt);
    g_canvas=lv_canvas_create(ui_2048);lv_obj_set_size(g_canvas,SZ*CS,SZ*CS);lv_obj_set_pos(g_canvas,GX,GY);static unsigned char buf[SZ*CS*SZ*CS*2];lv_canvas_set_buffer(g_canvas,buf,SZ*CS,SZ*CS,LV_COLOR_FORMAT_RGB565);
    lv_obj_add_event_cb(ui_2048,gest,LV_EVENT_GESTURE,NULL);reset();}
void ui_2048_screen_destroy(void){if(ui_2048){lv_obj_del(ui_2048);ui_2048=NULL;}}
