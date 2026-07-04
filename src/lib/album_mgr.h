#ifndef ALBUM_MGR_H
#define ALBUM_MGR_H
int album_count(void);
const char *album_get(int idx);
void album_prev(void);
void album_next(void);
int  album_get_index(void);
const char *album_current(void);
void album_scan(const char *dir);
void album_delete_current(void);
#endif
