#ifndef AUTH_H
#define AUTH_H
#ifdef __cplusplus
extern "C" {
#endif

#define AUTH_MAX_USERS 50
#define AUTH_USER_LEN  32
#define AUTH_PASS_LEN  32

int  auth_init(void);
int  auth_login(const char *user, const char *pwd);
int  auth_register(const char *user, const char *pwd);
int  auth_user_exists(const char *user);
int  auth_get_count(void);
int  auth_change_password(const char *user, const char *old_pwd, const char *new_pwd);
void auth_save_remembered(const char *user, const char *pwd);
int  auth_get_remembered(char *user_out, char *pwd_out, int max_len);
void auth_clear_remembered(void);

#ifdef __cplusplus
}
#endif
#endif
