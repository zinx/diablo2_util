#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include "util.h"

static char *stpcpy(char *dest, const char *src)
{
	while ((*dest = *src++))
		++dest;
	return dest;
}

struct ini_s {
	char *buffer, *pos, *end;
	char *section, *key, *value;
};

static char *ini_eol(ini_t *ini, char *p)
{
	return p + strcspn(p, "\r\n");
}

static char *ini_parse_section(ini_t *ini, char *p)
{
	char *e = p;
	e += strcspn(e, "\r\n]");
	if (*e != ']') return ini_eol(ini, e);
	*e++ = '\0';
	ini->pos = stpcpy(ini->pos, p) + 1;
	*ini->pos++ = '\n';
	*ini->pos++ = '\0';
	return e;
}

static char *ini_parse_string(ini_t *ini, char *p, const char *expect)
{
	char *e = p;

	if (*e == '"') {
		for (++e; *e && *e != '"'; ++e) {
			switch (*e) {
			case '\r': case '\n':
				return e;
			case '\\':
				if (e[1]) {
					*ini->pos++ = e[1];
					++e;
				}
				continue;
			default:
				*ini->pos++ = *e;
			}
		}
		*ini->pos++ = '\0';
		if (*e != '"') { return ini_eol(ini, e); }
		++e;
		e += strspn(e, " \t");
	} else {
		char *es, ch;
		e += strcspn(e, expect);
		for (es = e; es > p && (es[-1] == ' ' || es[-1] == '\t'); --es)
			;
		ch = *es;
		*es = '\0';
		ini->pos = stpcpy(ini->pos, p) + 1;
		*es = ch;
	}

	return e;
}


static char *ini_parse_setting(ini_t *ini, char *p)
{
	char *orig_pos = ini->pos;
	char *key = ini->pos;

	p = ini_parse_string(ini, p, "\r\n=");
	if (*p != '=' || !*key) {
		ini->pos = orig_pos;
		return ini_eol(ini, p);
	}
	++p;

	p += strspn(p, " \t");

	p = ini_parse_string(ini, p, "\r\n");
	if ((*p && *p != '\r' && *p != '\n')) {
		ini->pos = orig_pos;
		return ini_eol(ini, p);
	}

	return ini_eol(ini, p);
}

ini_t *ini_load(char *name)
{
	char *buffer, *p;
	ini_t *ini;
	HANDLE file;

	if (!SFileOpenFile(name, &file)) {
		int fd;

		fd = open(name, O_RDONLY);
		if (fd < 0) {
			char path[MAX_PATH], *game_path;
			game_path = util_game_path();
			snprintf(path, sizeof(path), "%s/%s", game_path, path);
			fd = open(path, O_RDONLY);
		}

		if (fd < 0) return NULL;

		off_t size = lseek(fd, 0, SEEK_END);
		lseek(fd, 0, SEEK_SET);
		buffer = malloc(size + 1);
		read(fd, buffer, size);
		close(fd);

		buffer[size] = '\0';
	} else {
		DWORD size = SFileGetFileSize(file, NULL);
		buffer = malloc(size + 1);
		SFileReadFile(file, buffer, size, &size, 0);
		SFileCloseFile(file);
		buffer[size] = '\0';
	}

	ini = malloc(sizeof(*ini));
	ini->buffer = malloc(strlen(buffer) + 1);
	ini->pos = ini->buffer;

	for (p = buffer; *p; ++p) {
		p += strspn(p, " \t\r\n");
		if (*p == ';') { p = ini_eol(ini, p+1); continue; }
		if (*p == '[') { p = ini_parse_section(ini, p+1); continue; }
		p = ini_parse_setting(ini, p);
	}

	ini->end = ini->pos;
	ini->pos = ini->buffer;

	free(buffer);

	return ini;
}

void ini_free(ini_t *ini)
{
	free(ini->buffer);
	free(ini);
}

void ini_rewind(ini_t *ini)
{
	ini->pos = ini->buffer;
	ini->section = "";
	ini->key = NULL;
	ini->value = NULL;
}

int ini_next(ini_t *ini)
{
	char *key, *value;

	for (;;) {
		if (ini->pos >= ini->end) return 0;

		key = ini->pos;
		value = key + strlen(key) + 1;
		ini->pos = value + strlen(value) + 1;

		if (*value != '\n')
			break;

		ini->section = key;
	}

	ini->key = key;
	ini->value = value;
	return 1;
}

int ini_seek_section(ini_t *ini, char *section)
{
	char *key, *value;

	for (;;) {
		if (ini->pos >= ini->end) return 0;

		key = ini->pos;
		value = key + strlen(key) + 1;

		if (*value != '\n') {
			ini->key = key;
			ini->value = value;

			if (!section && ini->pos == ini->buffer) {
				ini->section = "";
				return 1;
			}

			ini->pos = value + strlen(value) + 1;
			continue;
		}

		ini->pos = value + strlen(value) + 1;
		ini->section = key;

		if (!section || strcasecmp(key, section) == 0)
			break;
	}

	return 1;
}

int ini_seek_setting(ini_t *ini, char *setting)
{
	char *key, *value;

	for (;;) {
		if (ini->pos >= ini->end) return 0;

		key = ini->pos;
		value = key + strlen(key) + 1;
		ini->pos = value + strlen(value) + 1;

		if (*value != '\n') {
			ini->key = key;
			ini->value = value;
			if (!setting || strcasecmp(key, setting) == 0)
				return 1;
			continue;
		}

		ini->section = key;

		return 0;
	}

	/* NOT REACHED */
}

char *ini_get_setting(ini_t *ini, char *section, char *setting)
{
	ini_rewind(ini);

	while (ini_seek_section(ini, section)) {
		if (ini_seek_setting(ini, setting))
			return ini->value;
	}

	return NULL;
}

char *ini_section(ini_t *ini) { return ini->section; }
char *ini_key(ini_t *ini) { return ini->key; }
char *ini_value(ini_t *ini) { return ini->value; }
