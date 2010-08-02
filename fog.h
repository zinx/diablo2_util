#ifndef FOG_DLL_h
#define FOG_DLL_h 1

#define FOG_DLL __declspec(dllimport)

FOG_DLL void __cdecl FogLogDebug(char *fmt, ...) asm("Fog_10029") __attribute__((format(printf,1,2)));

#endif
