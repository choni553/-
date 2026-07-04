#include "../ui.h"
#include "src/lib/preload.h"
lv_obj_t *ui_Splash = NULL;
static lv_obj_t *bar;
static lv_timer_t *tick_timer;

static void splash_click_cb(lv_event_t *e)
{
    (void)e;
    preload_force_finish();
}

/* LVGL timer: read progress (LVGL-thread safe) and update bar */
static void tick_timer_cb(lv_timer_t *t)
{
    (void)t;
    int p = preload_get_progress();
    lv_bar_set_value(bar, p, LV_ANIM_ON);

    if (preload_is_done()) {
        if (tick_timer) { lv_timer_del(tick_timer); tick_timer = NULL; }
        ui_load_login_screen();
    }
}

void ui_Splash_screen_init(void)
{
    ui_Splash = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Splash, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_Splash, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(ui_Splash, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(ui_Splash, 0, 0);

    /* Background image */
    lv_obj_t *bg = lv_image_create(ui_Splash);
    lv_image_set_src(bg, &bg_login_img);
    lv_obj_align(bg, LV_ALIGN_CENTER, 0, 0);

    /* Animated GIF (static const array from gif_anim.c) */
    lv_obj_t *anim = lv_animimg_create(ui_Splash);
    lv_animimg_set_src(anim, (const void **)gif_anim_frames, gif_anim_num_frames);
    lv_animimg_set_duration(anim, gif_anim_duration);
    lv_animimg_set_repeat_count(anim, LV_ANIM_REPEAT_INFINITE);
    lv_obj_center(anim);
    lv_image_set_scale_x(anim, 546);
    lv_image_set_scale_y(anim, 625);
    lv_animimg_start(anim);

    /* Progress bar */
    bar = lv_bar_create(ui_Splash);
    lv_obj_set_size(bar, 700, 8);
    lv_obj_align(bar, LV_ALIGN_BOTTOM_MID, 0, -60);
    lv_bar_set_range(bar, 0, 100);
    lv_bar_set_value(bar, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(bar, lv_color_hex(0x333333), 0);
    lv_obj_set_style_bg_color(bar, lv_color_hex(0xe94560), LV_PART_INDICATOR);

    /* Click to skip */
    lv_obj_add_event_cb(ui_Splash, splash_click_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(anim, splash_click_cb, LV_EVENT_CLICKED, NULL);

    /* Start preload */
    preload_init();

    /* LVGL timer polls progress and updates the bar */
    tick_timer = lv_timer_create(tick_timer_cb, 50, NULL);
}

void ui_Splash_screen_destroy(void)
{
    if (tick_timer) { lv_timer_del(tick_timer); tick_timer = NULL; }
    if (ui_Splash) {
        /* Stop animimg child before deleting screen */
        lv_obj_t *anim = lv_obj_get_child(ui_Splash, 1);
        if (anim) lv_obj_del(anim);
        lv_obj_del(ui_Splash);
    }
    ui_Splash = NULL;
}
