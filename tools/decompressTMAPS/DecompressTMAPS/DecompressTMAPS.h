#pragma once
#include <iostream>
#include <stdint.h>
#include <vector>
#include <algorithm>    // std::sort
#include <filesystem>
#include "mctypes.h"
#include "png.h"
#include "BitmapIO.h"
#include <filesystem>

#define x_BYTE int8
#define x_WORD int16
#define x_DWORD int32
#define x_LONG int32

#define SIZEOF_UNSIGNED_INT 4

#define SIZEOF_UNSIGNED_CHAR 1

#define SIZEOF_UNSIGNED_SHORT 2

#if SIZEOF_UNSIGNED_SHORT != 2
#  error "sizeof (unsigned short) != 2"
#else
typedef unsigned short Bit16u;
typedef   signed short Bit16s;
#endif

#if SIZEOF_UNSIGNED_INT == 4
typedef unsigned int Bit32u;
typedef   signed int Bit32s;
#elif SIZEOF_UNSIGNED_LONG == 4
typedef unsigned long Bit32u;
typedef   signed long Bit32s;
#else
//#  error "can't find sizeof(type) of 4 bytes!"
#endif

#if SIZEOF_UNSIGNED_CHAR != 1
#  error "sizeof (unsigned char) != 1"
#else
typedef unsigned char Bit8u;
typedef   signed char Bit8s;
#endif

typedef          char   int8;
typedef   signed char   sint8;
typedef unsigned char   uint8;
typedef          short  int16;
typedef   signed short  sint16;
typedef unsigned short  uint16;
typedef          int    int32;
typedef   signed int    sint32;
typedef unsigned int    uint32;

#define _BYTE  uint8
#define _WORD  uint16
#define _DWORD uint32
#define _QWORD uint64

#define LOWORD(x)   (*((_WORD*)&(x)))   // low word
#define HIWORD(x)   (*((_WORD*)&(x)+1))
#define LOBYTE(x)   (*((_BYTE*)&(x)))   // low byte
#define HIBYTE(x)   (*((_BYTE*)&(x)+1))

namespace fs = std::filesystem;

enum ImageType { png, pnga, bmp, rnc, data };

int other_folder0[] = { 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
	51, 52, 53, 54, 57, 59, 76, 77, 82, 85, 88, 89, 90,
	91, 92, 93, 94, 95, 144, 260, 261, 425 - 1 };

int other_folder1[] = { 38, 39, 40, 41, 42, 43, 44, 45, 46, 47 ,48, 49, 50,
	51, 52, 53, 54, 77, 82, 261, -1 };

int other_folder2[] = { 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
	51, 52, 53, 54, 66, 77, 82, 261, -1 };

int* other_folder = nullptr;

int transparent_color = 0;

int sub_main(const char palfilename[], const char tmapsdatfilename[], const char tmapstabfilename[], const char tmapsstr[], int max_images, ImageType imageType, int padding, bool caveSprites, const char outputPath[]);