#ifndef PRELOAD_H
#define PRELOAD_H
#ifdef __cplusplus
extern "C" {
#endif

void preload_init(void);          /* starts background preload thread */
int  preload_get_progress(void);  /* 0-100, LVGL-thread safe to call */
int  preload_is_done(void);
void preload_force_finish(void);

#ifdef __cplusplus
}
#endif
#endif
