#pragma once

#include <cstdint>

#include "../sub_main.h"
#include "TypeConfigDat.h"

// types
#pragma pack (1)
typedef struct //lenght 50
{
	uint32_t x_DWORD_17DB70; // weak
	int16_t x_WORD_17DB74; // weak 4
	int16_t unk_17DB76_posx; // weak 6
	int16_t unk_17DB78_posy; // weak 8
	int16_t fill1; // test 10
	int16_t fill2; // test 12
	int16_t unk_17DB7E; // weak 14
	int16_t unk_17DB80; // weak 16
	int16_t x_WORD_17DB82_shift_step; // weak 18
	int16_t x_WORD_17DB84; // weak 20
	int16_t x_WORD_17DB86; // weak 22
	int16_t x_WORD_17DB88; // weak 24
	int16_t x_WORD_17DB8A; // weak 26
	int16_t x_WORD_17DB8C; // weak 28
	int8_t x_BYTE_17DB8E; // weak 30
	int8_t x_BYTE_17DB8F; // weak 31
	int16_t unk_17DB90; // weak 32
	std::array<uint8_t, 6> unk_17DB92; // weak 32
	std::array<uint8_t, 16> fill3; // fill
} type_x_DWORD_17DB70str;
#pragma pack (16)

// from sub_main
extern char x_BYTE_D419C_level_num;
extern char x_BYTE_D41AD_skip_screen;
extern const char* LevelsNames_D9204[61];
extern char* off_DB558[];
extern __int16 x_WORD_E131A;
extern char x_BYTE_E1324;
extern type_WORD_E1F84 str_E1BAC[10];
extern long lastTime_E208C;
extern __int16 timeCounter_E2094;
extern char minTimeCounter_E2096;
extern char maxTimeCounter_E2097;
extern type_x_BYTE_E25ED_db_str x_BYTE_E25ED_db_str[16];
extern int16_t x_WORD_E29D8;
extern int16_t m_ExitMenuLoop_E29DC;
extern char x_BYTE_E29DF_skip_screen;
extern char x_BYTE_E29E0;
extern uint8_t* pre_x_DWORD_E9C3C;
extern uint8_t x_BYTE_EB39E_keys[10];
extern uint8_t uiBackGroundColorIdx_EB3A8;

extern int32_t x_DWORD_17DBB8[4];
extern char x_BYTE_17DBC6;
extern uint32_t x_DWORD_17DBC8x[125];
extern uint32_t x_DWORD_17DDBCx[26];
extern char x_BYTE_17E09D;
extern int16_t x_WORD_1803EC;
extern uint8_t unk_180560x[44];
extern type_unk_18058Cstr unk_18058Cstr;

extern int test_regression_level;

// functions
void MenusAndIntros_76930(int a2, uint16_t a3, bool skipMenus = false);
void InitLanguage_76A40();
void Intros_76D10(char a1);
void sub_76CF0();
void MainMenu_76FA0(/*int a1,*/ int a2, uint16_t a3);
bool NewGameDialog_77350(type_WORD_E1F84* a1x);
char LanguageSettingDialog_779E0(type_WORD_E1F84* a1x);
char LoadGameDialog_780F0(type_WORD_E1F84* a1x);
char SaveGameDialog_78730(type_WORD_E1F84* a1x);
char SetKeysDialog_79610();
signed int sub_79E10(char* a1, char a2);
void DrawMenuAnimations_7AB00();
void LoadAndSetGraphicsAndPalette_7AC00();
int sub_7ADE0(char a1);
void sub_7BEC0();
void sub_7DA70(__int16 a1, __int16 a2, __int16 a3, __int16 a4, __int16 a5, __int16 a6);
void sub_7DD70();
int16_t TestMouseRegions_7E1F0();
signed int sub_7E620(type_WORD_E1F84* a1x);
char sub_7E800(type_WORD_E1F84* a1x);
char sub_7E820(type_WORD_E1F84* a1x);
int NewGameDraw_7EAE0(__int16* a1, __int16* a2, __int16* a3, __int16* a4, int8_t* a5, x_WORD* a6);
int LoadLanguageFile(bitmap_pos_struct2_t** a1x, bitmap_pos_struct2_t** a2x, uint8_t* a3, char* langfilename, bitmap_pos_struct_t* a3dattabindex);
int sub_7F960(bitmap_pos_struct2_t* a1x, bitmap_pos_struct2_t* a2x, uint8_t* a3, char* langcountstring, bitmap_pos_struct_t* a3dattabindex);
void DrawText_80C30(__int16 posX, __int16 posY, __int16 a3);
bool sub_80D40_move_graphics_and_play_sounds(__int16 a2, __int16 a3, __int16 a4, __int16 a5, char a6);
void NewGameSubdraw_81760(/*type_unk_E17CC_str_0x194* a1*/);
void WriteConfigDat_81DB0();
void DrawAndSoundDragonAndFire_81EE0(__int16 a5, __int16 a6);
Type_SecretMapScreenPortals_E2970* sub_824E0(__int16 a1);
void sub_82510(/*__int16 a1*//*, int *a2*/);
void sub_82670();
void DrawEndGameTable_82C20(__int16 a1);
void PlayIntros_83250(char a1);
void sub_833C0();
void ShowWelcomeScreen_83850();
void DrawNetGameMapBackground_85C8B(uint8_t* a1, uint8_t* a2, int a3, int a4, int a5, int a6);
bool DrawAndServe_pre_sub_7B250(uint32_t var, type_WORD_E1F84* var2x);
char ExitDialog_77980(type_WORD_E1F84* a1x);
char SetPlayerNameDialog_78E00(/*int a1, int a2, */type_WORD_E1F84* a3x);
int sub_7F6A0(int a1, int a2, __int16 a3, __int16 a4, char* a5, int8_t* a6, unsigned __int8 a7);
char SetJoystickDialog_79160(type_WORD_E1F84* a1x);
int sub_7CCF0();
int sub_7CD30();
int sub_7CCA0();
int sub_7CDA0();
char sub_7CBF0();
char sub_7CC40();
char MultiplayerMenu_7DE80(type_WORD_E1F84* a2x);
int DrawScrollDialog_7BF20(type_str_word_26* a1x);
void sub_8C0E0(unsigned __int8(/*__fastcall*/ *a1)(signed int));
int DrawScrollDialog2_7B660(int a1, int a2, __int16 a3, type_str_word_26* a4x, char* a5, char a6);
char /*__fastcall*/ sub_77680();
char DrawAndServe_7B250(/*int a1, int a2*//*, __int16 a3*/);
signed int sub_7C390();
void sub_7C710();
void SetMultiplayerColors_7D310();
bool SetMultiplayerColors_7CE50();
void sub_7C720(unsigned __int8 a1, uint8_t* a2);
int sub_7CB10();
bool InRegion_7B200(type_WORD_E1F84* a1x, int16_t testx, int16_t testy);
void PaletteMulti_7C9D0(signed __int16 a1);
void CleanRectByColor_7C7C0(uint8_t* a2);
void CleanRecByColor_85C42(uint8_t* a1, int a2, int a3, int a4, int a5, unsigned __int8 a6);
void DrawNetworkLevelName_7D380();
void PaletteCopy_7C800(signed __int16 a1);
void sub_85BF5(uint8_t* a1, uint8_t* a2, int a3, int a4, int a5, int a6);

void sub_41BC0();