#ifndef util_INI_h
#define util_INI_h 1

typedef struct ini_s ini_t;

ini_t *ini_load(char *name);
void ini_free(ini_t *ini);

void ini_rewind(ini_t *ini);
int ini_next(ini_t *ini);
int ini_seek_section(ini_t *ini, char *section);
int ini_seek_setting(ini_t *ini, char *setting);

char *ini_get_setting(ini_t *ini, char *section, char *setting);

char *ini_section(ini_t *ini);
char *ini_key(ini_t *ini);
char *ini_value(ini_t *ini);

#endif
