#include "sub_main.h"
#include "engine/engine_support.h"
#include "engine/Compare_maps.h"
#include "engine/CommandLineParser.h"
#include "engine/MenusAndIntros.h"
#include "engine/Network.h"
#include "engine/ConvertMapInfo.h"
#include "engine/LevelInit.h"
#include "engine/Entity.h"

/*

//2541e7
mainConnection_E12AA 2b22aa->326f0 7f0300 .. 03
after NetworkTestCall_72FBB 350000 .. 00
after NetworkCancel_748F7 not changed

connection_E12AE[] 2b22ae->34340 b0160002 .. 16
after NetworkTestCall_72FBB 900b73 .. 0b
after NetworkCancel_748F7 not changed

*/

#ifdef __linux__
#include <strings.h>
#include <cstdlib>
#include <cstring>
#define _stricmp strcasecmp
#define _strnicmp strncasecmp
#define strnicmp strncasecmp
#define __cdecl
#include <ctype.h>
#include "../findfirst/findfirst.h"
#include <iostream>
#include <functional>
#include <type_traits>
#else
#include <filesystem>
#endif //__linux__

//int test_regression_level = 1;
//first multi is 50(51) 10
//first hide level is 30(31) 5

//adress 2285ff

//int save_debugcounter = 0;

#define FIX_sub_48B90// - when set game have error, but compactible with original
int x_DWORD_E3BB0 = 0; // weak
int x_DWORD_E3BB8 = 1; // weak
uint8_t* x_DWORD_E3BAC = 0; // weak
char x_BYTE_D4B50 = 0; // weak

//(set in bool sub_558E0_InGameLoad(uint8_t fileindex)//2368e0)
//int count_begin = 1;//1
//int stage__4A190_0x6E8E = 0;
//int debugnextlevel = 0;

//bool config_EDITOR = false;
//bool config_LOAD_EDITED_LEVEL = false;

void begin_plugin() {
	//convert_x_WORD_E2970y_to_x_WORD_E2970();
	//x_D41A0_BYTEARRAY_4_struct.dwordindex_2388 = 10022;//fix sound param
}

void preconvert() {
	/*
	int16_t word_0;
	int16_t word_2;
	int16_t word_4;
	int16_t word_6;
	int16_t word_8;
	int16_t word_10;
	int16_t word_12;
	int16_t word_14;
	int16_t word_16;
	int16_t word_18;
	int16_t word_20;
}
type_D404C;
#pragma pack (16)

type_D404C str_D404C[5];

uint8_t ar_D404C[22 * 5]//particles table*/
/*
//type_E24BCx str_E24BCx[32];
memcpy((type_E17CC_0*)str_E1328, array_E1328 , 0x100*sizeof(type_E17CC_0));
char outname[512];
char buffer[512];
sprintf(outname, "c:\\prenos\\remc2\\outstr.txt");
FILE* file = fopen(outname, "wt");
sprintf(buffer, "type_E17CC_0 str_E17CC_0[48] = {\n");
fwrite(buffer, strlen(buffer), 1, file);
for (int i = 0; i < 0x100; i++)
{
	sprintf(buffer, "{0x%04X,0x%02X,0x%08X},\n",
		str_E1328[i].word_0, str_E1328[i].byte_2, str_E1328[i].dword_3
		);

	fwrite(buffer, strlen(buffer), 1, file);
}
sprintf(buffer, "};\n");
fwrite(buffer, strlen(buffer), 1, file);
fclose(file);
*/


/*
memcpy((uint8_t*)str_x_DWORD_D4C52ar_0x2F22, x_DWORD_D4C52ar + 0x2F22, 4000);
char outname[512];
char buffer[512];
sprintf(outname, "c:\\prenos\\remc2\\outstr.txt");
FILE* file = fopen(outname, "wt");
sprintf(buffer, "type_mapScreenPortals_E17CC mapScreenPortals_E17CC[168] = {\n");
fwrite(buffer, strlen(buffer), 1, file);
for (int i = 0; i < 168; i++)
{
	sprintf(buffer, "{0x%04X,0x%02X,0x%04X,0x%04X},\n",
		str_x_DWORD_D4C52ar_0x2F22[i].dword_0, str_x_DWORD_D4C52ar_0x2F22[i].word_4, str_x_DWORD_D4C52ar_0x2F22[i].adress_6,
		str_x_DWORD_D4C52ar_0x2F22[i].dword_10);
	fwrite(buffer, strlen(buffer), 1, file);
}
sprintf(buffer, "};\n");
fwrite(buffer, strlen(buffer), 1, file);
fclose(file);*/
/*
#pragma pack (1)
typedef struct {//lenght 22
	int16_t word_0;
	int16_t word_2;
	int16_t word_4;
	int16_t word_6;
	int16_t word_8;
	int16_t word_10;
	int16_t word_12_x; //X
	int16_t word_14_y; //Y
	int16_t word_16;
	int8_t byte_18_act;//1-hidden 2-activated
	int8_t byte_19;
	int16_t word_20;
}type_mapScreenPortals_E17CC;
#pragma pack (16)
	type_mapScreenPortals_E17CC mapScreenPortals_E17CC[168] = {
	//0 2  4    6       8   10      12      14        16    18 19 20 stub
	{0, 0, 0x74, 0x1de, 28, 0xa40,   0x341, 0x213,      0,    2,  0, 0, 0 }
};

uint8_t unk_E17CC_0x194x[0xE6C] = {
	*/

	/*memcpy((uint8_t*)x_BYTE_E25ED_db_str, x_BYTE_E25ED_db, 0x1E0);
	char outname[512];
	char buffer[512];
	sprintf(outname, "c:\\prenos\\remc2\\outstr.txt");
	FILE* file = fopen(outname, "wt");
	sprintf(buffer, "type_x_BYTE_E25ED_db_str x_BYTE_E25ED_db_str[16]={\n");
	fwrite(buffer, strlen(buffer), 1, file);
	for (int i = 0; i < 17; i++)
	{
		sprintf(buffer, "{0x%08X,0x%08X,0x%04X,0x%04X,0x%04X,0x%04X,0x%04X,0x%04X,0x%02X,0x%02X,0x%04X,0x%04X,0x%04X,0x%04X},\n", x_BYTE_E25ED_db_str[i].dword_0, x_BYTE_E25ED_db_str[i].dword_4, x_BYTE_E25ED_db_str[i].word_8, x_BYTE_E25ED_db_str[i].word_10,
			x_BYTE_E25ED_db_str[i].word_12, x_BYTE_E25ED_db_str[i].word_14, x_BYTE_E25ED_db_str[i].word_16, x_BYTE_E25ED_db_str[i].word_18,
			x_BYTE_E25ED_db_str[i].byte_20, x_BYTE_E25ED_db_str[i].byte_21, x_BYTE_E25ED_db_str[i].word_22, x_BYTE_E25ED_db_str[i].word_24,
			x_BYTE_E25ED_db_str[i].word_26, x_BYTE_E25ED_db_str[i].word_28);
		fwrite(buffer, strlen(buffer), 1, file);
	}
	sprintf(buffer, "};\n");
	fwrite(buffer, strlen(buffer), 1, file);
	fclose(file);*/
	/*
	typedef struct {//lenght 30
		int32_t dword_0;
		int32_t dword_4;
		int16_t word_8;
		int16_t word_10;
		int16_t word_12;
		int16_t word_14;
		int16_t word_16;
		int16_t word_18;
		int8_t byte_20;
		int8_t byte_21;
		int16_t word_22;
		int16_t word_24;
		int16_t word_26;
		//int8_t stuba[1];
		int16_t word_28;
		//int8_t stubb[1];
	} type_x_BYTE_E25ED_db_str;

	type_x_BYTE_E25ED_db_str x_BYTE_E25ED_db_str[16]=;
	uint8_t x_BYTE_E25ED_db[0x1E0] = {
	*/
	/*memcpy((uint8_t*)&str_D4C52ar[1], x_DWORD_D4C52ar+8, 0x132);
	str_D4C52ar[0].dword_0 = 0x2a5c44;
	char outname[512];
	char buffer[512];
	sprintf(outname, "c:\\prenos\\remc2\\outstr.txt");
	FILE* file = fopen(outname,"wt");
	sprintf(buffer,"type_D4C52ar str_D4C52ar[18]={\n");
	fwrite(buffer,strlen(buffer),1,file);
	for (int i = 0; i < 17; i++)
	{
		sprintf(buffer, "0x%08X,0x%08X,0x%04X,0x%08X,0x%08X,\n", str_D4C52ar[i].dword_0, str_D4C52ar[i].dword_4, str_D4C52ar[i].word_8, str_D4C52ar[i].dword_10, str_D4C52ar[i].dword_14);
		fwrite(buffer, strlen(buffer), 1, file);
	}
	sprintf(buffer, "};\n");
	fwrite(buffer, strlen(buffer), 1, file);
	fclose(file);*/
	/*
	memcpy(x_DWORD_D4C52ar_strtest, x_DWORD_D4C52ar + 0x002A7400 - 0x2a5c52, 14*500);
	char outname[512];
	char buffer[512];
	sprintf(outname, "c:\\prenos\\remc2\\outstr.txt");
	FILE* file = fopen(outname, "wt");
	sprintf(buffer, "type_D4C52ar2 x_DWORD_D4C52ar_strA0[32]={\n");
	fwrite(buffer, strlen(buffer), 1, file);
	for (int i = 0; i < 500; i++)
	{
		sprintf(buffer, "0x%08X,0x%04X,0x%08X,0x%08X,\n", x_DWORD_D4C52ar_strtest[i].dword_0, x_DWORD_D4C52ar_strtest[i].word_4, x_DWORD_D4C52ar_strtest[i].dword_6, x_DWORD_D4C52ar_strtest[i].dword_10);
		fwrite(buffer, strlen(buffer), 1, file);
	}
	sprintf(buffer, "};\n");
	fwrite(buffer, strlen(buffer), 1, file);
	fclose(file);
	*/
	//memcpy((uint8_t*)str_D4D84ar, x_DWORD_D4C52arx+0x132, 0x67da);

	/*
	memcpy((uint8_t*)& SPELLS_BEGIN_BUFFER_str, SPELLS_BEGIN_BUFFER_DA818x, 26*80);
	char outname[512];
	char buffer[512];
	sprintf(outname, "c:\\prenos\\remc2\\outstr.txt");
	FILE* file = fopen(outname, "wt");
	sprintf(buffer, "type_SPELLS_BEGIN_BUFFER_str SPELLS_BEGIN_BUFFER_str[26] =\n");
	fwrite(buffer, strlen(buffer), 1, file);
	for (int i = 0; i < 26; i++)
	{
		sprintf(buffer, "{{0x%02X,0x%02X,{\n", SPELLS_BEGIN_BUFFER_str[i].byte_0, SPELLS_BEGIN_BUFFER_str[i].byte_1);
		fwrite(buffer, strlen(buffer), 1, file);
		sprintf(buffer, "{0x%08X,0x%08X,0x%08X,0x%08X,0x%08X,0x%04X,0x%04X,0x%02X,0x%02X},\n", SPELLS_BEGIN_BUFFER_str[i].subspell[0].dword_2, SPELLS_BEGIN_BUFFER_str[i].subspell[0].dword_6, SPELLS_BEGIN_BUFFER_str[i].subspell[0].dword_A, SPELLS_BEGIN_BUFFER_str[i].subspell[0].dword_E, SPELLS_BEGIN_BUFFER_str[i].subspell[0].dword_0x12, SPELLS_BEGIN_BUFFER_str[i].subspell[0].word_0x16x, SPELLS_BEGIN_BUFFER_str[i].subspell[0].word_0x18, SPELLS_BEGIN_BUFFER_str[i].subspell[0].byte_0x1A, SPELLS_BEGIN_BUFFER_str[i].subspell[0].byte_0x1B);
		fwrite(buffer, strlen(buffer), 1, file);
		sprintf(buffer, "{0x%08X,0x%08X,0x%08X,0x%08X,0x%08X,0x%04X,0x%04X,0x%02X,0x%02X},\n", SPELLS_BEGIN_BUFFER_str[i].subspell[1].dword_2, SPELLS_BEGIN_BUFFER_str[i].subspell[1].dword_6, SPELLS_BEGIN_BUFFER_str[i].subspell[1].dword_A, SPELLS_BEGIN_BUFFER_str[i].subspell[1].dword_E, SPELLS_BEGIN_BUFFER_str[i].subspell[1].dword_0x12, SPELLS_BEGIN_BUFFER_str[i].subspell[1].word_0x16x, SPELLS_BEGIN_BUFFER_str[i].subspell[1].word_0x18, SPELLS_BEGIN_BUFFER_str[i].subspell[1].byte_0x1A, SPELLS_BEGIN_BUFFER_str[i].subspell[1].byte_0x1B);
		fwrite(buffer, strlen(buffer), 1, file);
		sprintf(buffer, "{0x%08X,0x%08X,0x%08X,0x%08X,0x%08X,0x%04X,0x%04X,0x%02X,0x%02X}}},\n", SPELLS_BEGIN_BUFFER_str[i].subspell[2].dword_2, SPELLS_BEGIN_BUFFER_str[i].subspell[2].dword_6, SPELLS_BEGIN_BUFFER_str[i].subspell[2].dword_A, SPELLS_BEGIN_BUFFER_str[i].subspell[2].dword_E, SPELLS_BEGIN_BUFFER_str[i].subspell[2].dword_0x12, SPELLS_BEGIN_BUFFER_str[i].subspell[2].word_0x16x, SPELLS_BEGIN_BUFFER_str[i].subspell[2].word_0x18, SPELLS_BEGIN_BUFFER_str[i].subspell[2].byte_0x1A, SPELLS_BEGIN_BUFFER_str[i].subspell[2].byte_0x1B);
		fwrite(buffer, strlen(buffer), 1, file);
	}
	sprintf(buffer, "};\n");
	fwrite(buffer, strlen(buffer), 1, file);
	fclose(file);
	*/
	/*
	int32_t dword_2;
	int32_t dword_6;
	int32_t dword_A;
	int32_t dword_E;
	int32_t dword_0x12;
	int16_t word_0x16x;
	int16_t word_0x18;
	int8_t byte_0x1A;
	uint8_t byte_0x1B;
}
type_SPELLS_BEGIN_BUFFER_str_sub;

typedef struct {//lenght 80
	int8_t byte_0;
	uint8_t byte_1;
	type_SPELLS_BEGIN_BUFFER_str_sub subspell[3];
}
type_SPELLS_BEGIN_BUFFER_str;

	type_SPELLS_BEGIN_BUFFER_str SPELLS_BEGIN_BUFFER_str[26] =
	{{1,2,{
	{3,4,0,0,0,0,0,0,0},
	{5,6,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0}}}
	};
	*/
	/*type_SPELLS_BEGIN_BUFFER_str SPELLS_BEGIN_BUFFER_str[26];

	int8_t SPELLS_BEGIN_BUFFER_DA818x[0x820] = {*/
}

void initposistruct() {//fix it
	posistruct1 = (posistruct_t*)malloc(sizeof(posistruct_t) * 1700);
	posistruct2 = (posistruct_t*)malloc(sizeof(posistruct_t) * 1700);
	posistruct3 = (posistruct_t*)malloc(sizeof(posistruct_t) * 1700);
	posistruct4 = (posistruct_t*)malloc(sizeof(posistruct_t) * 1700);
	posistruct5 = (posistruct_t*)malloc(sizeof(posistruct_t) * 1700);
	posistruct6 = (posistruct_t*)malloc(sizeof(posistruct_t) * 1700);
	posistruct7 = (posistruct_t*)malloc(sizeof(posistruct_t) * 1700);
	posistruct8 = (posistruct_t*)malloc(sizeof(posistruct_t) * 1700);
	posistruct9 = (posistruct_t*)malloc(sizeof(posistruct_t) * 1700);
	posistruct10 = (posistruct_t*)malloc(sizeof(posistruct_t) * 1700);
	posistruct11 = (posistruct_t*)malloc(sizeof(posistruct_t) * 1700);

	x_DWORD_EA3D4 = (posistruct_t*)malloc(sizeof(posistruct_t) * 1700);
};

//----- (00056210) --------------------------------------------------------
void sub_56210_process_command_line(int argc, char** argv)//237210
{
	int32_t x_DWORD_355208;//3551CE+3A DWORD
	x_BYTE x_BYTE_355234_hardisknumber;//harddrive//3551CE+66 BYTE
	x_BYTE x_BYTE_35520C;//3551CE+3e BYTE
	x_BYTE x_BYTE_355210_level;//3551CE+42 BYTE
	x_BYTE x_BYTE_355230;//3551CE+62 BYTE
	x_BYTE x_BYTE_355218;//3551CE+4a BYTE
	x_BYTE x_BYTE_355244_spellsedit;//aSpellsedit//3551CE+76 BYTE
	x_BYTE x_BYTE_355240_load_set_level;//3551CE+72 BYTE
	x_BYTE x_BYTE_35522C_nocd;//nocd//3551CE+5e BYTE
	x_BYTE x_BYTE_355224_showversion;//Showversion//3551CE+56 BYTE
	x_BYTE x_BYTE_355228_showversion2;//Showversion2//3551CE+5a BYTE
	x_BYTE x_BYTE_355214;//3551CE+46 BYTE
	x_BYTE x_BYTE_35521C_cc;//3551CE+4e BYTE
	x_BYTE x_BYTE_35523C_detectoff;//Detectoff//3551CE+6e BYTE
	x_BYTE x_BYTE_355220_langcheck;//Langcheck//3551CE+52 BYTE
	x_BYTE x_BYTE_355238_music2;//aMusic2//3551CE+6a BYTE

	uint8_t argnumber; // ebx
	char arg0; // dl
	char* v4; // edi
	char* v5; // esi
	//char v6; // al
	//char v7; // al
	//unsigned __int8 v8; // al
	//int v9; // ebx
	//char* v10; // esi
	//int result; // eax
	char actarg[512] = "\0"; // [esp+0h] [ebp-46h]
	//int v13; // [esp+80h] [ebp+3Ah]
	//char v14; // [esp+84h] [ebp+3Eh]
	//unsigned __int8 v15; // [esp+88h] [ebp+42h]
	//char v16; // [esp+8Ch] [ebp+46h]
	//char v17; // [esp+90h] [ebp+4Ah]
	//char v18; // [esp+94h] [ebp+4Eh]
	//char v19; // [esp+98h] [ebp+52h]
	//char v20; // [esp+9Ch] [ebp+56h]
	//char v21; // [esp+A0h] [ebp+5Ah]
	//char v22; // [esp+A4h] [ebp+5Eh]
	//char v23; // [esp+A8h] [ebp+62h]
	//unsigned __int8 v24; // [esp+ACh] [ebp+66h]
	//char v25; // [esp+B0h] [ebp+6Ah]
	//char v26; // [esp+B4h] [ebp+6Eh]
	//char v27; // [esp+B8h] [ebp+72h]
	//char v28; // [esp+BCh] [ebp+76h]
	//fix it:
	argnumber = 1;
	x_DWORD_355208/* v13*/ = -1;//3551CE+3A DWORD //355208
	x_BYTE_355234_hardisknumber/*v24*/ = 'C';//harddrive//3551CE+66 BYTE
	x_BYTE_35520C/*v14*/ = 0;//3551CE+3e BYTE
	x_BYTE_355210_level/*v15*/ = 0;//3551CE+42 BYTE
	x_BYTE_355230/*v23*/ = 0;//3551CE+62 BYTE
	x_BYTE_355218/*v17*/ = 0;//3551CE+4a BYTE
	x_BYTE_355244_spellsedit/*v28*/ = 0;//aSpellsedit//3551CE+76 BYTE
	x_BYTE_355240_load_set_level/*v27*/ = 0;//3551CE+72 BYTE
	x_BYTE_35522C_nocd/*v22*/ = 0;//nocd//3551CE+5e BYTE
	x_BYTE_355224_showversion/*v20*/ = 0;//Showversion//3551CE+56 BYTE
	x_BYTE_355228_showversion2/*v21*/ = 0;//Showversion2//3551CE+5a BYTE
	x_BYTE_355214/*v16*/ = 0;//3551CE+46 BYTE
	x_BYTE_35521C_cc/*v18*/ = 0;//3551CE+4e BYTE
	x_BYTE_35523C_detectoff/*v26*/ = 0;//Detectoff//3551CE+6e BYTE
	x_BYTE_355220_langcheck/*v19*/ = 0;//Langcheck//3551CE+52 BYTE
	x_BYTE_355238_music2/*v25*/ = 0;//aMusic2//3551CE+6a BYTE
	sub_89B60_aplicate_setting(7u);
	while (argnumber < argc)
	{
		strcpy(actarg, argv[argnumber]);
		arg0 = argv[argnumber][0];// **(x_BYTE **)((int)argv + 4 * v2);
		if (arg0 == '-' || arg0 == '/')
		{
			v4 = actarg;
			v5 = argv[argnumber] + 1;//(char *)(*(x_DWORD *)((int)argv + 4 * v2) + 1);
			strcpy(v4, v5);
		}
		if (_stricmp("network", (char*)actarg))
		{
			if (!_stricmp("editor", (char*)actarg))
			{
				config_EDITOR = true;
			}
			else if (!_stricmp("testlevel", (char*)actarg))
			{
				config_LOAD_EDITED_LEVEL = true;
			}
			else if (!_stricmp("reglevel", (char*)actarg))
			{
				test_regression_level = atoi(argv[++argnumber]);
			}
			else if (!_stricmp("level", (char*)actarg))
			{
				x_BYTE_355210_level = atoi(argv[++argnumber]);//2372C4 - 279F5D
				x_BYTE_D419C_level_num = x_BYTE_355210_level;
				x_BYTE_355240_load_set_level = 1;
			}
			else if (!_stricmp("harddrive", (char*)actarg))
			{
				x_BYTE_355234_hardisknumber = x_toupper(argv[++argnumber][0]);//**(x_BYTE **)((int)argv + 4 * ++v2);
				//x_BYTE_355234_hardisknumber = x_toupper(x_BYTE_355234_hardisknumber);
				//x_BYTE_355234_hardisknumber = v8;
				if (x_BYTE_355234_hardisknumber < 'A' || x_BYTE_355234_hardisknumber > 'Z')
					x_BYTE_355234_hardisknumber = 'C';
			}
			else if (!_stricmp("skipscreens", (char*)actarg))
			{
				x_BYTE_D41AD_skip_screen = 1;
			}
			else if (!_stricmp("nocd", (char*)actarg))
			{
				x_BYTE_35522C_nocd = 1;
			}
			else if (!_stricmp("showversion", (char*)actarg))
			{
				x_BYTE_355224_showversion = 1;
			}
			else if (!_stricmp("showversion2", (char*)actarg))
			{
				x_BYTE_355228_showversion2 = 1;
			}
			else if (!_stricmp("detectoff", (char*)actarg))
			{
				x_BYTE_35523C_detectoff = 1;
			}
			else if (!_stricmp("langcheck", (char*)actarg))
			{
				x_BYTE_355220_langcheck = 1;
			}
			else if (!_stricmp("vio", (char*)actarg))
			{
				//v9 = argnumber + 1;
				//v10 = argv[argnumber + 1];//*(x_DWORD *)((int)argv + 4 * v9);

				unk_18058Cstr.x_WORD_1805C4_vio1_analog_joy = atoi(argv[argnumber + 1]);
				argnumber += 2;
				unk_18058Cstr.x_WORD_1805C6_vio2_analog_joy = atoi(argv[argnumber]/* *(char**)((int)argv + 4 * v2)*/);
				sub_89B60_aplicate_setting(1u);
			}
			else if (!_stricmp("extern", (char*)actarg))
			{
				*(uint32_t*)&xx_array_E36C4[0] = atoi(argv[++argnumber]/* *(char **)((int)argv + 4 * ++v2)*/);
				myprintf("External ctrl at %ul\n");
				sub_89B60_aplicate_setting(6u);
			}
			else if (!_stricmp("VFX1", (char*)actarg))
			{
				sub_89B60_aplicate_setting(2u);
			}
			else if (!_stricmp("cc", (char*)actarg))
			{
				x_BYTE_35521C_cc = atoi(argv[++argnumber]/* *(char**)((int)argv + 4 * ++v2)*/);
			}
			else if (!_stricmp("spellsedit", (char*)actarg))
			{
				x_BYTE_355244_spellsedit = 1;
			}
			else if (!_stricmp("music2", (char*)actarg))
			{
				x_BYTE_355238_music2 = 1;
			}
			else if (!_stricmp("client", (char*)actarg))//set to all one computer adress
			{
				if (!Iam_server)
				{
					Iam_client = true;
					strcpy(serverIP, (char*)argv[++argnumber]);
					NetworkPort = atoi(argv[++argnumber]);
					if (NetworkPort < 0)NetworkPort = 0;
					if (NetworkPort > 99999)NetworkPort = 99999;
					if (ServerPort == -1)ServerPort = NetworkPort;
				}
			}
			else if (!_stricmp("server", (char*)actarg))//set to all one computer adress
			{
				if (!Iam_client)
				{
					Iam_server = true;
					strcpy(serverIP, (char*)argv[++argnumber]);
					NetworkPort = atoi(argv[++argnumber]);
					if (NetworkPort < 0)NetworkPort = 0;
					if (NetworkPort > 99999)NetworkPort = 99999;
					if (ServerPort == -1)ServerPort = NetworkPort;
				}
			}
			else if (!_stricmp("otherserverport", (char*)actarg))//set to all one computer adress
			{
				ServerPort = atoi(argv[++argnumber]);
				if (ServerPort < 0)ServerPort = 0;
				if (ServerPort > 99999)ServerPort = 99999;
			}
		}
		argnumber++;
	}
	if (!x_BYTE_35522C_nocd)//if cd
		sub_86A00_some_allocs();//fix it -not at now
	//x_D41A0_BYTEARRAY_0 = (uint8_t*)sub_83CD0_malloc2(0x36e17);//2A51A0
	/*if (!x_D41A0_BYTEARRAY_0)
	{
		myprintf("ERROR: CANT ALLOC CARPET MEMORY.");
		exit(-1);
	}*/

	/*x_D41A0_BYTEARRAY_4 = (uint8_t*)sub_83CD0_malloc2(0x9693);
	if (!x_D41A0_BYTEARRAY_4)
	{
		sub_83E80_freemem4(x_D41A0_BYTEARRAY_0);
		myprintf("ERROR: CANT ALLOC TAPE MEMORY.");
		exit(-1);
	}*/

	/*memset((void*)x_D41A0_BYTEARRAY_0[0], 0, 224791);
	memset((void*)x_DWORD_D41A4, 0, 38547);*/
	memset((void*)&D41A0_0, 0, sizeof(D41A0_0));
	//clean_x_D41A0_BYTEARRAY_0();

	//memset((void*)x_D41A0_BYTEARRAY_4, 0, 0x9693);

	if (x_BYTE_35520C)
	{
		x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 |= 0x8;
		x_D41A0_BYTEARRAY_4_struct.setting_byte3_24 |= 0x8;
	}
	if (x_BYTE_355230)
		x_D41A0_BYTEARRAY_4_struct.setting_byte3_24 |= 2u;
	if (x_BYTE_355218)
		x_D41A0_BYTEARRAY_4_struct.setting_byte3_24 |= 8u;
	if (x_BYTE_355244_spellsedit)
		x_D41A0_BYTEARRAY_4_struct.setting_byte3_24 |= 0x10u;
	if (!x_BYTE_35522C_nocd && x_BYTE_E2A28_speek)
		x_D41A0_BYTEARRAY_4_struct.setting_byte3_24 |= 0x40u;
	if (x_BYTE_355240_load_set_level)
		x_D41A0_BYTEARRAY_4_struct.setting_byte3_24 |= 0x80u;
	if (x_BYTE_355214)
		x_D41A0_BYTEARRAY_4_struct.setting_byte2_23 |= 2u;
	if (x_BYTE_355224_showversion)
		x_D41A0_BYTEARRAY_4_struct.setting_byte4_25 |= 8u;
	if (x_BYTE_355220_langcheck)
		x_D41A0_BYTEARRAY_4_struct.setting_byte4_25 |= 0x10u;
	if (x_BYTE_355238_music2)
		x_D41A0_BYTEARRAY_4_struct.setting_byte4_25 |= 0x40u;
	if (x_BYTE_35523C_detectoff)
		autoScanForSoundHardware_E379B = true;
	//result = x_D41A0_BYTEARRAY_4;
	x_D41A0_BYTEARRAY_4_struct.levelnumber_43w = x_BYTE_355210_level;
	x_D41A0_BYTEARRAY_4_struct.setting_45w = x_DWORD_355208;
	if (x_BYTE_35521C_cc)
		x_D41A0_BYTEARRAY_4_struct.setting_byte4_25 = (x_D41A0_BYTEARRAY_4_struct.setting_byte4_25 | 4) & 0xFD;
	if (x_BYTE_355228_showversion2)
	{
		PrintHeader_70890();
		sub_5BC20();
		exit(1);
	}
	//return (int)x_D41A0_BYTEARRAY_4;
}

//----- (0009B234) --------------------------------------------------------
int sub_9B234(int* a1)//27C234
{
	int result; // eax

	if (*a1)
	{
		//fix  sub_9C58C(*a1);
		allert_error();
		//fix
		result = x_free((void*)*a1);
		*a1 = 0;
	}
	return result;
}


//----- (0008B770) --------------------------------------------------------
int sub_8B770()//26c770
{
	return sub_9B234((int*)unk_18058Cstr.unk_1805CE);
}

//----- (00075900) --------------------------------------------------------
void sub_75900()
{
	sub_75AB0();
}

//----- (0008B5A0) --------------------------------------------------------
int sub_8B5A0()//26c5a0
{
	if (x_BYTE_E36D1)
	{
		switch (unk_18058Cstr.x_WORD_1805C2_joystick)
		{
		case 1:
			sub_8B770();
			break;
		case 9:
			sub_75900();
			break;
		default:
			break;
		}
		x_BYTE_E36D1 = 0;
	}
	return 0;
}

//----- (00056730) --------------------------------------------------------
int sub_56730_clean_memory()//237730
{
	return sub_8B5A0();
}

//----- (0005C490) --------------------------------------------------------
void sub_5C490_testers_info()//23d490
{
	//int result; // eax
	int v1; // edx

	myprintf("\n");
	myprintf("***********************************************************\n");
	myprintf("*   TESTERS: WRITE DOWN THE ABOVE VERSION DATE AND TIME.  *\n");
	myprintf("*   Press RETURN to continue...                           *\n");
	myprintf("***********************************************************\n");
	do
	{
		if (x_DWORD_E3BB0 <= 0 || x_DWORD_E3BB8 & 4 || *x_DWORD_E3BAC == 13 || *x_DWORD_E3BAC == 26)
		{
			v1 = fgetc((FILE*)&x_DWORD_E3BAC);
		}
		else
		{
			v1 = *x_DWORD_E3BAC;
			x_DWORD_E3BAC++;
			x_DWORD_E3BB0--;
		}
	} while (v1 != 10);
}

//----- (00053CF0) --------------------------------------------------------
int sub_53CF0_access(const char* a1)//234cf0
{
	return myaccess(a1, 0);
}

//----- (00053D10) --------------------------------------------------------
uint8_t sub_53D10_create_nether_subdir(const std::string& gameDir, const std::string& subDir)
{
	char result; // al
	if ((signed __int16)sub_53CF0_access(gameDir.c_str()) <= -1 && mymkdir(gameDir.c_str()))//234D6A - 234CF0 | 279D30
		return 2;
	std::string fullDir = std::string(gameDir) + "/" + std::string(subDir);
	if ((sub_53CF0_access(fullDir.c_str()) & 0x8000u) != 0 && mymkdir(fullDir.c_str()))
		result = 2;
	else
		result = 3;
	return result;
}

//----- (000560D0) --------------------------------------------------------
void sub_560D0_create_sound_dir()//2370d0
{
	FILE* diginifile2; // eax
	FILE* diginifile; // ebx
	FILE* mdiini; // eax
	FILE* mdiini2; // ebx
	//printbuffer char v5; // [esp+0h] [ebp-2Ah]
	//printbuffer2 char v6; // [esp+80h] [ebp+56h]
	//char v7; // [esp+A8h] [ebp+7Eh]

	//v7 = 1;
	sprintf(printbuffer, "DEVICE\t\tNone\r\nDRIVER\t\tNone\r\nIO_ADDR\t\t-1\r\nIRQ\t\t-1\r\nDMA_8_BIT\t\t-1\r\nDMA_16_BIT\t\t-1\r\n");
	std::string digPath = GetSubDirectoryFile(gameFolder, "SOUND", "DIG.INI");
	diginifile2 = DataFileIO::CreateOrOpenFile(digPath.c_str(), 512);
	if (diginifile2 == NULL)
	{
		diginifile = DataFileIO::CreateOrOpenFile(digPath.c_str(), 546);
		if (diginifile != NULL)
		{
			WriteFile_98CAA(diginifile, (uint8_t*)printbuffer, strlen(printbuffer));
			DataFileIO::Close(diginifile);
		}
	}
	else
	{
		DataFileIO::Close(diginifile2);
	}
	std::string mdiPath = GetSubDirectoryFile(gameFolder, "SOUND", "MDI.INI");
	mdiini = DataFileIO::CreateOrOpenFile(mdiPath.c_str(), 512);
	if (mdiini == NULL)
	{
		mdiini2 = DataFileIO::CreateOrOpenFile(mdiPath.c_str(), 546);
		if (mdiini2 != NULL)
		{
			WriteFile_98CAA(mdiini2, (uint8_t*)printbuffer, strlen(printbuffer));
			DataFileIO::Close(mdiini2);
		}
	}
	else
	{
		DataFileIO::Close(mdiini);
	}
	//return v7;
}

//----- (00053EF0) --------------------------------------------------------
bool sub_53EF0_fileexist(const char* path, const char* path2)//234ef0//fix a2
{
	FILE* testfile1; // eax
	FILE* testfile2; // eax
	bool result = false; // [esp+0h] [ebp-4h]
	testfile1 = DataFileIO::CreateOrOpenFile(path, 512);
	testfile2 = DataFileIO::CreateOrOpenFile(path2, 512);
	if (testfile1 == NULL || testfile2 == NULL)
		result = true;
	if (testfile1 != NULL)
		DataFileIO::Close(testfile1);
	if (testfile2 != NULL)
		DataFileIO::Close(testfile2);
	return result;
}

//----- (00053F60) --------------------------------------------------------
bool sub_53F60(const char* a1)//234f60
{
	return myaccess(a1, 0) != NULL;//234F69 - 279CF2
}

//----- (00053F80) --------------------------------------------------------
char LoadFilesFromCDAndGameData(const char* cdPath, const char* gamePath, const char* fileName)//234f80
{
	FILE* file1; // ebx
	FILE* file2; // esi
	int v5; // eax
	int v6; // edi
	FILE* file3; // ebx
	FILE* file4; // edi
	signed int v10; // ebx
	int v11; // eax
	int v12; // esi
	int v13; // esi
	int v14; // eax

	char printbuffer[512] = { '\0' };//char v15; // [esp+0h] [ebp-194h]
	char printbuffer2[512] = { '\0' };//char v16; // [esp+90h] [ebp-104h]
	// pdwScreenBuffer_351628 - readbuffer
	//printbuffer - char v17; // [esp+120h] [ebp-74h]
	int v18; // [esp+184h] [ebp-10h]
	int v19; // [esp+188h] [ebp-Ch]
	int v20; // [esp+18Ch] [ebp-8h]
	//FILE* v21; // [esp+190h] [ebp-4h]

	if (!sub_53F60(cdPath))//234F90 - 234F60
		return 2;
	if (!sub_53F60(gamePath))//234FA5 - 234F60
		return 1;
	sprintf(printbuffer, "%s/%s.TAB", cdPath, fileName);//234FCA - 269F3D5
	sprintf(printbuffer2, "%s/%s.TAB", gamePath, fileName);//234FE3 - 269F3D5
	file1 = DataFileIO::CreateOrOpenFile(printbuffer, 0x200);//234FF7 - 279817
	if (file1 == NULL)//tady asi bude nerovnost
		return 3;
	file2 = DataFileIO::CreateOrOpenFile(printbuffer2, 0x222);//235012 - 279817
	if (file2 == NULL)
		return 3;
	v5 = DataFileIO::FileLengthBytes(file1);
	v6 = DataFileIO::Read(file1, readbuffer, v5);
	v19 = WriteFile_98CAA(file2, readbuffer, v6);
	DataFileIO::Close(file1);
	DataFileIO::Close(file2);
	if (v6 != v19)
		return 3;
	sprintf(printbuffer, "%s/%s.DAT", cdPath, fileName);
	sprintf(printbuffer2, "%s/%s.DAT", gamePath, fileName);
	file3 = DataFileIO::CreateOrOpenFile(printbuffer, 512);
	//v21 = v8;
	if (file3 == NULL)
		return 3;
	file4 = DataFileIO::CreateOrOpenFile(printbuffer2, 546);
	if (file4 == NULL)
		return 3;
	v10 = DataFileIO::FileLengthBytes(file3);
	v18 = v10;
	v20 = 0;
	while (v10)
	{
		if (v10 <= 64000)
			v11 = v10;
		else
			v11 = 64000;
		v12 = DataFileIO::Read(file3, readbuffer, v11);
		v10 -= v12;
		if (WriteFile_98CAA(file4, readbuffer, v12) != v12)
		{
			DataFileIO::Close(file3);
			DataFileIO::Close(file4);
			return 3;
		}
		v20 += v12;
		v13 = v20;
		settextposition(x_DWORD_F4720.x, x_DWORD_F4720.y);
		v14 = 100 * v13 / v18;
		if (v14 > 100)
			v14 = 100;
		sprintf(printbuffer, "%d%c", v14, 37);
		outtext(printbuffer);
	}
	DataFileIO::Close(file3);
	DataFileIO::Close(file4);
	memset(readbuffer, 0, 64000);
	return 0;
}

//----- (00054200) --------------------------------------------------------
char sub_54200_create_user_directiores()//235200
{
	int8_t v0l; // bx
	int8_t v0h; // bx
	//int v1; // eax
	//int v2; // edx
	//int v3; // eax
	//int v4; // edx
	//int v5; // eax
	//int v6; // edx
	//int v7; // eax
	//int v8; // edx
	//char* buffer; // [esp+0h] [ebp-2h]
	//char v11; // [esp+40h] [ebp+3Eh]
	//int v12; // [esp+80h] [ebp+7Eh]

	// fix if begin
	//v2 = 0;
	//v4 = 0;
	//v6 = 0;
	v0h = 0;
	//v8 = 0;

	readbuffer[0] = 0x0;
	// end

	printbuffer[0] = 0;
	outtext("Checking Setup Version ..");//23521B - 29EBED
	v0l = 1;
	std::string versionPath = GetSubDirectoryFile(gameFolder, "CDATA", "VERSION.DAT");//235250 - 26F3D5
	DataFileIO::ReadFileAndDecompress(versionPath.c_str(), &readbuffer);//235260 - 234E60
	if (readbuffer[0] != 60)
		v0h = 1;
	outtext("\n");//235277 - 29EBED
	std::string cDataTmapsPath0 = GetSubDirectoryFile(gameFolder, "CDATA", "TMAPS0-0.DAT");//2352A8 - 26F3D5
	std::string dataTmapsPath0 = GetSubDirectoryFile(cdFolder, "DATA", "TMAPS0-0.DAT");//2352BE - 26F3D5
	if (v0h || sub_53EF0_fileexist(cDataTmapsPath0.c_str(), dataTmapsPath0.c_str()))
	{
		x_BYTE_D41AD_skip_screen = 0;//fix can not exit from setup
		//fix it - whne file not exist
		outtext("Creating Setup Directories 1 ..");//2352E4 -29EBED
		x_DWORD_F4720 = gettextposition();//2352EC - 29E953

		std::string cDataTmapsPath = GetSubDirectoryPath(gameFolder, "CDATA");//23531A - 26F3D5
		std::string dataTmapsPath = GetSubDirectoryPath(cdFolder, "DATA");//23531A - 26F3D5
		if (LoadFilesFromCDAndGameData(dataTmapsPath.c_str(), cDataTmapsPath.c_str(), "TMAPS0-0"))// this needs to create something // 235330 -234F80 // create tmaps00 tab / data
			v0l = 0;
		outtext("\n");//235343 - 29EBED
	}
	if (v0l)
	{
		std::string cDataTmapsPath1 = GetSubDirectoryFile(gameFolder, "CDATA", "TMAPS1-0.DAT");//23537C - 26F3D5
		std::string dataTmapsPath1 = GetSubDirectoryFile(cdFolder, "DATA", "TMAPS1-0.DAT");//235392 - 26F3D5
		if (v0h || sub_53EF0_fileexist(cDataTmapsPath1.c_str(), dataTmapsPath1.c_str()))
		{
			//fix it - whne file not exist
			outtext("Creating Setup Directories 2 ..");
			x_DWORD_F4720 = gettextposition(/*v3, v4, v0*/);
			std::string cDataTmapsPath = GetSubDirectoryPath(gameFolder, "CDATA");//2353EE - 26F3D5
			std::string dataTmapsPath = GetSubDirectoryPath(cdFolder, "DATA");//23531A - 26F3D5
			if (LoadFilesFromCDAndGameData(dataTmapsPath.c_str(), cDataTmapsPath.c_str(), "TMAPS1-0"))//235404 - 234F80
				v0l = 0;
			outtext("\n");
		}
	}
	if (v0l)
	{
		std::string cDataTmapsPath2 = GetSubDirectoryFile(gameFolder, "CDATA", "TMAPS2-0.DAT");
		std::string dataTmapsPath2 = GetSubDirectoryFile(cdFolder, "DATA", "TMAPS2-0.DAT");
		if (v0h || sub_53EF0_fileexist(cDataTmapsPath2.c_str(), dataTmapsPath2.c_str()))
		{
			//fix it - whne file not exist
			outtext("Creating Setup Directories 3 ..");
			x_DWORD_F4720 = gettextposition(/*v5, v6, v0*/);
			std::string cDataTmapsPath = GetSubDirectoryPath(gameFolder, "CDATA");
			std::string dataTmapsPath = GetSubDirectoryPath(cdFolder, "DATA");
			if (LoadFilesFromCDAndGameData(dataTmapsPath.c_str(), cDataTmapsPath.c_str(), "TMAPS2-0"))
				v0l = 0;
			outtext("\n");
		}
	}
	if (v0l && !((x_D41A0_BYTEARRAY_4_struct.setting_byte1_22) & 8))//test it
	{
		outtext("Setting Up Levels ..");
		x_DWORD_F4720 = gettextposition(/*v7, v8, v0*/);
		std::string clevelsPath = GetSubDirectoryPath(gameFolder, "CLEVELS");
		std::string levelPath = GetSubDirectoryPath(cdFolder, "LEVELS");
		if (LoadFilesFromCDAndGameData(levelPath.c_str(), clevelsPath.c_str(), "LEVELS"))
			v0l = 0;
		outtext("\n");
	}
	if (v0l && v0h)
	{
		versionPath = GetSubDirectoryFile(gameFolder, "CDATA", "VERSION.DAT");
		readbuffer[0] = 60;
		sub_98C48_open_nwrite_close(versionPath.c_str(), readbuffer, 4);
	}

	return v0l;
}

//----- (0008C2CD) --------------------------------------------------------
void sub_8C2CD()//26d2cd
{
	;
}

//----- (0005C430) --------------------------------------------------------
void NetworkAllocation2_5C430()//23d430
{
	x_D41A0_BYTEARRAY_4_struct.isNetwork_216w = NetworkAllocation_74556();//255556 push ebp 355250
}

//----- (0006FDA0) --------------------------------------------------------
void sub_6FDA0()//fix//250da0
{
	//int v0; // eax
	//void(*v1)(); // eax
	//__int16 v2; // dx
	//int result; // eax

	//fix it
	//v2 = 0;
	//fix it

	//v0 = (int)x_D41A0_BYTEARRAY_4;
	x_D41A0_BYTEARRAY_4_struct.dwordindex_2388 = 10022;
	x_D41A0_BYTEARRAY_4_struct.dwordindex_2392 = 0;
	//v1 = dos_getvect(8);
	//x_WORD_F5334 = v2;
	//x_DWORD_F5330 = v1;
	/* __outx_BYTE(0x43u, 0x36u);
	 __outx_BYTE(0x40u, x_D41A0_BYTEARRAY_4[0x954]);
	 __outx_BYTE(0x40u, x_D41A0_BYTEARRAY_4[0x954] >> 8);*/
	 //BYTE1(result) = 1;
	x_BYTE_DB734 = 1;
	//return result;
}

//----- (00046DD0) --------------------------------------------------------
void /*__fastcall*/ sub_46DD0_init_sound_and_music(/*int a1, int a2, char* a3*/)//227DD0
{
	//char* v3; // eax
	//int v4; // edx
	//char v5; // bl
	//char v6; // dl
	//char v8[512]; // [esp+0h] [ebp-40h]
	// fix if begin
	//v4 = 0;
	// end

	sub_83CC0(20);
	if (!x_BYTE_D4B50)
	{
		x_BYTE_D4B50 = 1;
		if ((x_D41A0_BYTEARRAY_4_struct.setting_byte1_22) & 0x40)//fix it
		{
			PrintTextMessage_70910((char*)"Sound Disabled\0");
			musicActive_E37FD = false;
			soundActive_E3799 = false;
			musicAble_E37FC = false;
			soundAble_E3798 = false;
		}
		else
		{
			sprintf(printbuffer, "%s/%s", gameDataPath.c_str(), "sound");
			PrintTextMessage_70910((char*)"Initialise Sound\0");
			InitSoundAndMusic_90FD0(/*v3*//*v3, v4, a3*/); //fix it sound off here!
			if (!soundActiveL_E2A14)
				myprintf("ERROR: NOT ENOUGH MEMORY FOR SOUNDS\n");
			if ((x_D41A0_BYTEARRAY_4_struct.setting_byte4_25) & 0x40)//fix it
				InitMusicBank_8EAD0(1);
		}
		if (soundAble_E3798 || musicAble_E37FC)
		{
			//x_DWORD_F42A4_sound_timer = sub_92600_AIL_register_timer(sub_46820_simple_timer);
			//sub_92930_AIL_set_timer_frequency(x_DWORD_F42A4_sound_timer, 0x78u);
			//sub_92BA0_AIL_start_timer(x_DWORD_F42A4_sound_timer);
			x_BYTE_D4B51 = 1;
		}
		else
		{
			sub_6FDA0();
		}
		//v5 = x_BYTE_E3798_sound_active2;
		if (!soundAble_E3798 && !musicAble_E37FC && x_BYTE_E2A28_speek)
		{
			sub_86860_speak_Sound(x_WORD_1803EC);
			sub_86BD0_freemem1();
			//v6 = x_D41A0_BYTEARRAY_4_struct.setting_byte3_24 & 0xBF;
			x_BYTE_E2A28_speek = soundAble_E3798;
			(x_D41A0_BYTEARRAY_4_struct.setting_byte3_24) &= 0xBF;
		}
	}
	sub_83CC0(21);
}

//----- (0005B8D0) --------------------------------------------------------
void Initialize()//23c8d0
{
	PrintHeader_70890();//23C8D6 - 251890
	if ((x_D41A0_BYTEARRAY_4_struct.setting_byte4_25) & 8)
		sub_5C490_testers_info();//23C8E6 - 23D490

	Logger->debug("Init:Begin of creating dirs\n");

	std::string exepath = get_exe_path();

	if (sub_53D10_create_nether_subdir(gameDataPath, "SAVE") != 3//23C906 - 234D10
		|| sub_53D10_create_nether_subdir(gameDataPath, "CDATA") != 3//23C931 - 234D10
		|| sub_53D10_create_nether_subdir(gameDataPath, "CLEVELS") != 3//23C95C - 234D10
		|| sub_53D10_create_nether_subdir(gameDataPath, "SOUND") != 3//23C987 - 234D10
		|| sub_53D10_create_nether_subdir(gameDataPath, "LANGUAGE") != 3//23C9B2 - 234D10
		|| sub_53D10_create_nether_subdir(gameDataPath, "SHOTS") != 3)//23C9DD - 234D10
	{
		myprintf("Error creating setup directories.\n");
		exit(-1);
	}

	SetCDFilePaths(cdDataPath.c_str(), pstr);

	Logger->debug("Init:End of creating dirs\n");

	sub_560D0_create_sound_dir();//23C9ED - 2370D0
	sub_5BCC0_set_any_variables1();//23C9F2 - 23CCC0
	if (!sub_5BF50_load_psxdata())//23C9F7 - 23CF50 //something with files about their loading, or just a set of Palettes
		exit(-1);
	sub_5C1B0_set_any_variables2();//23CA05 - 23D1B0
	if (!sub_54200_create_user_directiores())//23CA0A - 235200 //the percentages go here too
	{
		myprintf("ERROR COPYING DATA FILES TO LOCAL DRIVE \n");
		exit(-1);
	}
	sub_71410_process_tmaps();//252410

	//sub_6EB90((unsigned int **)*filearray_2aa18c);//24FB90
	//2AA18C
	CreateIndexes_6EB90(&filearray_2aa18c[filearrayindex_POINTERSDATTAB]);//24fb90
	//sub_6EB90((unsigned int **)&*filearray_2aa18c[0x60]);//24FB90
	//2AA1EC
	CreateIndexes_6EB90(&filearray_2aa18c[filearrayindex_BUILD00DATTAB]);//24fb90 adress 0x23ca2e

	sub_101C0();//23CA36 - 1f11c0 //sem se to vubec nedostane
	//zkontrolovat inicializaci

	//*xadataspellsdat.colorPalette_var28 = (uint8_t*)malloc(10000);//fix it

	/*if ( !(x_D41A0_BYTEARRAY_4_struct.setting_byte3_24 & 8) )
	//fix it //x_D41A0_BYTEARRAY_4_struct.setting_byte3_24=50CF38 =0
	*/
	//*xadataspellsdat.colorPalette_var28 = (uint8_t*)malloc(50000);
	if (!(x_D41A0_BYTEARRAY_4_struct.setting_byte3_24 & 8))
	{
		char spellDataPath[MAX_PATH];
		sprintf(spellDataPath, "%s/%s", cdDataPath.c_str(), "DATA/SPELLS.DAT");
		DataFileIO::ReadFileAndDecompress(spellDataPath, xadataspellsdatx.colorPalette_var28);//234e60 buffer - 2ab818
	}

	//v0 = sub_5C0A0();//23d0a0g
	SetDefaultSpells_5C0A0();
	if (x_BYTE_D4B80)
		exit(-1);
	x_BYTE_D4B80 = 1;
	x_WORD_180660_VGA_type_resolution = 1;//fix it
	if (x_WORD_180660_VGA_type_resolution & 1)
		ClearGraphicsBuffer_72883((void*)pdwScreenBuffer_351628, 320, 200, 0);//253883
	else
		ClearGraphicsBuffer_72883((void*)pdwScreenBuffer_351628, 640, 480, 0);//2538a9

	//VGA_Write_basic_Palette(basic_pal_buffer);

  //sub_90B27_VGA_pal_fadein_fadeout(*xadatapald0dat2.colorPalette_var28, 0x20u, 0);

	sub_90B27_VGA_pal_fadein_fadeout(0, 0x10u, 0);//271B27 tady se zapina vga
	//fixed
	ClearGraphicsBuffer_72883((void*)pdwScreenBuffer_351628, 640, 480, 0);//fixed
	//sub_75200_VGA_Blit640(480);//debug
	//sub_75200_VGA_Blit640(480);//debug
	//fixed
  //*xadatapald0dat2.colorPalette_var28 = (uint8_t*)malloc(10000);//fix it
	memset((void*)*xadatapald0dat2.colorPalette_var28, 0, 768);
	sub_41A90_VGA_Palette_install((TColor*)*xadatapald0dat2.colorPalette_var28);//222a90
	x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 = 0;//fix it
	if ((x_D41A0_BYTEARRAY_4_struct.setting_byte1_22) & 0x110)//2a51a4 // 38cf50+16 //00 druh grafiky/inicializace
	{
		if (x_WORD_180660_VGA_type_resolution & 1)
			sub_90D6E_VGA_set_video_mode_320x200_and_Palette((TColor*)*xadatapald0dat2.colorPalette_var28);
		else
			sub_90E07_VGA_set_video_mode_640x480_and_Palette((TColor*)*xadatapald0dat2.colorPalette_var28);
		memset((void*)*xadatapald0dat2.colorPalette_var28, 0, 768);
		sub_41A90_VGA_Palette_install((TColor*)*xadatapald0dat2.colorPalette_var28);//222a90
	}
	else
	{
		sub_90B27_VGA_pal_fadein_fadeout(0, 0x10u, 0);
		x_WORD_180660_VGA_type_resolution = 1;//fix it
		if (x_WORD_180660_VGA_type_resolution & 1)
			sub_90D6E_VGA_set_video_mode_320x200_and_Palette((TColor*)pdwScreenBuffer_351628);//3aa0a4
		else
			sub_90E07_VGA_set_video_mode_640x480_and_Palette((TColor*)pdwScreenBuffer_351628);
		memset((void*)*xadatapald0dat2.colorPalette_var28, 0, 0x300);
		sub_41A90_VGA_Palette_install((TColor*)*xadatapald0dat2.colorPalette_var28);//222a90
	}
	if (x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 & 8)
		sub_8C2CD();
	else
		sub_753D0();
	// ?keyboard init

	soundActive_E3799 = soundAble_E3798;
	musicActive_E37FD = musicAble_E37FC;
	sub_8CEDF_install_mouse();
	if (!x_DWORD_E3768)
	{
		sub_5BC20();
		myprintf("ERROR : MOUSE DRIVER NOT INSTALLED\n");
		exit(-1);
	}
	//mouse init

	//pointersdat_buffer = xadatapointersdat.colorPalette_var28;//eb394 - 2bc394 -446f1{set in 23cf50}

	//nema to byt buffer obrazovky?! ---

	sub_8CD27_set_cursor((*filearray_2aa18c[filearrayindex_POINTERSDATTAB].posistruct)[0]);//anything with vga, maybe mouse cursor//26dd27 //xadatapointersdat asi 1a6f44
	//1a6f44->1a6578 00 00 pointer->0b0005a1a1a1a1a10005a1b413aaa1
	//nastavi se zde:sub_6EB90(&filearray_2aa18c[filearrayindex_POINTERSDATTAB]);//24fb90

	//pak je to v 809380

	//VGA_Set_Cursor(*(*filearray_2aa18c[filearrayindex_POINTERSDATTAB].posistruct),0);

	NetworkAllocation2_5C430(); //23d430
	sub_46DD0_init_sound_and_music(/*v2, v3, (char*)filearray_2aa18c[filearrayindex_POINTERSDATTAB].begin_buffer*/);//init sound and music//227DD0
	//x_WORD_E2A14_sound_activel = 0;x_BYTE_E3798_sound_active2 = 0;//debug tittes

	//test_x_D41A0_BYTEARRAY_0();
}

//----- (00055F70) --------------------------------------------------------
int sub_main(int argc, char** argv, char**  /*envp*/)//236F70
{
	int exitCode = 0;
	try
	{
		begin_plugin();

		preconvert();//rewrite and remove it later

		*xadataclrd0dat.colorPalette_var28 = (uint8_t*)malloc(4096);//fix it

		//*xadataspellsdat.colorPalette_var28 = (uint8_t*)malloc(50000);

		signed int v3; // edi
		unsigned __int16 v4; // si
		//skip __int16 v6; // [esp+0h] [ebp-1Ch]
		//__int16 v7; // [esp+Ch] [ebp-10h]

		//fix it
		v3 = 0;
		v4 = 0;
		//fix it

		//skip memset(&v6, 0, 28);//236F7F - 26D250
		//v7 = 0;
		//skip v6 = 0x3301;
	//removed  int386(0x21, (REGS*)&v6, (REGS*)&v6);//236F9D - 279D52 //INT 21,33 - Get/Set System Values (Ctl-Break/Boot Drive) AH = 33h AL = 01 to set Ctrl - Break checking flag
	//may be INT 33,1 Show Mouse Cursor see:https://www.equestionanswers.com/c/c-int33-mouse-service.php
		//skip signal(7, 1);//236FA9 - 279DC0
		//skip signal(4, 1);//236FB5 - 279DC0
		//skip signal(6, 1);//236FC1 - 279DC0

		printf("Reading Ini file");
		if (!readini()) exit(1);

		spdlog::level::level_enum level = spdlog::level::info;

#ifdef _DEBUG
		level = GetLoggingLevelFromString("Debug");
#else
		level = GetLoggingLevelFromString(loggingLevel.c_str());
#endif // _DEBUG
		InitializeLogging(level);

		if (assignToSpecificCores)
		{
#ifdef _MSC_VER
			SetThreadIdealProcessor(GetCurrentThread(), 0);
			DWORD_PTR dw = SetThreadAffinityMask(GetCurrentThread(), DWORD_PTR(1) << 0);
#endif
		}

		if (CommandLineParams.DoDisableGraphicsEnhance()) {
			Logger->debug("Disabling enhanced graphics");
			bigSprites = false;
			bigTextures = false;
			texturepixels = 32;
		}

		//Set Paths for game data
		Logger->debug("Getting Game data paths");
		gameDataPath = GetSubDirectoryPath(gameFolder);
		cdDataPath = GetSubDirectoryPath(cdFolder);
		bigGraphicsPath = GetSubDirectoryPath(bigGraphicsFolder);

		Logger->debug("Initializing graphics Width: {} Height: {}", windowResWidth, windowResHeight);
		VGA_Init(windowResWidth, windowResHeight, maintainAspectRatio, displayIndex);

		//char maindir[1024];
		Logger->info("Finding Game Data...");
		if (std::string mainfile = GetSubDirectoryFile(gameFolder, "CDATA", "TMAPS0-0.DAT"); !file_exists(mainfile.c_str()))//test original file
		{
			//myprintf("Original Game Data Not Found, find GOG iso file\n");
			/*char locexepath[1024];
			get_exe_path(locexepath);
			for (int i = 0;i < strlen(locexepath);i++)
			{
				if (locexepath[i] == '\\')
					locexepath[i] = '/';
			}
			sprintf(mainfile, "%s/%s%s", locexepath,gamepath, "/MC2.dat");
			sprintf(maindir, "%s/%s%s", locexepath, gamepath, "/extracted-game-files");
			//sprintf(mainfile, "%s", (char*)"c:\\prenos\\gparted-live-0.27.0-1-i686");*/
			//sprintf(maindir, "%s", (char*)"c:\\prenos\\ex");
			//if (!file_exists(mainfile))//test existing GOG cd iso file
			{
				Logger->error("Original game not found in {} folder", gameDataPath.c_str());
				mydelay(20000);
				exit(1);//iso not found
			}
			/*myprintf("GOG game iso cd founded!\n");
			sprintf(mainfile, "%s%s", gamepath, "/extracted-game-files\\data\\tmaps0-0.dat");
			if (file_exists(mainfile))
			{
				myprintf("I found extracted GOG game files!\n");
				sprintf(gamepath, "%s", maindir);
			}
			else
			{
				myprintf("Extracting GOG iso cd...\n");
				sprintf(mainfile, "%s/%s%s", locexepath, gamepath, "/MC2.dat");

				cd_iso_extract(mainfile, maindir);
				//cd_iso_extract((char*)"c:\\prenos\\MC2.dat.bin", maindir);

				//sprintf(mainfile, "%s%s", gamepath, "\\data\\tmaps0-0.dat");
				if (file_exists(mainfile))
				{
					myprintf("GOG iso cd extracted!\n");
					sprintf(gamepath, "%s", maindir);
				}
				else
				{
					myprintf("Any problem with GOG iso cd extracting\n");
					mydelay(3000);
					exit(1);//problem with file extracting
				}
			}	*/
		}
		else
		{
			Logger->info("Original Game Data Found!");
		}

		//dos_setvect(9, null_vector, 0);

		initposistruct();

		sub_56210_process_command_line(argc, argv);//236FD4 - 237210
		if (CommandLineParams.ModeTestNetwork()) {
			if (Iam_server || Iam_client)
				InitNetworkInfo();
		}

		//-init 0x2a51a4 je nekde tu
		if (CommandLineParams.DoCopySkipConfig()) {
			x_BYTE_D41AD_skip_screen = config_skip_screen;
		}

		Initialize();//236FDC - 23C8D0//rozdil 1E1000

		sub_46830_main_loop(/*0, */v3, v4);//227830

		sub_5BC20();//23CC20 //remove devices?
		sub_56730_clean_memory();//237730

		if (CommandLineParams.ModeTestNetwork()) {
			if (Iam_server || Iam_client)
			{
				EndMyNetLib();
				/*EndLibNetClient();
				if (Iam_server)
					EndLibNetServer();*/
			}
		}
	}
	catch (const std::exception& e)
	{
		Logger->critical("Critial Error: {}", e.what());
		exitCode = -1;
	}
	Logger->info("Exited Game");
	return exitCode;
}