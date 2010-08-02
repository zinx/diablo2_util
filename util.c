#include <string.h>
#include <libgen.h>

#include "util.h"

#define PE_MAGIC "PE\0\0" 

struct pe_hdr {
	char magic[4]; /* "PE\0\0" */
	WORD machine;
	WORD sections;
	DWORD timedatestamp;
	DWORD symbol_tab;
	DWORD symbols;
	WORD opt_header_size;
	WORD characteristics;
};

struct section_hdr {
	char name[8];
	DWORD virtual_size;
	DWORD virtual_address;
	DWORD size_of_data;
	DWORD raw_data;
	DWORD relocation_offset;
	DWORD line_number_offset;
	WORD relocations;
	WORD line_numbers;
	DWORD characteristics;
};

#define adler32_step(a,b,data) \
	do { \
		a = (a + (data)) % 65521; \
		b = (b + a) % 65521; \
	} while(0)

#define adler32_dword(a,b,dword) \
	do { \
		adler32_step(a,b,(dword)&0xff); \
		adler32_step(a,b,((dword)>>8)&0xff); \
		adler32_step(a,b,((dword)>>16)&0xff); \
		adler32_step(a,b,((dword)>>24)&0xff); \
	} while(0)

unsigned int util_csum_library(HMODULE lib)
{
	int i;
	int a = 1, b = 0;

	struct pe_hdr *pe = (struct pe_hdr*)lib;
	if (memcmp(pe->magic, PE_MAGIC, 4) != 0) {
		DWORD pe_offset = *(DWORD*)((char*)lib + 0x3c);
		pe = (struct pe_hdr*)((char*)lib + pe_offset);
		if (memcmp(pe->magic, PE_MAGIC, 4) != 0)
			return (DWORD)-1;
	}

	struct section_hdr *section = (struct section_hdr *) ((char*)pe + sizeof(*pe) + pe->opt_header_size);
	for (i = 0; i < pe->sections; ++i) {
		int j;

		adler32_dword(a, b, section[i].raw_data);
		for (j = 0; j < 8; ++j)
			adler32_step(a, b, (unsigned char)section[i].name[j]);
	}

	return (b << 16) | a;
}

char *util_game_path(void)
{
	static char *dir = NULL, d2win[MAX_PATH];
	if (!dir) {
		HMODULE lib = LoadLibrary("D2Win.dll");
		if (!lib) return NULL;
		GetModuleFileName(lib, d2win, sizeof(d2win));
		FreeLibrary(lib);
		dir = dirname(d2win);
	}
	return dir;
}
