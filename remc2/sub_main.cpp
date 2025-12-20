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