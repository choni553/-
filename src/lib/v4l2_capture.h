/**
 * @file v4l2_capture.h
 * V4L2 camera capture module - header
 *
 * Supports V4L2 Multiplanar API (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE)
 * with UYVY / YUYV packed YUV 4:2:2 pixel formats.
 */

#ifndef V4L2_CAPTURE_H
#define V4L2_CAPTURE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

typedef struct v4l2_ctx v4l2_ctx_t;

/**
 * Open and initialize a V4L2 capture device (multiplanar API).
 * @param device   Device node path, e.g. "/dev/video-camera0"
 * @param width    Desired frame width
 * @param height   Desired frame height
 * @param nbufs    Number of kernel buffers to request (typ. 2-4)
 * @return Context pointer on success, NULL on failure.
 */
v4l2_ctx_t *v4l2_open(const char *device, unsigned int width,
                      unsigned int height, unsigned int nbufs);

int v4l2_start(v4l2_ctx_t *ctx);
int v4l2_stop(v4l2_ctx_t *ctx);
void v4l2_close(v4l2_ctx_t *ctx);

/**
 * Dequeue a filled frame buffer (VIDIOC_DQBUF, blocking).
 * @return Buffer index on success, -1 on error.
 */
int v4l2_grab(v4l2_ctx_t *ctx, void **data, size_t *size);

/**
 * Re-queue a buffer back to the driver (VIDIOC_QBUF).
 */
int v4l2_release(v4l2_ctx_t *ctx, int index);

unsigned int v4l2_get_width(v4l2_ctx_t *ctx);
unsigned int v4l2_get_height(v4l2_ctx_t *ctx);

/**
 * Non-blocking check if a frame is ready to dequeue (uses poll()).
 * @return 1=ready, 0=timeout, -1=error.
 */
int v4l2_frame_ready(v4l2_ctx_t *ctx, int timeout_ms);

/* ---- pixel format conversion ---- */

/** Convert YUYV (Y0 U0 Y1 V0) to RGB888 */
void v4l2_yuyv_to_rgb888(const uint8_t *yuyv, uint8_t *rgb,
                         unsigned int width, unsigned int height);

/** Convert UYVY (U0 Y0 V0 Y1) to RGB888 */
void v4l2_uyvy_to_rgb888(const uint8_t *uyvy, uint8_t *rgb,
                         unsigned int width, unsigned int height);

/** Convert YUYV to XRGB8888 (little-endian: [B,G,R,0xFF]) */
void v4l2_yuyv_to_xrgb8888(const uint8_t *yuyv, uint8_t *xrgb,
                           unsigned int width, unsigned int height);

/** Convert UYVY to XRGB8888 (little-endian: [B,G,R,0xFF]) */
void v4l2_uyvy_to_xrgb8888(const uint8_t *uyvy, uint8_t *xrgb,
                           unsigned int width, unsigned int height);

#ifdef __cplusplus
}
#endif

#endif /* V4L2_CAPTURE_H */
