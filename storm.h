#ifndef STORM_DLL_h
#define STORM_DLL_h 1

#define STORM_DLL	__declspec(dllimport)

STORM_DLL BOOL __stdcall SFileOpenArchive(char *archivename, DWORD dwPriority, DWORD dwFlags, HANDLE *handle) asm("Storm_266");
STORM_DLL BOOL __stdcall SFileCloseArchive(HANDLE hArchive) asm("Storm_252");
STORM_DLL BOOL __stdcall SFileGetBasePath(char *result, int maxSize) asm("Storm_273");
STORM_DLL BOOL __stdcall SFileReadFile(HANDLE hFile, void *buffer, DWORD nNumberOfBytesToRead, DWORD *read, LONG lpDistanceToMoveHigh) asm("Storm_269");
STORM_DLL DWORD __stdcall SFileGetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh) asm("Storm_265");
STORM_DLL BOOL __stdcall SFileOpenFile(char *filename, HANDLE *handle) asm("Storm_267");
STORM_DLL BOOL __stdcall SFileCloseFile(HANDLE hFile) asm("Storm_253");

#endif
