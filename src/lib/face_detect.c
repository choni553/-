#include "face_detect.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* 
 * RKNN face detection module (placeholder for now)
 * Real implementation requires: rknn_api.h, librknnrt.so
 * Current: returns simulated detections for UI testing
 */
static int initialized=0;
int face_detect_init(const char *model_path){
    (void)model_path;
    fprintf(stderr,"[FACE] init (simulated) with %s\n",model_path?model_path:"default");
    initialized=1;return 0;}
int face_detect_run(const unsigned char *rgb,int w,int h,face_box_t *boxes,int max){
    (void)rgb;(void)w;(void)h;
    if(!initialized||!boxes||max<1)return 0;
    /* Simulated: one face at center */
    boxes[0].x=w/2-80;boxes[0].y=h/2-100;boxes[0].w=160;boxes[0].h=200;boxes[0].conf=0.92f;strcpy(boxes[0].name,"Person");
    return 1;}
void face_detect_deinit(void){initialized=0;}
int face_register(const char *name,const unsigned char *rgb,int w,int h){(void)name;(void)rgb;(void)w;(void)h;return 0;}
