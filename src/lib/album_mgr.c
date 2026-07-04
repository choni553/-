#include "album_mgr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
static char files[256][256];static int count=0,idx=0;static char base[256]="/root/ljr/album";
static int cmp(const void *a,const void *b){return strcmp((const char*)a,(const char*)b);}
void album_scan(const char *d){count=0;idx=0;if(d)strncpy(base,d,sizeof(base)-1);
    DIR *dp=opendir(base);if(!dp)return;struct dirent *de;
    while((de=readdir(dp))&&count<256){if(strstr(de->d_name,".bmp")||strstr(de->d_name,".jpg")||strstr(de->d_name,".png")){snprintf(files[count],sizeof(files[0]),"%s/%s",base,de->d_name);count++;}}closedir(dp);qsort(files,count,sizeof(files[0]),cmp);}
int album_count(void){return count;}
const char *album_get(int i){return(i>=0&&i<count)?files[i]:NULL;}
void album_prev(void){if(count>0){idx--;if(idx<0)idx=count-1;}}
void album_next(void){if(count>0){idx++;if(idx>=count)idx=0;}}
int album_get_index(void){return idx;}
const char *album_current(void){return(count>0&&idx<count)?files[idx]:NULL;}
void album_delete_current(void){if(count>0&&idx<count){unlink(files[idx]);for(int i=idx;i<count-1;i++)strcpy(files[i],files[i+1]);count--;if(idx>=count&&count>0)idx=count-1;}}
