#include "../ui.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
lv_obj_t *ui_Controls=NULL;static lv_obj_t *slBL,*vlBL;
static int wf(const char *p,const char *v){int fd=open(p,O_WRONLY);if(fd<0)return-1;int n=strlen(v);return write(fd,v,n)==n?(close(fd),0):(close(fd),-1);}
static void ge(int pin){char b[16];snprintf(b,sizeof(b),"%d",pin);wf("/sys/class/gpio/export",b);}
static void gd(int pin){char p[64];snprintf(p,sizeof(p),"/sys/class/gpio/gpio%d/direction",pin);wf(p,"out");}
static void gv(int pin,int v){char p[64];snprintf(p,sizeof(p),"/sys/class/gpio/gpio%d/value",pin);wf(p,v?"1":"0");}
static void led_on(int id){int lp[]={120,121,123,124};gv(lp[id],1);}
static void led_off(int id){int lp[]={120,121,123,124};gv(lp[id],0);}
static void buz_on(void){gv(111,1);}static void buz_off(void){gv(111,0);}
static void bl_set(int pct){if(pct<0)pct=0;if(pct>100)pct=100;int raw=(pct*255)/100;char b[16];snprintf(b,sizeof(b),"%d",raw);wf("/sys/class/backlight/backlight/brightness",b);}
static void back_cb(lv_event_t *e){(void)e;ui_load_main_menu();}
static void l0_cb(lv_event_t *e){lv_obj_has_state(lv_event_get_target(e),LV_STATE_CHECKED)?led_on(0):led_off(0);}
static void l1_cb(lv_event_t *e){lv_obj_has_state(lv_event_get_target(e),LV_STATE_CHECKED)?led_on(1):led_off(1);}
static void l2_cb(lv_event_t *e){lv_obj_has_state(lv_event_get_target(e),LV_STATE_CHECKED)?led_on(2):led_off(2);}
static void l3_cb(lv_event_t *e){lv_obj_has_state(lv_event_get_target(e),LV_STATE_CHECKED)?led_on(3):led_off(3);}
static void bz_cb(lv_event_t *e){lv_obj_has_state(lv_event_get_target(e),LV_STATE_CHECKED)?buz_on():buz_off();}
static void bl_cb(lv_event_t *e){if(lv_event_get_code(e)==LV_EVENT_VALUE_CHANGED){int b=(int)lv_slider_get_value(slBL);char buf[8];snprintf(buf,sizeof(buf),"%d%%",b);lv_label_set_text(vlBL,buf);bl_set(b);}}
void ui_Controls_screen_init(void){
    int lp[]={120,121,123,124};for(int i=0;i<4;i++){ge(lp[i]);usleep(100000);gd(lp[i]);gv(lp[i],0);}ge(111);usleep(100000);gd(111);gv(111,0);
    ui_Controls=lv_obj_create(NULL);lv_obj_clear_flag(ui_Controls,LV_OBJ_FLAG_SCROLLABLE);lv_obj_set_style_bg_color(ui_Controls,lv_color_hex(0xfce4ec),0);lv_obj_set_style_bg_opa(ui_Controls,LV_OPA_COVER,0);lv_obj_set_style_pad_all(ui_Controls,0,0);
    lv_obj_t *back=lv_btn_create(ui_Controls);lv_obj_set_size(back,60,32);lv_obj_set_pos(back,16,12);lv_obj_set_style_bg_color(back,lv_color_hex(0x546E7A),0);lv_obj_set_style_radius(back,6,0);lv_obj_add_event_cb(back,back_cb,LV_EVENT_CLICKED,NULL);lv_obj_t *bl=lv_label_create(back);lv_label_set_text(bl,LV_SYMBOL_LEFT" Back");lv_obj_center(bl);
    lv_obj_t *t=lv_label_create(ui_Controls);lv_label_set_text(t,"Controls");lv_obj_set_style_text_font(t,&lv_font_montserrat_24,0);lv_obj_align(t,LV_ALIGN_TOP_MID,0,20);
    lv_obj_t *card=lv_obj_create(ui_Controls);lv_obj_set_size(card,520,380);lv_obj_align(card,LV_ALIGN_TOP_MID,0,60);lv_obj_set_style_bg_color(card,lv_color_white(),0);lv_obj_set_style_radius(card,12,0);lv_obj_set_style_border_width(card,0,0);lv_obj_clear_flag(card,LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *r0s,*r1s,*r2s,*r3s,*rBs;
    const char *rows[]={"LED 0 (GPIO120)","LED 1 (GPIO121)","LED 2 (GPIO123)","LED 3 (GPIO124)","Buzzer (GPIO111)"};
    lv_event_cb_t cbs[]={l0_cb,l1_cb,l2_cb,l3_cb,bz_cb};
    lv_obj_t **sts[]={&r0s,&r1s,&r2s,&r3s,&rBs};
    for(int i=0;i<5;i++){
        lv_obj_t *l=lv_label_create(card);lv_label_set_text(l,rows[i]);lv_obj_set_pos(l,30,30+i*52);lv_obj_set_style_text_font(l,&lv_font_montserrat_14,0);
        lv_obj_t *sw=lv_switch_create(card);lv_obj_set_size(sw,60,30);lv_obj_set_pos(sw,350,30+i*52);lv_obj_add_event_cb(sw,cbs[i],LV_EVENT_VALUE_CHANGED,NULL);
        lv_obj_t *v=lv_label_create(card);lv_label_set_text(v,"OFF");lv_obj_set_pos(v,430,30+i*52);lv_obj_set_style_text_font(v,&lv_font_montserrat_14,0);if(sts[i])*sts[i]=v;
    }
    lv_obj_t *blbl=lv_label_create(card);lv_label_set_text(blbl,"Backlight");lv_obj_set_pos(blbl,30,300);lv_obj_set_style_text_font(blbl,&lv_font_montserrat_14,0);
    slBL=lv_slider_create(card);lv_obj_set_size(slBL,240,10);lv_obj_set_pos(slBL,170,304);lv_slider_set_range(slBL,0,100);lv_slider_set_value(slBL,50,LV_ANIM_OFF);lv_obj_add_event_cb(slBL,bl_cb,LV_EVENT_ALL,NULL);
    vlBL=lv_label_create(card);lv_label_set_text(vlBL,"50%");lv_obj_set_pos(vlBL,430,300);lv_obj_set_style_text_font(vlBL,&lv_font_montserrat_14,0);}
void ui_Controls_screen_destroy(void){if(ui_Controls){lv_obj_del(ui_Controls);ui_Controls=NULL;}}
