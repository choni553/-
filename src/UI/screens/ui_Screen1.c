#include "../ui.h"
#include <string.h>

lv_obj_t * ui_Screen1 = NULL;
lv_obj_t * ui_username_ta = NULL, *ui_password_ta = NULL, *ui_login_btn = NULL, *ui_logo_img = NULL, *ui_status_label = NULL;
static lv_obj_t * local_kb = NULL;

static void ta_focus_cb(lv_event_t * e) {
    lv_obj_t * ta = lv_event_get_target(e);
    if (local_kb) { lv_keyboard_set_textarea(local_kb, ta); lv_obj_clear_flag(local_kb, LV_OBJ_FLAG_HIDDEN); }
}
static void hide_kb(lv_event_t * e __attribute__((unused))) { if (local_kb) lv_obj_add_flag(local_kb, LV_OBJ_FLAG_HIDDEN); }
static void screen_click_cb(lv_event_t * e) { if (lv_event_get_target(e) == ui_Screen1) hide_kb(e); }
static void goto_register(lv_event_t * e __attribute__((unused))) { ui_load_register_screen(); }
static void login_click_cb(lv_event_t * e __attribute__((unused))) {
    const char * user = lv_textarea_get_text(ui_username_ta);
    const char * pwd = lv_textarea_get_text(ui_password_ta);
    if (strlen(user) == 0 || strlen(pwd) == 0) {
        lv_label_set_text(ui_status_label, "Please enter username and password");
        lv_obj_set_style_text_color(ui_status_label, lv_color_hex(0xFF5252), 0);
        return;
    }
    hide_kb(e);
    ui_load_main_menu();
}

void ui_Screen1_screen_init(void)
{
    ui_Screen1 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_Screen1, lv_color_black(), 0);
    lv_obj_set_style_pad_all(ui_Screen1, 0, 0);

    lv_obj_t * bg = lv_image_create(ui_Screen1);
    lv_image_set_src(bg, &bg_login_img);
    lv_obj_align(bg, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t * title = lv_label_create(ui_Screen1);
    lv_label_set_text(title, "User Login");
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_28, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 80);

    lv_obj_t * card = lv_obj_create(ui_Screen1);
    lv_obj_set_size(card, 460, 230);
    lv_obj_align(card, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_shadow_width(card, 0, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * ul = lv_label_create(card);
    lv_label_set_text(ul, "Username");
    lv_obj_set_style_text_color(ul, lv_color_hex(0x555555), 0);
    lv_obj_set_style_text_font(ul, &lv_font_montserrat_12, 0);
    lv_obj_align(ul, LV_ALIGN_TOP_LEFT, 20, 12);

    ui_username_ta = lv_textarea_create(card);
    lv_obj_set_size(ui_username_ta, 420, 36);
    lv_obj_align(ui_username_ta, LV_ALIGN_TOP_MID, 0, 34);
    lv_textarea_set_placeholder_text(ui_username_ta, "Enter username");
    lv_textarea_set_one_line(ui_username_ta, true);
    lv_textarea_set_max_length(ui_username_ta, 32);
    lv_obj_set_style_radius(ui_username_ta, 6, 0);
    lv_obj_set_style_border_color(ui_username_ta, lv_color_hex(0xCCCCCC), 0);
    lv_obj_set_style_border_width(ui_username_ta, 1, 0);
    lv_obj_set_style_pad_left(ui_username_ta, 10, 0);
    lv_obj_set_style_text_font(ui_username_ta, &lv_font_montserrat_14, 0);
    lv_obj_set_style_border_color(ui_username_ta, lv_color_hex(0x1A237E), LV_STATE_FOCUSED);
    lv_obj_add_event_cb(ui_username_ta, ta_focus_cb, LV_EVENT_FOCUSED, NULL);

    lv_obj_t * pl = lv_label_create(card);
    lv_label_set_text(pl, "Password");
    lv_obj_set_style_text_color(pl, lv_color_hex(0x555555), 0);
    lv_obj_set_style_text_font(pl, &lv_font_montserrat_12, 0);
    lv_obj_align(pl, LV_ALIGN_TOP_LEFT, 20, 78);

    ui_password_ta = lv_textarea_create(card);
    lv_obj_set_size(ui_password_ta, 420, 36);
    lv_obj_align(ui_password_ta, LV_ALIGN_TOP_MID, 0, 100);
    lv_textarea_set_placeholder_text(ui_password_ta, "Enter password");
    lv_textarea_set_one_line(ui_password_ta, true);
    lv_textarea_set_password_mode(ui_password_ta, true);
    lv_textarea_set_max_length(ui_password_ta, 32);
    lv_obj_set_style_radius(ui_password_ta, 6, 0);
    lv_obj_set_style_border_color(ui_password_ta, lv_color_hex(0xCCCCCC), 0);
    lv_obj_set_style_border_width(ui_password_ta, 1, 0);
    lv_obj_set_style_pad_left(ui_password_ta, 10, 0);
    lv_obj_set_style_text_font(ui_password_ta, &lv_font_montserrat_14, 0);
    lv_obj_set_style_border_color(ui_password_ta, lv_color_hex(0x1A237E), LV_STATE_FOCUSED);
    lv_obj_add_event_cb(ui_password_ta, ta_focus_cb, LV_EVENT_FOCUSED, NULL);

    ui_login_btn = lv_btn_create(card);
    lv_obj_set_size(ui_login_btn, 420, 38);
    lv_obj_align(ui_login_btn, LV_ALIGN_TOP_MID, 0, 148);
    lv_obj_set_style_bg_color(ui_login_btn, lv_color_hex(0x1A237E), 0);
    lv_obj_set_style_radius(ui_login_btn, 6, 0);
    lv_obj_add_event_cb(ui_login_btn, login_click_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * bl = lv_label_create(ui_login_btn);
    lv_label_set_text(bl, "LOGIN");
    lv_obj_set_style_text_color(bl, lv_color_white(), 0);
    lv_obj_set_style_text_font(bl, &lv_font_montserrat_18, 0);
    lv_obj_center(bl);

    lv_obj_t * reg_link = lv_label_create(card);
    lv_label_set_text(reg_link, "No account? Register");
    lv_obj_set_style_text_color(reg_link, lv_color_hex(0x1A237E), 0);
    lv_obj_set_style_text_font(reg_link, &lv_font_montserrat_12, 0);
    lv_obj_align(reg_link, LV_ALIGN_BOTTOM_MID, 0, -4);
    lv_obj_add_flag(reg_link, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(reg_link, goto_register, LV_EVENT_CLICKED, NULL);

    ui_status_label = lv_label_create(ui_Screen1);
    lv_label_set_text(ui_status_label, "");
    lv_obj_set_style_text_font(ui_status_label, &lv_font_montserrat_14, 0);
    lv_obj_align(ui_status_label, LV_ALIGN_BOTTOM_MID, 0, -45);

    lv_obj_add_event_cb(ui_Screen1, screen_click_cb, LV_EVENT_CLICKED, NULL);
    local_kb = lv_keyboard_create(ui_Screen1);
    lv_obj_set_size(local_kb, 1024, 250);
    lv_obj_align(local_kb, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_flag(local_kb, LV_OBJ_FLAG_HIDDEN);
    lv_keyboard_set_textarea(local_kb, ui_username_ta);
}
void ui_Screen1_screen_destroy(void) { if (ui_Screen1) lv_obj_del(ui_Screen1); ui_Screen1 = NULL; local_kb = NULL; }
