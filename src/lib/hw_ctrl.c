#include "hw_ctrl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

static int sysfs_write(const char *path, const char *value)
{
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        fprintf(stderr, "[HW] FAIL open %s: %s\n", path, strerror(errno));
        return -1;
    }
    ssize_t len = strlen(value);
    ssize_t ret = write(fd, value, len);
    close(fd);
    if (ret != len) {
        fprintf(stderr, "[HW] FAIL write %s: %s\n", path, strerror(errno));
        return -1;
    }
    return 0;
}

static int gpio_export(int pin)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", pin);
    int r = sysfs_write("/sys/class/gpio/export", buf);
    if (r != 0) fprintf(stderr, "[HW] GPIO%d already exported or busy\n", pin);
    else fprintf(stderr, "[HW] GPIO%d exported\n", pin);
    return r;
}

static int gpio_set_direction(int pin, const char *dir)
{
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
    return sysfs_write(path, dir);
}

static int gpio_set_value(int pin, int value)
{
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
    int r = sysfs_write(path, value ? "1" : "0");
    if (r != 0) fprintf(stderr, "[HW] FAIL set GPIO%d=%d\n", pin, value);
    return r;
}

static int gpio_unexport(int pin)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", pin);
    return sysfs_write("/sys/class/gpio/unexport", buf);
}

/* ---- state ---- */
static int led_pins[4]   = { GPIO_LED_0, GPIO_LED_1, GPIO_LED_2, GPIO_LED_3 };
static int led_states[4] = { 0, 0, 0, 0 };
static int buzzer_state  = 0;
static int backl_state   = 0;
static int backl_brightness = 100;
static int backl_saved   = 100;

int hardware_init(void)
{
    fprintf(stderr, "[HW] === hardware_init start ===\n");
    for (int i = 0; i < 4; i++) {
        gpio_export(led_pins[i]);
        usleep(100000);
        gpio_set_direction(led_pins[i], "out");
        gpio_set_value(led_pins[i], 0);
        led_states[i] = 0;
        fprintf(stderr, "[HW] LED%d (GPIO%d) ready, val=0\n", i, led_pins[i]);
    }

    gpio_export(GPIO_BUZZER);
    usleep(100000);
    gpio_set_direction(GPIO_BUZZER, "out");
    gpio_set_value(GPIO_BUZZER, 0);
    buzzer_state = 0;
    fprintf(stderr, "[HW] Buzzer (GPIO%d) ready\n", GPIO_BUZZER);

    fprintf(stderr, "[HW] === hardware_init done ===\n");
    return 0;
}

void hardware_deinit(void)
{
    for (int i = 0; i < 4; i++) {
        gpio_set_value(led_pins[i], 0);
        gpio_unexport(led_pins[i]);
    }
    gpio_set_value(GPIO_BUZZER, 0);
    gpio_unexport(GPIO_BUZZER);
}

void led_on(int id)  { if (id >= 0 && id < 4) { int r = gpio_set_value(led_pins[id], 1); led_states[id]=1; fprintf(stderr,"[HW] LED%d ON (ret=%d)\n",id,r); } }
void led_off(int id) { if (id >= 0 && id < 4) { int r = gpio_set_value(led_pins[id], 0); led_states[id]=0; fprintf(stderr,"[HW] LED%d OFF (ret=%d)\n",id,r); } }
void led_toggle(int id) { led_states[id] ? led_off(id) : led_on(id); }
int  led_get_state(int id) { return (id >= 0 && id < 4) ? led_states[id] : 0; }

void buzzer_on(void)      { gpio_set_value(GPIO_BUZZER, 1); buzzer_state=1; fprintf(stderr,"[HW] Buzzer ON\n"); }
void buzzer_off(void)     { gpio_set_value(GPIO_BUZZER, 0); buzzer_state=0; fprintf(stderr,"[HW] Buzzer OFF\n"); }
void buzzer_toggle(void)  { buzzer_state ? buzzer_off() : buzzer_on(); }
int  buzzer_get_state(void) { return buzzer_state; }

void backlight_on(void)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", backl_saved);
    sysfs_write(BACKLIGHT_PATH, buf);
    backl_state = 1;
    fprintf(stderr, "[HW] Backlight ON (val=%d)\n", backl_saved);
}

void backlight_off(void)
{
    backl_saved = backl_brightness;
    sysfs_write(BACKLIGHT_PATH, "255");
    backl_state = 0;
    fprintf(stderr, "[HW] Backlight OFF\n");
}

void backlight_set_brightness(int pct)
{
    if (pct < 0) pct = 0;
    if (pct > 100) pct = 100;
    backl_brightness = pct;
    int raw = (pct * 255) / 100;
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", raw);
    sysfs_write(BACKLIGHT_PATH, buf);
    fprintf(stderr, "[HW] Backlight brightness=%d%% raw=%d\n", pct, raw);
}

int backlight_get_brightness(void) { return backl_brightness; }
int backlight_get_state(void)      { return backl_state; }
