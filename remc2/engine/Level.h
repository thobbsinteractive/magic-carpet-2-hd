#pragma once

#include "global_types.h"
#include "engine_support.h"
#include "Basic.h"


extern type_str_164 unk_F42B0x[1136];
extern type_str_160 str_D7BD6[157];


bool SaveLevelSLEV_55250(uint8_t savefileindex, char* filenameindex);//SLEV1
bool SaveLevelSMAP_55320(uint8_t savefileindex, char* filenameindex);
bool SaveLevelSVER_55450(uint8_t savefileindex, int32_t a2, char* filenameindex);
bool LoadLevel_555D0(uint8_t fileindex, int levelindex);
bool LoadLevelSMAP_558E0(uint8_t savefileindex);
bool LoadLevelSLEV_55A10(uint8_t savefileindex);
bool SaveLevel_55080(uint8_t savefileindex, int32_t LevelNumber, char* filenameindex);
void sub_49270_generate_level_features(Type_Level_2FECE* terrain);
void sub_47160();
void sub_55100(char a1);
void sub_57680_FixPointersAfterLoad();
void sub_549A0(type_str_611* a1, type_str_611* a2);
void sub_49F90();
void sub_55AB0(type_str_0x2BDE* playStr);
void sub_71990();
void sub_71930();
void SetStagetagForTermod_49830(Type_Level_2FECE* terrain);
void CreateIndexes_6EB90(filearray_struct* a1);
void SetSpell_6D5E0(type_event_0x6E8E* a1, char a2);
void sub_712F0();
void sub_71A70_setTmaps(MapType_t a1);
int sub_6D710(type_event_0x6E8E* a1, unsigned __int8 a2, unsigned __int8 a3);
void sub_83CC0(char a1);
