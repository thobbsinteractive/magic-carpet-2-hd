#pragma once

#include <cstdint>

#include "Mc1GameState.h"

// MC1 frame icons (20FB0): the castle, balloons and wizard panel (22E50) and the spell icons (23D40)
//
// port: as the MC2 UI, the panel, the spell icons and the spell table take a position and a scale, from the getters of
// GameUI.cpp (GetInGameUiScale, GetTopBarPosition, GetSpellIconsPosition, GetSpellMenuPosition). At 640x480 and
// 320x200 the scale is 1 and the positions are the original ones. The low res UI (320x200, doubled coordinates)
// always stays the original, as its bitmaps are not scaled.

// the sprites of the top bar (spritesIndex): the frame of the minimap and a cell (castle, balloons, wizard), as the MC2
// MINI_MAP_PANEL and HEALTH_PANEL
constexpr int MC1_MINI_MAP_PANEL = 40;
constexpr int MC1_HEALTH_PANEL = 41;

// GetInGameUiScale in the 640 UI, else 1
uint8_t Mc1UiScale();
// MC2 GetTopBarWidth with the MC1 sprites
int Mc1GetTopBarWidth(uint8_t scale);
// MC2 GetTopBarPosition with the MC1 top bar: the top left of the panel and the minimap in it (0, 0 in the original)
void Mc1GetTopBarPosition(int16_t* posX, int16_t* posY);
// the two spell icons of the mouse buttons (1FEh and 23Eh, 2 in the original): GetSpellIconsPosition
void Mc1SpellIconsPosition(int16_t* leftPosX, int16_t* rightPosX, int16_t* posY);
// the size of the spell table of the spells mode (4 x 6 cells), scaled
int32_t Mc1SpellTableWidth(uint8_t scale);
int32_t Mc1SpellTableHeight(uint8_t scale);
// the top left of the spell table of the spells mode (180h, A2h / C2h in the original): port, in the 640 UI against the
// right edge of the screen, as far up from the bottom as the original (scaled)
void Mc1SpellTablePosition(int16_t* posX, int16_t* posY, uint8_t scale);
void Mc1MapSize(int32_t* width, int32_t* height, uint8_t scale);

// the castle with its life, mana and size, the balloons, the wizard; the flashes of pd+187h..189h counted down.
// posX, posY: the top left of the panel (the minimap's frame is at 2, 2 of it)
void DrawPlayerPanel_22E50_22E50(Type_Mc1Entity* wizard, int16_t posX = 0, int16_t posY = 0, uint8_t scale = 1);
// the spell of the slot: the mana of the owner per cast, the charge, the key number (pd+34Ch flash), darkened
// without castle mana
void DrawSpellIcon_23D40_23D40(int32_t x, int32_t y, Type_Mc1Entity* spell, int32_t selected, uint8_t scale = 1);
// the spells mode: a spell not got (the shape of its icon over the cell)
void DrawSpellAbsent_23CF0_23CF0(int32_t x, int32_t y, int32_t model, uint8_t scale = 1);
// the spells mode: the cell of a got spell with its key, darkened without castle mana
void DrawSpellCell_24230_24230(int32_t x, int32_t y, Type_Mc1Entity* spell, int32_t selected, uint8_t scale = 1);
// the frame of the cell under the joystick in the spells mode (320x200 / 640x480)
void FrameRect320_24D20_24D20(int32_t x, int32_t y, int32_t width, int32_t height, int32_t color);
void FrameRect640_24DA0_24DA0(int32_t x, int32_t y, int32_t width, int32_t height, int32_t color);
// the map: the table of the players with their kills of each other (the mouse at the bottom)
void DrawPlayersTable_22880_22880();
