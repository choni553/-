#include "camera.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/videodev2.h>

#define W 640
#define H 480

static unsigned char simbuf[W*H*2];
static int cam_fd = -1, started = 0;
static struct { void *start; int len; } bufs[4];
static struct v4l2_buffer vbuf;
static enum v4l2_buf_type vtype;

/* ---- YUYV to RGB565 ---- */
static void yuyv2rgb565(unsigned char *in, unsigned char *out) {
    unsigned short *dst = (unsigned short *)out;
    for (int i = 0; i < W*H/2; i++) {
        int y0 = in[0], u = in[1]-128, y1 = in[2], v = in[3]-128; in += 4;
        int rv = 359*v, guv = -88*u - 183*v, bu = 454*u;
        int r = (y0*256 + rv)>>8, g = (y0*256 + guv)>>8, b = (y0*256 + bu)>>8;
        if (r>255)r=255; else if (r<0)r=0;
        if (g>255)g=255; else if (g<0)g=0;
        if (b>255)b=255; else if (b<0)b=0;
        *dst++ = ((r>>3)<<11)|((g>>2)<<5)|(b>>3);
        r = (y1*256 + rv)>>8, g = (y1*256 + guv)>>8, b = (y1*256 + bu)>>8;
        if (r>255)r=255; else if (r<0)r=0;
        if (g>255)g=255; else if (g<0)g=0;
        if (b>255)b=255; else if (b<0)b=0;
        *dst++ = ((r>>3)<<11)|((g>>2)<<5)|(b>>3);
    }
}

/* ---- YUYV to RGB888 (for BMP) ---- */
static void yuyv2rgb(unsigned char *in, unsigned char *out) {
    for (unsigned i = 0; i < W*H*2; i += 4) {
        int y0 = in[i], u = in[i+1]-128, y1 = in[i+2], v = in[i+3]-128;
        for (int j = 0; j < 2; j++) {
            int yy = j ? y1 : y0;
            int r = yy + ((1436*v)>>10), g = yy - ((352*u+731*v)>>10), b = yy + ((1814*u)>>10);
            if (r>255)r=255; else if (r<0)r=0;
            if (g>255)g=255; else if (g<0)g=0;
            if (b>255)b=255; else if (b<0)b=0;
            *out++ = r; *out++ = g; *out++ = b;
        }
    }
}

/* ---- BMP writer ---- */
#pragma pack(push,1)
typedef struct { unsigned short bfType; unsigned bfSize; unsigned short bfReserved1,bfReserved2; unsigned bfOffBits; } BFH;
typedef struct { unsigned biSize; int biWidth,biHeight; unsigned short biPlanes,biBitCount; unsigned biCompression,biSizeImage; int biXPelsPerMeter,biYPelsPerMeter; unsigned biClrUsed,biClrImportant; } BIH;
#pragma pack(pop)

static int save_bmp(unsigned char *rgb, const char *path) {
    BFH fh; BIH ih; memset(&fh,0,sizeof(fh)); memset(&ih,0,sizeof(ih));
    fh.bfType=0x4D42; fh.bfSize=sizeof(BFH)+sizeof(BIH)+W*H*3; fh.bfOffBits=sizeof(BFH)+sizeof(BIH);
    ih.biSize=sizeof(BIH); ih.biWidth=W; ih.biHeight=-H; ih.biPlanes=1; ih.biBitCount=24;
    static unsigned char row[W*3+4] __attribute__((aligned(4)));
    FILE *f = fopen(path,"wb"); if (!f) return -1;
    fwrite(&fh,sizeof(fh),1,f); fwrite(&ih,sizeof(ih),1,f);
    for (int y = 0; y < H; y++) {
        int src_off = ((H-1-y)*W*3);
        for (int x = 0; x < W; x++) { int si = src_off+x*3, di = x*3; row[di] = rgb[si+2]; row[di+1] = rgb[si+1]; row[di+2] = rgb[si]; }
        fwrite(row, W*3, 1, f);
    }
    fclose(f); return 0;
}

/* ========== Public API ========== */
int Camera_Init(char *dev) {
    cam_fd = open(dev, O_RDWR);
    if (cam_fd < 0) { perror("camera: open"); return -1; }
    printf("camera: opened %s fd=%d\n", dev, cam_fd);
    struct v4l2_format fmt; memset(&fmt,0,sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; fmt.fmt.pix.width=W; fmt.fmt.pix.height=H; fmt.fmt.pix.pixelformat=V4L2_PIX_FMT_YUYV;
    ioctl(cam_fd, VIDIOC_S_FMT, &fmt);
    printf("camera: fmt YUYV %ux%u\n", fmt.fmt.pix.width, fmt.fmt.pix.height);
    struct v4l2_requestbuffers req; memset(&req,0,sizeof(req));
    req.count=4; req.type=V4L2_BUF_TYPE_VIDEO_CAPTURE; req.memory=V4L2_MEMORY_MMAP;
    ioctl(cam_fd, VIDIOC_REQBUFS, &req);
    for (int i = 0; i < 4; i++) {
        memset(&vbuf,0,sizeof(vbuf)); vbuf.index=i; vbuf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE; vbuf.memory=V4L2_MEMORY_MMAP;
        ioctl(cam_fd, VIDIOC_QUERYBUF, &vbuf);
        bufs[i].len = vbuf.length;
        bufs[i].start = mmap(NULL, vbuf.length, PROT_READ|PROT_WRITE, MAP_SHARED, cam_fd, vbuf.m.offset);
        ioctl(cam_fd, VIDIOC_QBUF, &vbuf);
    }
    return 0;
}

int Camera_Start(void) { vtype = V4L2_BUF_TYPE_VIDEO_CAPTURE; int r = ioctl(cam_fd,VIDIOC_STREAMON,&vtype); if(r==0)started=1; return r; }

int Camera_Show(void) {
    if (!started) return 0;
    fd_set fds; FD_ZERO(&fds); FD_SET(cam_fd, &fds); struct timeval tv={0,30000};
    if (select(cam_fd+1, &fds, NULL, NULL, &tv) <= 0) return 0;
    memset(&vbuf,0,sizeof(vbuf)); vbuf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE; vbuf.memory=V4L2_MEMORY_MMAP;
    if (ioctl(cam_fd, VIDIOC_DQBUF, &vbuf) < 0) return -1;
    yuyv2rgb565((unsigned char *)bufs[vbuf.index].start, simbuf);
    ioctl(cam_fd, VIDIOC_QBUF, &vbuf);
    return 0;
}

int Camera_Quit(void) {
    if (!started) return 0;
    ioctl(cam_fd, VIDIOC_STREAMOFF, &vtype); started = 0;
    for (int i = 0; i < 4; i++) munmap(bufs[i].start, bufs[i].len);
    close(cam_fd); cam_fd = -1; return 0;
}

int Camera_Get_Pic(char *bmp_name) {
    memset(&vbuf,0,sizeof(vbuf)); vbuf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE; vbuf.memory=V4L2_MEMORY_MMAP;
    if (ioctl(cam_fd, VIDIOC_DQBUF, &vbuf) < 0) { printf("camera: DQBUF fail\n"); return -1; }
    static unsigned char rgb[W*H*3]; yuyv2rgb((unsigned char *)bufs[vbuf.index].start, rgb);
    save_bmp(rgb, bmp_name);
    ioctl(cam_fd, VIDIOC_QBUF, &vbuf);
    return 0;
}

unsigned char *Camera_Get_Buffer(void) { return simbuf; }
int Camera_Is_Started(void) { return started; }
