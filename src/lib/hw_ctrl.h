#ifndef HW_CTRL_H
#define HW_CTRL_H
#ifdef __cplusplus
extern "C" {
#endif

/* LED GPIO pins (from reference) */
#define GPIO_LED_0  120
#define GPIO_LED_1  121
#define GPIO_LED_2  123
#define GPIO_LED_3  124

/* Backlight sysfs (from reference) */
#define BACKLIGHT_PATH "/sys/class/backlight/backlight/brightness"
#define BACKLIGHT_MAX  255

/* Buzzer GPIO (from reference) */
#define GPIO_BUZZER 111

int  hardware_init(void);
void hardware_deinit(void);

/* LED */
void led_on(int id);
void led_off(int id);
void led_toggle(int id);
int  led_get_state(int id);

/* Buzzer */
void buzzer_on(void);
void buzzer_off(void);
void buzzer_toggle(void);
int  buzzer_get_state(void);

/* Backlight */
void backlight_on(void);
void backlight_off(void);
void backlight_set_brightness(int pct);   /* 0-100 */
int  backlight_get_brightness(void);
int  backlight_get_state(void);

#ifdef __cplusplus
}
#endif
#endif
