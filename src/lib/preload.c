#include "preload.h"
#include <stdio.h>
#include <pthread.h>

static volatile int progress = 0;
static volatile int done = 0;
static pthread_t thread;

/* CB is called from background thread — MUST be LVGL-safe or empty.
   The splash screen timer polls preload_get_progress() in the LVGL thread. */
static void *preload_thread(void *arg)
{
    (void)arg;
    const int stages[][2] = {{0,30},{30,70},{70,90},{90,100}};
    for (int s = 0; s < 4 && !done; s++) {
        int duration = 1500000;
        int start = stages[s][0], end = stages[s][1];
        for (int t = 0; t < 20 && !done; t++) {
            progress = start + (end - start) * t / 20;
            usleep(duration / 20);
        }
    }
    if (!done) { progress = 100; done = 1; }
    return NULL;
}

void preload_init(void)
{
    progress = 0;
    done = 0;
    pthread_create(&thread, NULL, preload_thread, NULL);
}

int preload_get_progress(void) { return progress; }
int preload_is_done(void)      { return done || progress >= 100; }

void preload_force_finish(void)
{
    done = 1;
    progress = 100;
}
