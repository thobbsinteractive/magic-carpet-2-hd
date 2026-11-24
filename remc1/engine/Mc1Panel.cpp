#include "Mc1Panel.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

#include "Mc1Blocks.h"
#include "Mc1Files.h"
#include "Mc1Game.h"
#include "Mc1Graphics.h"
#include "Mc1Minimap.h"
#include "Mc1Screen.h"
#include "Mc1Shared.h"
#include "../../remc2/engine/read_config.h"

static Type_Mc1Entity* Entities()
{
	return mc1Entities;
}

static int16_t ReadS16(const uint8_t* data)
{
	int16_t value;
	memcpy(&value, data, sizeof(value));
	return value;
}

static uint16_t ReadU16(const uint8_t* data)
{
	uint16_t value;
	memcpy(&value, data, sizeof(value));
	return value;
}

static int32_t ReadS32(const uint8_t* data)
{
	int32_t value;
	memcpy(&value, data, sizeof(value));
	return value;
}

static bool LowRes()
{
	return (*videoMode_12F02E_12F01E & 1) != 0;
}

// port: the layout and scale of the MC2 UI (GameUI.cpp), only in the 640 UI: the low res UI (320x200) stays the
// original, as its bitmaps are not scaled
static bool LaidOutUi()
{
	return !LowRes() && UseInGameUiLayout();
}

uint8_t Mc1UiScale()
{
	return LowRes() ? 1 : GetInGameUiScale();
}

int Mc1GetTopBarWidth(uint8_t scale)
{
	// MC2 GetTopBarWidth with the MC1 sprites: 2 pixel border either side, the frame of the minimap and the 3 cells
	// (castle, balloons, wizard)
	return (4 + spritesIndex[MC1_MINI_MAP_PANEL].width_4 + 3 * spritesIndex[MC1_HEALTH_PANEL].width_4) * scale;
}

void Mc1GetTopBarPosition(int16_t* posX, int16_t* posY)
{
	// MC2 GetTopBarPosition with the MC1 top bar (its margin as is: only the 640 UI is laid out)
	*posX = 0;
	*posY = 0;
	if (LaidOutUi())
	{
		*posX = (int16_t)GetUiElementPosX(topBarLayout, Mc1GetTopBarWidth(Mc1UiScale()));
		*posY = (int16_t)(std::clamp)(topBarLayout.marginY, 0, UiScreenHeight());   // down from the top
	}
}

void Mc1SpellIconsPosition(int16_t* leftPosX, int16_t* rightPosX, int16_t* posY)
{
	// the icons of MC1 are where the MC2 ones are: 1FEh and 23Eh, 2 (right aligned by default)
	*leftPosX = 0x1FE;
	*rightPosX = 0x23E;
	*posY = 2;
	if (LaidOutUi())
		GetSpellIconsPosition(leftPosX, rightPosX, posY);
}

void Mc1SpellTablePosition(int16_t* posX, int16_t* posY, uint8_t scale)
{
	*posX = 0x180;
	*posY = LowRes() ? 0xA2 : 0xC2;
	if (!LaidOutUi())
		return;
	// the spells screen is 640x480 laid out as the MC2 spell menu (640 wide, up from the bottom): the table at 180h,
	// as far up from the bottom as in 640x480, scaled
	int16_t menuX, bottomY;
	GetSpellMenuPosition(&menuX, &bottomY);
	*posX = (int16_t)(menuX + 0x180 * scale);
	*posY = (int16_t)(std::max)(bottomY - (0x1E0 - 0xC2) * scale, 0);
}

// the edges the UI is clipped to: 640x400 (the low res UI's doubled coordinates) or 640x480 in the original, the
// screen at the other resolutions
static int32_t UiWidth()
{
	return LowRes() ? 0x280 : UiScreenWidth();
}

static int32_t UiHeight()
{
	return LowRes() ? 0x190 : UiScreenHeight();
}

static void DrawSprite(int32_t x, int32_t y, const bitmap_pos_struct_t& sprite, uint8_t scale = 1)
{
	if (LowRes())
		DrawBitmap320_60CE0_611F0((int16_t)x, (int16_t)y, sprite, 1);
	else
		DrawBitmap640_60D18_61228((int16_t)x, (int16_t)y, sprite, scale);
}

// 23940 / 23AE0: the sprite blended with the screen (BB934 = B7934 + 4000h, [screen][pixel]); 23AE0 blends the colour
// in the shape of the sprite, without the checks. In 320x200 (videoMode 1) the position and the rows are halved,
// the pixels of the rows are not (the upper half of the sprite)
static void BlendSprite(int32_t x, int32_t y, const bitmap_pos_struct_t& sprite, int32_t color, uint8_t scale = 1)
{
	const bool low = *videoMode_12F02E_12F01E == 1;
	if (color < 0)
	{
		if ((int16_t)x < 0 || (int16_t)y < 0 || (int16_t)x + sprite.width_4 * scale > UiWidth()
			|| (int16_t)y + sprite.height_5 * scale > UiHeight())
			return;
	}
	const uint32_t pitch = *screenWidth_12EFF0_12EFE0;
	if (scale > 1 && !low)
	{
		// port: each pixel of the sprite a square of scale x scale (the 640 UI at other resolutions)
		uint8_t* row = *workScreen_12EFF4_12EFE4 + pitch * (int16_t)y + (int16_t)x;
		int32_t column = 0;
		int32_t rows = sprite.height_5;
		const uint8_t* data = sprite.data;
		while (rows)
		{
			const int8_t code = (int8_t)*data++;
			if (!code)
			{
				row += pitch * scale;
				column = 0;
				rows--;
			}
			else if (code < 0)
				column -= code;
			else
			{
				for (int32_t i = 0; i < code; i++, column++)
				{
					const uint8_t source = color < 0 ? data[i] : (uint8_t)color;
					for (int32_t dy = 0; dy < scale; dy++)
					{
						uint8_t* pixel = row + dy * pitch + column * scale;
						for (int32_t dx = 0; dx < scale; dx++, pixel++)
							*pixel = tables_B7934_B7924.blend_0x4000[*pixel][source];
					}
				}
				data += code;
			}
		}
		return;
	}
	int32_t rows = sprite.height_5;
	uint8_t* row;
	if (low)
	{
		rows /= 2;
		row = *workScreen_12EFF4_12EFE4 + pitch * ((int16_t)y / 2) + (int16_t)x / 2;
	}
	else
		row = *workScreen_12EFF4_12EFE4 + pitch * (int16_t)y + (int16_t)x;
	uint8_t* pixel = row;
	const uint8_t* data = sprite.data;
	while ((int16_t)rows)
	{
		const int8_t code = (int8_t)*data++;
		if (!code)
		{
			row += pitch;
			pixel = row;
			rows--;
		}
		else if (code < 0)
			pixel -= code;
		else
		{
			for (int32_t i = 0; i < code; i++, pixel++)
				*pixel = tables_B7934_B7924.blend_0x4000[*pixel][color < 0 ? data[i] : (uint8_t)color];
			data += code;
		}
	}
}

// 24C20 / 24CA0 (61640 / 616C0): the rectangle within 640x400 or 640x480, at least 2x2; the low resolution halves it.
// The translucent fill (9ADFC & 4) is set only by the texts
static void FillRect(int32_t x, int32_t y, int32_t width, int32_t height, uint8_t color)
{
	const bool low = LowRes();
	if (width < 2 || height < 2 || x < 0 || x + width >= UiWidth() || y < 0 || y + height >= UiHeight())
		return;
	const uint32_t pitch = *screenWidth_12EFF0_12EFE0; // DOS 140h / 280h
	uint16_t columns = (uint16_t)width;
	uint16_t rows = (uint16_t)height;
	uint8_t* row;
	if (low)
	{
		columns >>= 1;
		rows >>= 1;
		row = *workScreen_12EFF4_12EFE4 + pitch * ((uint16_t)y >> 1) + ((uint16_t)x >> 1);
	}
	else
		row = *workScreen_12EFF4_12EFE4 + pitch * (uint16_t)y + (uint16_t)x;
	for (; rows; rows--, row += pitch)
		memset(row, color, columns);
}

// 22810: the bar of the value up to the width
static void DrawBar(int32_t x, int32_t y, int32_t width, int32_t height, int32_t value, uint8_t color)
{
	if (value >= width)
		value = width;
	if (value < 2 || height < 2)
		return;
	FillRect(x, y, value, height, color);
}

// 61594 / 615D4: a pixel within 640x400 or 640x480
static void PutPixel(int32_t x, int32_t y, uint8_t color)
{
	const uint32_t pitch = *screenWidth_12EFF0_12EFE0;
	if (LowRes())
	{
		if ((uint16_t)x >= 0x280 || (uint16_t)y >= 0x190)
			return;
		(*workScreen_12EFF4_12EFE4)[pitch * ((uint16_t)y >> 1) + ((uint16_t)x >> 1)] = color;
	}
	else
	{
		if ((uint16_t)x >= UiWidth() || (uint16_t)y >= UiHeight())
			return;
		(*workScreen_12EFF4_12EFE4)[pitch * (uint16_t)y + (uint16_t)x] = color;
	}
}

// 247C0: the rectangle faded by the table (B7934, [fade][screen])
static void FadeRect(int32_t x, int32_t y, int32_t width, int32_t height, uint8_t fade)
{
	if (*videoMode_12F02E_12F01E == 1)
	{
		x = (int16_t)x / 2;
		y = (int16_t)y / 2;
		width = (int16_t)width / 2;
		height = (int16_t)height / 2;
	}
	const uint32_t pitch = *screenWidth_12EFF0_12EFE0;
	for (int16_t row = (int16_t)y; row < (int16_t)y + (int16_t)height; row++)
		for (int16_t column = (int16_t)x; column < (int16_t)x + (int16_t)width; column++)
		{
			uint8_t& pixel = (*workScreen_12EFF4_12EFE4)[pitch * row + column];
			pixel = tables_B7934_B7924.fades_0x0000[fade][pixel];
		}
}

void DrawPlayerPanel_22E50_22E50(Type_Mc1Entity* wizard, int16_t posX, int16_t posY, uint8_t scale)
{
	const intptr_t index = wizard - Entities();
	if (!index || index >= mc1EntityCount)
		return;
	Type_Mc1Settings_AE408_AE3F8* settings = mc1Settings_AE408_AE3F8;
	uint8_t* playerData = wizard->playerData_0xA0_160;
	const int16_t player = ReadS16(playerData + 0x30);
	const uint8_t mana = playerColors_99B58_99B58[player][0];
	const uint8_t maxMana = playerColors_99B58_99B58[player][1];
	const uint8_t mark = (&playerColors_99B58_99B58[0][0])[settings->colorIndex_0x5D_93[2] + player * 2];
	const uint8_t life = 0x7B;
	const int32_t s = scale;
	const int32_t y = posY + 2 * s;
	// the frame of the panel, then the cells of the castle, the balloons and the wizard
	BlendSprite(posX + 2 * s, y, spritesIndex[MC1_MINI_MAP_PANEL], -1, scale);
	int32_t x = posX + (spritesIndex[MC1_MINI_MAP_PANEL].width_4 + 2) * s;
	// the mana percent to win (word at 9790h of the level, 415C0)
	const uint16_t targetPercent = ReadU16((const uint8_t*)&mc1State_AE400_AE3F0->level_0x2F503_193795 + 0x9790);
	const int32_t markX = targetPercent * 0x40 / 100 * s;
	Type_Mc1Entity* castle = &Entities()[ReadU16(playerData + 0x32)];
	if (castle > Entities() && castle->word_0x1A_26 > 0)
	{
		if ((int8_t)playerData[0x187] > 0 && settings->colorIndex_0x5D_93[1])
		{
			BlendSprite(x, y, spritesIndex[55], -1, scale);
			playerData[0x187]--;
		}
		else
			BlendSprite(x, y, spritesIndex[MC1_HEALTH_PANEL], -1, scale);
		DrawSprite(x + 2 * s, y, spritesIndex[castle->word_0x1A_26 + 0x2B], scale);
		DrawSprite(x + 0x26 * s, y, spritesIndex[42], scale);
		DrawBar(x + 0x3A * s, y + 8 * s, 0x40 * s, 10 * s, castle->life_0x0C_12 * 64 / castle->maxLife_0x08_8 * s, life);
		// the mana of the castle and of the balloons (pd+134h) of the mana of the level; blinking when full
		const int32_t total = settings->manaStats_0xB8_184.total_0x04;
		const int32_t held = castle->mana_0x8C_140 + ReadS32(playerData + 0x134);
		if (held == castle->dword_0x88_136)
		{
			if (settings->colorIndex_0x5D_93[2] == 1)
				DrawBar(x + 0x3A * s, y + 0x1A * s, 0x40 * s, 10 * s, castle->dword_0x88_136 * 64 / total * s, maxMana);
			else
				DrawBar(x + 0x3A * s, y + 0x1A * s, 0x40 * s, 10 * s, held * 64 / total * s, mana);
		}
		else
		{
			DrawBar(x + 0x3A * s, y + 0x1A * s, 0x40 * s, 10 * s, castle->dword_0x88_136 * 64 / total * s, maxMana);
			DrawBar(x + 0x3A * s, y + 0x1A * s, 0x40 * s, 10 * s, held * 64 / total * s, mana);
		}
		if (!(settings->flags_0x00_0 & 0x10))
		{
			FillRect(x + 0x3A * s + markX, y + 0x18 * s, 2 * s, 2 * s, mark);
			FillRect(x + 0x3A * s + markX, y + (0x1A + 10) * s, 2 * s, 2 * s, mark);
		}
	}
	else
		BlendSprite(x, y, spritesIndex[54], -1, scale);
	x += spritesIndex[MC1_HEALTH_PANEL].width_4 * s;
	if (castle > Entities())
	{
		if ((int8_t)playerData[0x189] > 0 && settings->colorIndex_0x5D_93[1])
		{
			BlendSprite(x, y, spritesIndex[55], -1, scale);
			playerData[0x189]--;
		}
		else
			BlendSprite(x, y, spritesIndex[MC1_HEALTH_PANEL], -1, scale);
		// the balloons by the size of the castle
		int16_t balloons;
		switch ((uint16_t)castle->word_0x1A_26)
		{
		case 1: case 2: case 3:
			balloons = 1;
			break;
		case 4: case 5:
			balloons = 2;
			break;
		case 6: case 7:
			balloons = 3;
			break;
		default:
			balloons = 0;
			break;
		}
		if (balloons)
		{
			DrawSprite(x + 2 * s, y, spritesIndex[balloons + 0x32], scale);
			DrawSprite(x + 0x26 * s, y, spritesIndex[42], scale);
		}
		for (int16_t i = 0; i < balloons; i++)
		{
			Type_Mc1Entity* balloon = &Entities()[ReadU16(playerData + 0x34 + i * 2)];
			if (balloon <= Entities())
				continue;
			if (balloon->life_0x0C_12 >= 0)
				DrawBar(x + 0x3A * s, y + (0x0A + i * 2) * s, 0x40 * s, 2 * s, balloon->life_0x0C_12 * 0x40 / balloon->maxLife_0x08_8 * s, life);
			DrawBar(x + 0x3A * s, y + (0x1C + i * 2) * s, 0x40 * s, 2 * s, balloon->mana_0x8C_140 * 0x40 / balloon->dword_0x88_136 * s, mana);
		}
	}
	else
		BlendSprite(x, y, spritesIndex[54], -1, scale);
	x += spritesIndex[MC1_HEALTH_PANEL].width_4 * s;
	if ((int8_t)playerData[0x188] > 0 && settings->colorIndex_0x5D_93[1])
	{
		BlendSprite(x, y, spritesIndex[55], -1, scale);
		playerData[0x188]--;
	}
	else
		BlendSprite(x, y, spritesIndex[MC1_HEALTH_PANEL], -1, scale);
	// the wizard: blinking while hit (flags 30h) or pd+14Bh
	if (((wizard->flags_0x10_16 & 0x30) || ReadS16(playerData + 0x14B)) && settings->colorIndex_0x5D_93[1])
	{
		BlendSprite(x + 2 * s, y, spritesIndex[43], -1, scale);
		BlendSprite(x + 0x26 * s, y, spritesIndex[42], -1, scale);
	}
	else
	{
		DrawSprite(x + 2 * s, y, spritesIndex[43], scale);
		DrawSprite(x + 0x26 * s, y, spritesIndex[42], scale);
	}
	DrawBar(x + 0x3A * s, y + 8 * s, 0x40 * s, 10 * s, wizard->life_0x0C_12 * 64 / wizard->maxLife_0x08_8 * s, life);
	DrawBar(x + 0x3A * s, y + 0x1A * s, 0x40 * s, 10 * s, wizard->dword_0x88_136 * 64 / settings->manaStats_0xB8_184.total_0x04 * s, maxMana);
	DrawBar(x + 0x3A * s, y + 0x1A * s, 0x40 * s, 10 * s, wizard->mana_0x8C_140 * 64 / settings->manaStats_0xB8_184.total_0x04 * s, mana);
	if (!(settings->flags_0x00_0 & 0x10))
	{
		FillRect(x + 0x3A * s + markX, y + 0x18 * s, 2 * s, 2 * s, mark);
		FillRect(x + 0x3A * s + markX, y + (0x1A + 10) * s, 2 * s, 2 * s, mark);
	}
}

// 23D40 / 24230: the key of the spell, in another mode or while pd+34Ch (by the model) counts down; darkened while the
// castle has less mana than the spell needs (dword_0x84)
static void DrawSpellKey(int32_t x, int32_t y, Type_Mc1Entity* spell, Type_Mc1Entity* owner, const bitmap_pos_struct_t& cell,
	uint8_t scale)
{
	uint8_t* playerData = owner->playerData_0xA0_160;
	Type_Mc1Player_340B& ownerPlayer = mc1State_AE400_AE3F0->players_0x340B_13323[ReadS16(playerData + 0x30)];
	if (ownerPlayer.mode_0x44A_1098 || (int8_t)playerData[0x34C + (int8_t)spell->model_0x41_65] > 0)
	{
		playerData[0x34C + (int8_t)spell->model_0x41_65]--;
		for (int32_t key = 0; key < 10; key++)
		{
			const int8_t slot = (int8_t)playerData[0x304 + key];
			if (slot == -1)
				continue;
			if (&Entities()[ReadS32(playerData + 0x214 + slot * 4)] != spell)
				continue;
			BlendSprite(x, y, spritesIndex[key + 0x1E], colorLookup_AD168_AD158[0], scale);
			break;
		}
	}
	if (spell->dword_0x84_132)
	{
		const uint16_t castle = ReadU16(playerData + 0x32);
		if (!castle || spell->dword_0x84_132 > Entities()[castle].mana_0x8C_140)
			FadeRect(x, y, cell.width_4 * scale, cell.height_5 * scale, 0x30);
	}
}

void DrawSpellIcon_23D40_23D40(int32_t x, int32_t y, Type_Mc1Entity* spell, int32_t selected, uint8_t scale)
{
	if (spell <= Entities())
		return;
	SetFont_5A3C0_5A8D0(1);
	Type_Mc1Entity* owner = &Entities()[spell->word_0x2A_42];
	if (owner <= Entities())
		return;
	const int16_t player = ReadS16(owner->playerData_0xA0_160 + 0x30);
	const uint8_t dot = playerColors_99B58_99B58[player][0];
	const uint8_t bar = playerColors_99B58_99B58[player][1];
	// blinking while the spell recovers (word_0x30 below 40h)
	if (spell->word_0x30_48 < 0x40 && spell->word_0x30_48 && spell->word_0x32_50 > 0x40 && mc1Settings_AE408_AE3F8->colorIndex_0x5D_93[1])
		return;
	const int32_t s = scale;
	const bitmap_pos_struct_t& back = spritesIndex[spell->word_0x30_48 ? 2 : 1];
	if (selected)
		DrawSprite(x, y, back, scale);
	else
		BlendSprite(x, y, back, -1, scale);
	DrawSprite(x, y, spritesIndex[(int8_t)spell->model_0x41_65 + 6], scale);
	// the rest of the mana of the owner to the next cast as the bar, the casts as the dots (two in a column)
	FillRect(x + 4 * s, y + 0x24 * s, owner->mana_0x8C_140 % spell->dword_0x88_136 * 0x38 / spell->dword_0x88_136 * s, 4 * s, bar);
	int32_t casts = owner->mana_0x8C_140 / spell->dword_0x88_136;
	for (int32_t column = 0; column < 0x1B && casts; column++)
		for (int32_t i = 0; i < 2 && casts; i++, casts--)
		{
			if (s == 1)
				PutPixel(x + 4 + column * 2, y + 0x24 + i * 2, dot);
			else
				FillRect(x + (4 + column * 2) * s, y + (0x24 + i * 2) * s, s, s, dot);
		}
	// the charge of the spell
	if (spell->byte_0x3E_62 && (int8_t)spell->byte_0x3D_61 > 0)
		FillRect(x + 4 * s, y + 0x24 * s, (int8_t)spell->byte_0x3D_61 * 0x37 / (int8_t)spell->byte_0x3E_62 * s, 4 * s, colorLookup_AD168_AD158[0xF0]);
	DrawSpellKey(x, y, spell, owner, spritesIndex[1], scale);
}

void DrawSpellAbsent_23CF0_23CF0(int32_t x, int32_t y, int32_t model, uint8_t scale)
{
	BlendSprite(x, y, spritesIndex[3], -1, scale);
	BlendSprite(x, y, spritesIndex[model + 6], 0xA6, scale);
}

void DrawSpellCell_24230_24230(int32_t x, int32_t y, Type_Mc1Entity* spell, int32_t selected, uint8_t scale)
{
	if (spell <= Entities())
		return;
	SetFont_5A3C0_5A8D0(1);
	Type_Mc1Entity* owner = &Entities()[spell->word_0x2A_42];
	if (owner <= Entities())
		return;
	if (spell->word_0x30_48 < 0x40 && spell->word_0x30_48 && spell->word_0x32_50 > 0x40 && mc1Settings_AE408_AE3F8->colorIndex_0x5D_93[1])
		return;
	const bitmap_pos_struct_t& back = spritesIndex[spell->word_0x30_48 ? 4 : 3];
	if (selected)
		DrawSprite(x, y, back, scale);
	else
		BlendSprite(x, y, back, -1, scale);
	DrawSprite(x, y, spritesIndex[(int8_t)spell->model_0x41_65 + 6], scale);
	DrawSpellKey(x, y, spell, owner, spritesIndex[3], scale);
}

// 24D20 / 24DA0 (61740 / 61810): the frame of the rectangle within 640x400 or 640x480 by four lines
static void FrameRect(int32_t x, int32_t y, int32_t width, int32_t height, uint16_t color, bool low)
{
	if (width < 2 || height < 2 || x < 0 || x + width >= UiWidth() || y < 0 || y + height >= (low ? 0x190 : UiHeight()))
		return;
	auto line = low ? DrawLine320_6B260_6B770 : DrawLine640_6B378_6B888;
	width--;
	height--;
	line((int16_t)x, (int16_t)y, (int16_t)(x + width), (int16_t)y, color, 0);
	line((int16_t)(x + width), (int16_t)y, (int16_t)(x + width), (int16_t)(y + height + 1), color, 0);
	line((int16_t)x, (int16_t)(y + height), (int16_t)(x + width + 1), (int16_t)(y + height), color, 0);
	line((int16_t)x, (int16_t)y, (int16_t)x, (int16_t)(y + height), color, 0);
}

void FrameRect320_24D20_24D20(int32_t x, int32_t y, int32_t width, int32_t height, int32_t color)
{
	FrameRect(x, y, width, height, (uint16_t)color, true);
}

void FrameRect640_24DA0_24DA0(int32_t x, int32_t y, int32_t width, int32_t height, int32_t color)
{
	FrameRect(x, y, width, height, (uint16_t)color, false);
}

void DrawPlayersTable_22880_22880()
{
	// the table of the active players in the middle of the map: a row a player with its name and mana (sprite 85), a
	// column of the kills (pd+1Eh) for each of the 8 players (sprite 86); the width counts only the active players
	Type_Mc1State_AE400_AE3F0* state = mc1State_AE400_AE3F0;
	SetFont_5A3C0_5A8D0(1);
	int32_t count = 0;
	for (int32_t i = 0; i < state->playerCount_0x0A_10; i++)
		if (state->players_0x340B_13323[i].active_0x06_6)
			count++;
	const bitmap_pos_struct_t& head = spritesIndex[85];
	const bitmap_pos_struct_t& cell = spritesIndex[86];
	const int32_t x0 = (0x280 - (cell.width_4 * count + head.width_4)) / 2;
	int32_t y = ((*videoMode_12F02E_12F01E == 1 ? 0x190 : 0x1E0) - head.height_5 * count) / 2;
	char text[0x100];
	for (int32_t row = 0; row < state->playerCount_0x0A_10; row++)
	{
		Type_Mc1Player_340B& player = state->players_0x340B_13323[row];
		if (player.active_0x06_6 != 1)
			continue;
		Type_Mc1Entity* wizard = &Entities()[player.entityIndex_0x0A_10];
		int32_t x = x0;
		DrawSprite(x, y, head);
		FillRect(x + 4, y + 4, head.width_4 - 8, head.height_5 - 8, playerColors_99B58_99B58[row][0]);
		DrawText_5A180_5A690(player.name_0x40A_1034, x + 8, (int16_t)(y + 6), playerColors_99B58_99B58[row][1]);
		sprintf(text, "%d", wizard->dword_0x88_136);
		DrawText_5A180_5A690(text, x + 8, (int16_t)(y + 0x14), playerColors_99B58_99B58[row][1]);
		x += head.width_4;
		for (int32_t column = 0; column < 8; column++, x += cell.width_4)
		{
			if (column != row && state->players_0x340B_13323[column].active_0x06_6 == 1)
			{
				DrawSprite(x, y, cell);
				FillRect(x + 4, y + 4, cell.width_4 - 8, cell.height_5 - 8, playerColors_99B58_99B58[column][0]);
				sprintf(text, "%03d", ReadS16(wizard->playerData_0xA0_160 + 0x1E + column * 2));
				DrawText_5A180_5A690(text, x + 8, (int16_t)(y + 0x0A), playerColors_99B58_99B58[column][1]);
			}
			else if (column == row)
			{
				DrawSprite(x, y, cell);
				FillRect(x + 4, y + 4, cell.width_4 - 8, cell.height_5 - 8, colorLookup_AD168_AD158[0]);
			}
		}
		y += head.height_5;
	}
}
