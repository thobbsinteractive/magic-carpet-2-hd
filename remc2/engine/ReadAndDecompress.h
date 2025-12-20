#pragma once
#ifndef MAIN_READANDDECOMPRESS
#define MAIN_READANDDECOMPRESS
#include "MapType.h"
#include "Basic.h"

#include "Entity.h"

extern uint8_t BigTextureBuffer[128 * 128 * 160];
signed int sub_AB9E1_get_file_unpack_size(const char* a1);
signed int UnpackAndLoadMemoryFromPath(Pathstruct path);
uint8_t* ClearMemoryForPath(Pathstruct path);
bool sub_84250_load_file_array(int psindex);
void sub_54630_load_psxblock(uint16_t TextSize);
void sub_54660_read_and_decompress_sky_and_blocks(MapType_t GraphicsType, uint8_t GraphicsSize);
void sub_54800_read_and_decompress_tables(MapType_t a1);

#endif //MAIN_READANDDECOMPRESS