#ifndef CAMERA_H
#define CAMERA_H
#ifdef __cplusplus
extern "C" {
#endif
int  Camera_Init(char *devname);
int  Camera_Start(void);
int  Camera_Show(void);
int  Camera_Quit(void);
int  Camera_Get_Pic(char *bmp_name);
unsigned char *Camera_Get_Buffer(void);
int  Camera_Is_Started(void);
#ifdef __cplusplus
}
#endif
#endif
