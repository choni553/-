#ifndef FACE_DETECT_H
#define FACE_DETECT_H
#ifdef __cplusplus
extern "C" {
#endif
typedef struct {int x,y,w,h;float conf;char name[32];} face_box_t;
int  face_detect_init(const char *model_path);
int  face_detect_run(const unsigned char *rgb888,int w,int h,face_box_t *boxes,int max_boxes);
void face_detect_deinit(void);
int  face_register(const char *name,const unsigned char *rgb888,int w,int h);
#ifdef __cplusplus
}
#endif
#endif
