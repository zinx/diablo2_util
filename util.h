#ifndef UTIL_h
#define UTIL_h 1

#include <windows.h>
#include "storm.h"
#include "fog.h"
#include "ini.h"

#define LOG(fmt,...) FogLogDebug("%s:%d:" fmt, __FILE__, __LINE__ , ## __VA_ARGS__ )

unsigned int util_csum_library(HMODULE lib);
char *util_game_path(void); /* path to D2Win.dll, technically. */

#endif
