#include "v4l2_capture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
struct v4l2_buf{void *start;size_t length;};
struct v4l2_ctx{int fd;unsigned int w,h,nbufs;struct v4l2_buf*bufs;uint32_t pf;};
v4l2_ctx_t *v4l2_open(const char*d,unsigned int w,unsigned int h,unsigned int n){
    v4l2_ctx_t*c=calloc(1,sizeof(*c));c->fd=-1;c->fd=open(d,O_RDWR);if(c->fd<0){perror("v4l2:open");free(c);return NULL;}
    struct v4l2_capability cap;ioctl(c->fd,VIDIOC_QUERYCAP,&cap);
    printf("v4l2: driver=%s card=%s\n",cap.driver,cap.card);
    struct v4l2_format fmt;memset(&fmt,0,sizeof(fmt));
    fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;fmt.fmt.pix.width=w;fmt.fmt.pix.height=h;
    fmt.fmt.pix.pixelformat=V4L2_PIX_FMT_YUYV;fmt.fmt.pix.field=V4L2_FIELD_ANY;
    if(ioctl(c->fd,VIDIOC_S_FMT,&fmt)<0){perror("v4l2:S_FMT");goto fail;}
    c->w=fmt.fmt.pix.width;c->h=fmt.fmt.pix.height;c->pf=fmt.fmt.pix.pixelformat;
    printf("v4l2: fmt=%c%c%c%c %ux%u\n",(char)(c->pf&0xFF),(char)((c->pf>>8)&0xFF),(char)((c->pf>>16)&0xFF),(char)((c->pf>>24)&0xFF),c->w,c->h);
    struct v4l2_requestbuffers req;memset(&req,0,sizeof(req));
    req.count=n;req.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;req.memory=V4L2_MEMORY_MMAP;
    if(ioctl(c->fd,VIDIOC_REQBUFS,&req)<0){perror("v4l2:REQBUFS");goto fail;}
    c->nbufs=req.count;c->bufs=calloc(c->nbufs,sizeof(struct v4l2_buf));printf("v4l2: %u buffers\n",c->nbufs);
    for(unsigned i=0;i<c->nbufs;i++){struct v4l2_buffer buf;memset(&buf,0,sizeof(buf));
        buf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;buf.memory=V4L2_MEMORY_MMAP;buf.index=i;
        if(ioctl(c->fd,VIDIOC_QUERYBUF,&buf)<0){perror("v4l2:QUERYBUF");goto fail;}
        c->bufs[i].length=buf.length;c->bufs[i].start=mmap(NULL,buf.length,PROT_READ|PROT_WRITE,MAP_SHARED,c->fd,buf.m.offset);
        if(c->bufs[i].start==MAP_FAILED){perror("v4l2:mmap");goto fail;}
        if(ioctl(c->fd,VIDIOC_QBUF,&buf)<0){perror("v4l2:QBUF");goto fail;}}
    return c;fail:v4l2_close(c);return NULL;}
int v4l2_start(v4l2_ctx_t*c){enum v4l2_buf_type t=V4L2_BUF_TYPE_VIDEO_CAPTURE;return ioctl(c->fd,VIDIOC_STREAMON,&t)<0?(perror("v4l2:STREAMON"),-1):0;}
int v4l2_grab(v4l2_ctx_t*c,void**d,size_t*s){struct v4l2_buffer buf;memset(&buf,0,sizeof(buf));buf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;buf.memory=V4L2_MEMORY_MMAP;if(ioctl(c->fd,VIDIOC_DQBUF,&buf)<0){perror("v4l2:DQBUF");return-1;}*d=c->bufs[buf.index].start;*s=buf.bytesused;return buf.index;}
int v4l2_release(v4l2_ctx_t*c,int i){struct v4l2_buffer buf;memset(&buf,0,sizeof(buf));buf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;buf.memory=V4L2_MEMORY_MMAP;buf.index=i;return ioctl(c->fd,VIDIOC_QBUF,&buf)<0?(perror("v4l2:QBUF"),-1):0;}
int v4l2_stop(v4l2_ctx_t*c){enum v4l2_buf_type t=V4L2_BUF_TYPE_VIDEO_CAPTURE;return ioctl(c->fd,VIDIOC_STREAMOFF,&t)<0?(perror("v4l2:STREAMOFF"),-1):0;}
void v4l2_close(v4l2_ctx_t*c){if(!c)return;if(c->bufs)for(unsigned i=0;i<c->nbufs;i++)if(c->bufs[i].start&&c->bufs[i].start!=MAP_FAILED)munmap(c->bufs[i].start,c->bufs[i].length);free(c->bufs);if(c->fd>=0)close(c->fd);free(c);}
unsigned v4l2_get_width(v4l2_ctx_t*c){return c?c->w:0;}unsigned v4l2_get_height(v4l2_ctx_t*c){return c?c->h:0;}
int v4l2_frame_ready(v4l2_ctx_t*c,int ms){struct pollfd p;p.fd=c->fd;p.events=POLLIN;int r=poll(&p,1,ms);return r<0?(perror("v4l2:poll"),-1):(r>0&&(p.revents&POLLIN))?1:0;}
#define CL(v)((unsigned char)((v)>255?255:((v)<0?0:(v))))
/* Full-range BT.601 for USB cameras (JFIF standard) */
void v4l2_yuyv_to_rgb888(const uint8_t*y,uint8_t*r,unsigned w,unsigned h){
    for(unsigned i=0;i<w*h;i+=2){int y0=y[0],u=y[1]-128,y1=y[2],v=y[3]-128;y+=4;
        r[0]=CL(y0+((1436*v)>>10));r[1]=CL(y0-((352*u+731*v)>>10));r[2]=CL(y0+((1814*u)>>10));r+=3;
        r[0]=CL(y1+((1436*v)>>10));r[1]=CL(y1-((352*u+731*v)>>10));r[2]=CL(y1+((1814*u)>>10));r+=3;}}
void v4l2_uyvy_to_rgb888(const uint8_t*y,uint8_t*r,unsigned w,unsigned h){
    for(unsigned i=0;i<w*h;i+=2){int u=y[0]-128,y0=y[1],v=y[2]-128,y1=y[3];y+=4;
        r[0]=CL(y0+((1436*v)>>10));r[1]=CL(y0-((352*u+731*v)>>10));r[2]=CL(y0+((1814*u)>>10));r+=3;
        r[0]=CL(y1+((1436*v)>>10));r[1]=CL(y1-((352*u+731*v)>>10));r[2]=CL(y1+((1814*u)>>10));r+=3;}}
void v4l2_yuyv_to_xrgb8888(const uint8_t*y,uint8_t*x,unsigned w,unsigned h){
    for(unsigned i=0;i<w*h;i+=2){int y0=y[0],u=y[1]-128,y1=y[2],v=y[3]-128;y+=4;
        x[0]=CL(y0+((1814*u)>>10));x[1]=CL(y0-((352*u+731*v)>>10));x[2]=CL(y0+((1436*v)>>10));x[3]=0xFF;x+=4;
        x[0]=CL(y1+((1814*u)>>10));x[1]=CL(y1-((352*u+731*v)>>10));x[2]=CL(y1+((1436*v)>>10));x[3]=0xFF;x+=4;}}
void v4l2_uyvy_to_xrgb8888(const uint8_t*y,uint8_t*x,unsigned w,unsigned h){
    for(unsigned i=0;i<w*h;i+=2){int u=y[0]-128,y0=y[1],v=y[2]-128,y1=y[3];y+=4;
        x[0]=CL(y0+((1814*u)>>10));x[1]=CL(y0-((352*u+731*v)>>10));x[2]=CL(y0+((1436*v)>>10));x[3]=0xFF;x+=4;
        x[0]=CL(y1+((1814*u)>>10));x[1]=CL(y1-((352*u+731*v)>>10));x[2]=CL(y1+((1436*v)>>10));x[3]=0xFF;x+=4;}}
#undef CL
