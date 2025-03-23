#pragma once

#include <cstdint>

#include "engine_support.h"
#include "global_types.h"

#include "Type_F2C20ar.h"
#include "Type_Unk_F0E20x.h"

enum class SpriteRenderMode {
    shadow = 0,
    normal = 1,
    reflection = 2
};

void DrawSprite_41BD3_F2CC6zero(
    SpriteRenderMode a1, 
    const int32_t spriteWidth,
    type_F2C20ar &str_F2C20ar,
	uint8_t* m_ptrDWORD_E9C38_smalltit,
	const int m_bufferOffset_E9C38_1,
	const int m_bufferOffset_E9C38_2,
	const int m_bufferOffset_E9C38_3,
    const int iScreenWidth_DE560,
    uint8_t* ViewPortRenderBufferStart_DE558,
	type_unk_F0E20x m_str_F0E20x[GAME_RES_MAX_WIDTH + 100],
    uint8_t x_BYTE_F6EE0_tablesx[83456],
    type_x_D41A0_BYTEARRAY_4_struct x_D41A0_BYTEARRAY_4_struct,
	const uint8_t* m_ptrColorPalette
    );

void DrawSprite_41BD3_F2CC6set(
    SpriteRenderMode a1, 
    const int32_t spriteWidth,
    type_F2C20ar &str_F2C20ar,
	uint8_t* m_ptrDWORD_E9C38_smalltit,
	const int m_bufferOffset_E9C38_1,
    const int iScreenWidth_DE560,
    uint8_t* ViewPortRenderBufferStart_DE558,
    uint8_t x_BYTE_F6EE0_tablesx[83456],
    type_x_D41A0_BYTEARRAY_4_struct x_D41A0_BYTEARRAY_4_struct
    );
