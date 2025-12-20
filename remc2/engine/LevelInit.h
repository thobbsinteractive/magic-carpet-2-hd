#pragma once
#ifndef MAIN_LEVEL_INIT
#define MAIN_LEVEL_INIT
#include "Basic.h"
#include "Sound.h"
#include "Terrain.h"
#ifdef REMC2_CODE
#include "Compare_maps.h"
#include "MouseOnOff.h"
#include "DatTabIndexes.h"
#include "PrintMessages.h"
#endif
#include "ConvertMapInfo.h"
#include "ReadAndDecompress.h"
#include "Entity.h"

extern type_str_164 unk_F42B0x[1136];

void LevelInit_56C00(levelDataType_2FECE* levelData);
void SetLevelId_53590(levelDataType_2FECE* levelData);
void LevelInitGame_56A30();
void ClearSettings_567C0();
void sub_49F30();
char LevelDecompress_533B0(int16 levelIndex, levelDataType_2FECE* levelData);
void sub_49270_generate_level_features(levelDataType_2FECE* terrain);
#endif //MAIN_LEVEL_INIT

