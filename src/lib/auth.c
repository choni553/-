#include "auth.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define DATA_DIR   "/root/ljr/data"
#define ACCT_FILE  DATA_DIR "/accounts.dat"
#define REMEM_FILE DATA_DIR "/remember.dat"
#define SALT_KEY   0xA3

static void xor_crypt(const char *in, char *out, int len)
{
    for (int i = 0; i < len; i++) out[i] = in[i] ^ (SALT_KEY + i % 7);
}

static int ensure_dir(void)
{
    mkdir(DATA_DIR, 0755);
    return 0;
}

int auth_init(void) { return ensure_dir(); }

int auth_login(const char *user, const char *pwd)
{
    if (!user || !pwd || !*user || !*pwd) return -1;
    FILE *f = fopen(ACCT_FILE, "r");
    if (!f) return -1;

    char line[AUTH_USER_LEN + AUTH_PASS_LEN + 4];
    int found = 0;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = 0;
        char *colon = strchr(line, ':');
        if (!colon) continue;
        *colon = 0;
        if (strcmp(line, user) != 0) { *colon = ':'; continue; }
        char crypted_hex[AUTH_PASS_LEN * 2 + 2] = {0};
        strncpy(crypted_hex, colon + 1, sizeof(crypted_hex) - 1);
        char crypted[AUTH_PASS_LEN] = {0};
        int clen = strlen(crypted_hex) / 2;
        for (int i = 0; i < clen; i++)
            sscanf(crypted_hex + i * 2, "%2hhx", (unsigned char *)&crypted[i]);
        char decrypted[AUTH_PASS_LEN] = {0};
        xor_crypt(crypted, decrypted, clen);
        found = (strcmp(pwd, decrypted) == 0);
        break;
    }
    fclose(f);
    return found ? 0 : -1;
}

int auth_user_exists(const char *user)
{
    if (!user || !*user) return 0;
    FILE *f = fopen(ACCT_FILE, "r");
    if (!f) return 0;
    char line[AUTH_USER_LEN + AUTH_PASS_LEN + 4];
    int found = 0;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = 0;
        char *colon = strchr(line, ':');
        if (!colon) continue;
        *colon = 0;
        if (strcmp(line, user) == 0) { found = 1; break; }
    }
    fclose(f);
    return found;
}

int auth_register(const char *user, const char *pwd)
{
    if (!user || !pwd || strlen(user) < 1 || strlen(pwd) < 1) return -1;
    if (auth_user_exists(user)) return -2;
    if (auth_get_count() >= AUTH_MAX_USERS) return -3;

    ensure_dir();
    FILE *f = fopen(ACCT_FILE, "a");
    if (!f) return -1;

    int plen = strlen(pwd);
    char crypted[AUTH_PASS_LEN] = {0};
    xor_crypt(pwd, crypted, plen);
    fprintf(f, "%s:", user);
    for (int i = 0; i < plen; i++)
        fprintf(f, "%02x", (unsigned char)crypted[i]);
    fprintf(f, "\n");
    fclose(f);
    return 0;
}

int auth_get_count(void)
{
    FILE *f = fopen(ACCT_FILE, "r");
    if (!f) return 0;
    int n = 0;
    char buf[256];
    while (fgets(buf, sizeof(buf), f)) n++;
    fclose(f);
    return n;
}

int auth_change_password(const char *user, const char *old_pwd, const char *new_pwd)
{
    if (auth_login(user, old_pwd) != 0) return -1;
    /* Simple approach: re-read all, replace matching line, re-write */
    FILE *f = fopen(ACCT_FILE, "r");
    if (!f) return -1;

    char lines[AUTH_MAX_USERS][AUTH_USER_LEN + AUTH_PASS_LEN + 4];
    int count = 0;
    while (fgets(lines[count], sizeof(lines[0]), f) && count < AUTH_MAX_USERS)
        count++;
    fclose(f);

    f = fopen(ACCT_FILE, "w");
    if (!f) return -1;

    int plen = strlen(new_pwd);
    char crypted[AUTH_PASS_LEN] = {0};
    xor_crypt(new_pwd, crypted, plen);
    char new_hash[AUTH_PASS_LEN * 2 + 2] = {0};
    for (int i = 0; i < plen; i++)
        sprintf(new_hash + i * 2, "%02x", (unsigned char)crypted[i]);

    for (int i = 0; i < count; i++) {
        char *colon = strchr(lines[i], ':');
        if (colon) { *colon = 0; if (strcmp(lines[i], user) == 0) {
            fprintf(f, "%s:%s\n", user, new_hash); continue;
        } *colon = ':'; }
        fputs(lines[i], f);
    }
    fclose(f);
    return 0;
}

/* ---- Remember me ---- */
void auth_save_remembered(const char *user, const char *pwd)
{
    ensure_dir();
    FILE *f = fopen(REMEM_FILE, "w");
    if (!f) return;

    int plen = strlen(pwd), ulen = strlen(user);
    char buf[AUTH_USER_LEN + AUTH_PASS_LEN + 4] = {0};
    snprintf(buf, sizeof(buf), "%s:", user);
    xor_crypt(pwd, buf + ulen + 1, plen);
    /* hex encode user+pass */
    for (int i = 0; i < ulen + 1 + plen; i++)
        fprintf(f, "%02x", (unsigned char)buf[i]);
    fclose(f);
}

int auth_get_remembered(char *user_out, char *pwd_out, int max_len)
{
    FILE *f = fopen(REMEM_FILE, "r");
    if (!f) return -1;
    char hex[256] = {0};
    if (!fgets(hex, sizeof(hex), f)) { fclose(f); return -1; }
    fclose(f);
    hex[strcspn(hex, "\r\n")] = 0;
    int hlen = strlen(hex) / 2;
    char raw[128] = {0};
    for (int i = 0; i < hlen && i < (int)sizeof(raw); i++)
        sscanf(hex + i * 2, "%2hhx", (unsigned char *)&raw[i]);
    char *colon = strchr(raw, ':');
    if (!colon) return -1;
    *colon = 0;
    strncpy(user_out, raw, max_len);
    int plen = hlen - (colon - raw) - 1;
    char decrypted[AUTH_PASS_LEN] = {0};
    xor_crypt(colon + 1, decrypted, plen);
    decrypted[plen] = 0;
    strncpy(pwd_out, decrypted, max_len);
    return 0;
}

void auth_clear_remembered(void) { unlink(REMEM_FILE); }
