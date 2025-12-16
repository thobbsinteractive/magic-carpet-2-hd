#include "Terrain.h"

bool lowDiffHeightmap_D47DC = true;

uint16_t x_WORD_17B4E0; // 34c4e0

uint8_t mapTerrainType_10B4E0[65536]; // 256x256 //map array1
uint8_t mapHeightmap_11B4E0[65536]; // 256x256 //map array2 // heightmap
uint8_t mapShading_12B4E0[65536]; // 256x256 //map array3
uint8_t mapAngle_13B4E0[65536]; // 256x256 //map array4 // water //!!! find all comparation, change to int8_t and test it !!!
int16_t mapEntityIndex_15B4E0[65536]; // 256x256 //map array5

char x_BYTE_F2CD0x[7 * 7 * 7 * 7][2]; // 233cd0//4802 //4816

uint8_t x_BYTE_D41B7 = 44; // weak

int debugcounter_224959 = 0;
//----- (00043830) --------------------------------------------------------
void GenerateLevelMap_43830(type_str_2FECE* a2x)//224830
{
	x_WORD_17B4E0 = a2x->seed_0x2FEE5;
	D41A0_0.rand_0x8 = a2x->seed_0x2FEE5;
	memset((void*)mapEntityIndex_15B4E0, 0, 0x20000);
	sub_B5E70_decompress_terrain_map_level(x_WORD_17B4E0, a2x->offset_0x2FEE9, a2x->raise_0x2FEED, a2x->gnarl_0x2FEF1);

	sub_44DB0_truncTerrainHeight(mapEntityIndex_15B4E0, mapHeightmap_11B4E0);//225db0 //trunc and create

	memset((void*)mapEntityIndex_15B4E0, 0, 0x20000);
	sub_44E40(a2x->river_0x2FEF5, a2x->lriver_0x2FEF9);//225e40 //add any fields

	sub_45AA0_setMax4Tiles();//226aa0

	sub_440D0(a2x->snLin_0x2FF01);//2250d0

	sub_45060(a2x->snFlt_0x2FF05, a2x->bhLin_0x2FF09);//226060

	sub_44320();//225320

	sub_45210(a2x->snFlt_0x2FF05, a2x->bhLin_0x2FF09);//226210

	sub_454F0(a2x->source_0x2FEFD, a2x->rkSte_0x2FF11);//2264f0

	sub_45600(a2x->bhFlt_0x2FF0D);//226600

	sub_43FC0();//224fc0
	memset((void*)mapTerrainType_10B4E0, 0, 0x10000);
	sub_43970();//224970 // smooth terrain
	sub_43EE0();//224ee0 // add rivers

	sub_44580();//225580 //set angle of terrain
	if (isCaveLevel_D41B6)
		sub_43B40();//224b40 //change angle of terrain
	else
		sub_43D50();//224d50 //change angle of terrain
	sub_44D00();//225d00
}

//----- (000B5E70) --------------------------------------------------------
void sub_B5E70_decompress_terrain_map_level(__int16 a1, unsigned __int16 a2, __int16 a3, int32_t a4)//296e70
{
	uaxis_2d sumEnt;
	mapEntityIndex_15B4E0[a2] = a3;//32c4e0 //first seed
	for(int i = 7; i >= 0 ; i--)
	{
		sumEnt.word = a2;
		for (int j = 1 << (7 - i); j > 0; j--)
		{
			for (int k = 1 << (7 - i); k > 0; k--)
			{
				sub_B5EFA(1 << i, &sumEnt, a4, &a1);//355220
			}
			sumEnt.word += (2 * (1 << i)) << 8;
		}
		for (int j = 1 << (7 - i); j > 0; j--)
		{
			for (int k = 1 << (7 - i); k > 0; k--)
			{
				sub_B5F8F(1 << i, &sumEnt, a4, &a1);
			}
			sumEnt.word += (2 * (1 << i)) << 8;
		}
	}
}

//----- (00044DB0) --------------------------------------------------------
void sub_44DB0_truncTerrainHeight(int16_t mapEntityIndex_15B4E0[], uint8_t mapHeightmap_11B4E0[])//225db0 // map to heightmap
{
	int revMaxEnt = 0;
	uint32_t weightedVar;
	int maxEnt = -32000;
	int minEnt = 32000;
	for(int i = 0; i< 256 * 256; i++)
	{
		if (mapEntityIndex_15B4E0[i] > maxEnt)
			maxEnt = mapEntityIndex_15B4E0[i];
		if (mapEntityIndex_15B4E0[i] < minEnt)
			minEnt = mapEntityIndex_15B4E0[i];
	}
	if (maxEnt)
		revMaxEnt = 0xC40000 / maxEnt;
	for (int i = 0; i < 256 * 256; i++)
	{
		weightedVar = revMaxEnt * mapEntityIndex_15B4E0[i] >> 16;
		mapEntityIndex_15B4E0[i] = 0;
		if ((weightedVar & 0x8000u) != 0)//water level trunc
			weightedVar = 0;
		if (weightedVar > 196)//trunc max height
			weightedVar = 196;
		mapHeightmap_11B4E0[i] = weightedVar;
	}
}

int sub_B5C60_getTerrainAlt2(uint16_t axisX, uint16_t axisY)//296c60
{
	int point1;
	int diaPoint;
	int result;
	int point2;
	
	uaxis_2d locX;
	uaxis_2d locY;
	uaxis_2d locV2;

	locX.word = axisX;
	locY.word = axisY;

	locV2._axis_2d.x = locX._axis_2d.y;
	locV2._axis_2d.y = locY._axis_2d.y;

	if ((locV2._axis_2d.x + locV2._axis_2d.y) & 1)
	{
		if (locY._axis_2d.x > (locY._axis_2d.x + locX._axis_2d.x) % 256)
		{
			locV2._axis_2d.y++;
			point1 = mapHeightmap_11B4E0[locV2.word];
			locV2._axis_2d.x++;
			point2 = mapHeightmap_11B4E0[locV2.word];
			locV2._axis_2d.y--;
			diaPoint = (255 - locY._axis_2d.x) * (mapHeightmap_11B4E0[locV2.word] - point2) + locX._axis_2d.x * (point2 - point1);
		}
		else
		{
			point1 = mapHeightmap_11B4E0[locV2.word];
			locV2._axis_2d.x++;
			point2 = locX._axis_2d.x * (mapHeightmap_11B4E0[locV2.word] - point1);
			locV2._axis_2d.x--;
			locV2._axis_2d.y++;
			diaPoint = locY._axis_2d.x * (mapHeightmap_11B4E0[locV2.word] - point1) + point2;
		}
		result = (diaPoint >> 3) + 32 * point1;
	}
	else
	{
		if (locX._axis_2d.x <= locY._axis_2d.x)
		{
			point1 = mapHeightmap_11B4E0[locV2.word];
			locV2._axis_2d.y++;
			point2 = mapHeightmap_11B4E0[locV2.word];
			locV2._axis_2d.x++;
			diaPoint = locY._axis_2d.x * (point2 - point1) + locX._axis_2d.x * (mapHeightmap_11B4E0[locV2.word] - point2);
		}
		else
		{
			point1 = mapHeightmap_11B4E0[locV2.word];
			locV2._axis_2d.x++;
			point2 = mapHeightmap_11B4E0[locV2.word];
			locV2._axis_2d.y++;
			diaPoint = locY._axis_2d.x * (mapHeightmap_11B4E0[locV2.word] - point2) + locX._axis_2d.x * (point2 - point1);
		}
		result = (diaPoint >> 3) + 32 * point1;
	}
	return result;
}

//----- (00044E40) --------------------------------------------------------
void sub_44E40(int count, uint8_t minSmooth)//225e40
{
	uaxis_2d index;
	int locCount = count;
	for (int i = 0; i < 256 * 256; i++)
	{
		if (mapHeightmap_11B4E0[i])
			mapAngle_13B4E0[i] = 5;
		else
			mapAngle_13B4E0[i] = 0;
	}
	int i = 0;
	while ((locCount > 0) && (i < 1000))
	{
		for (i = 0; i < 1000; i++)
		{
			x_WORD_17B4E0 = 9377 * x_WORD_17B4E0 + 9439;
			index.word = x_WORD_17B4E0 % 0xffffu;
			if ((mapHeightmap_11B4E0[index.word] > minSmooth) && mapAngle_13B4E0[index.word])
			{
				sub_44EE0_smooth_tiles(index);
				locCount--;
				break;
			}
		}
	}
	for (int i = 0; i < 256 * 256; i++)
	{
		mapTerrainType_10B4E0[i] = 255;
	}
}

//----- (00045AA0) --------------------------------------------------------
void sub_45AA0_setMax4Tiles()//226aa0
{
	//  X-X
	//  | |
	//  B-X

	uaxis_2d indexx;
	char angleIndex;
	uint8_t minHeight;
	uint8_t maxHeight;
	bool runAgain;

	do
	{
		runAgain = false;
		for (int i = 0; i < 256 * 256; i++)
		{
			indexx.word = i;
			angleIndex = 0;
			if (!mapAngle_13B4E0[indexx.word])
				angleIndex = 1;
			minHeight = mapHeightmap_11B4E0[indexx.word];
			maxHeight = minHeight;
			indexx._axis_2d.x++;
			if (!mapAngle_13B4E0[indexx.word])
				angleIndex++;
			if (minHeight > mapHeightmap_11B4E0[indexx.word])
				minHeight = mapHeightmap_11B4E0[indexx.word];
			if (maxHeight < mapHeightmap_11B4E0[indexx.word])
				maxHeight = mapHeightmap_11B4E0[indexx.word];
			indexx._axis_2d.y++;
			if (!mapAngle_13B4E0[indexx.word])
				angleIndex++;
			if (minHeight > mapHeightmap_11B4E0[indexx.word])
				minHeight = mapHeightmap_11B4E0[indexx.word];
			if (maxHeight < mapHeightmap_11B4E0[indexx.word])
				maxHeight = mapHeightmap_11B4E0[indexx.word];
			indexx._axis_2d.x--;
			if (!mapAngle_13B4E0[indexx.word])
				angleIndex++;
			if (minHeight > mapHeightmap_11B4E0[indexx.word])
				minHeight = mapHeightmap_11B4E0[indexx.word];
			if (maxHeight < mapHeightmap_11B4E0[indexx.word])
				maxHeight = mapHeightmap_11B4E0[indexx.word];
			indexx._axis_2d.y--;
			if (maxHeight != minHeight && angleIndex == 4)
			{
				runAgain = true;
				mapHeightmap_11B4E0[indexx.word] = minHeight;
				indexx._axis_2d.x++;
				mapHeightmap_11B4E0[indexx.word] = minHeight;
				indexx._axis_2d.y++;
				mapHeightmap_11B4E0[indexx.word] = minHeight;
				indexx._axis_2d.x--;
				mapHeightmap_11B4E0[indexx.word] = minHeight;
				indexx._axis_2d.y--;
			}
		}
	} while (runAgain);
}

//----- (000440D0) --------------------------------------------------------
void sub_440D0(unsigned __int16 a1)//2250d0
{
	//    X
	//   / \
	//  X B X
	//   \|/
	//    X

	uint8_t maxHeight;
	uint8_t minHeight;
	int diffHeight;
	char ang3;
	char ang2;
	char ang5;
	uaxis_2d index;

	for (int i = 0; i < 256 * 256; i++)
	{
		index.word = i;
		if (mapAngle_13B4E0[index.word] == 5)
		{
			maxHeight = 0;
			minHeight = 255;
			if (mapHeightmap_11B4E0[index.word])
				maxHeight = mapHeightmap_11B4E0[index.word];
			if (mapHeightmap_11B4E0[index.word] < 255)
				minHeight = mapHeightmap_11B4E0[index.word];
			index._axis_2d.y--;
			if (maxHeight < mapHeightmap_11B4E0[index.word])
				maxHeight = mapHeightmap_11B4E0[index.word];
			if (minHeight > mapHeightmap_11B4E0[index.word])
				minHeight = mapHeightmap_11B4E0[index.word];
			index._axis_2d.x++;
			index._axis_2d.y++;
			if (maxHeight < mapHeightmap_11B4E0[index.word])
				maxHeight = mapHeightmap_11B4E0[index.word];
			if (minHeight > mapHeightmap_11B4E0[index.word])
				minHeight = mapHeightmap_11B4E0[index.word];
			index._axis_2d.x--;
			index._axis_2d.y++;
			if (maxHeight < mapHeightmap_11B4E0[index.word])
				maxHeight = mapHeightmap_11B4E0[index.word];
			if (minHeight > mapHeightmap_11B4E0[index.word])
				minHeight = mapHeightmap_11B4E0[index.word];
			index._axis_2d.x--;
			index._axis_2d.y--;
			if (maxHeight < mapHeightmap_11B4E0[index.word])
				maxHeight = mapHeightmap_11B4E0[index.word];
			if (minHeight > mapHeightmap_11B4E0[index.word])
				minHeight = mapHeightmap_11B4E0[index.word];
			diffHeight = maxHeight - minHeight;
			index._axis_2d.x++;
			if (diffHeight <= a1)
			{
				if (diffHeight == a1)
					mapAngle_13B4E0[index.word] = 4;
				else
					mapAngle_13B4E0[index.word] = 3;
			}
		}
	}

	//  X-X
	//  | |
	//  B-X

	for (int i = 0; i < 256 * 256; i++)	
	{
		index.word = i;
		ang3 = 0;
		ang2 = 0;
		ang5 = 0;
		if (mapAngle_13B4E0[index.word] == 3)
			ang3 = 1;
		if (mapAngle_13B4E0[index.word] == 2)
			ang2 = 1;
		if (mapAngle_13B4E0[index.word] == 5)
			ang5 = 1;
		index._axis_2d.x++;
		if (mapAngle_13B4E0[index.word] == 3)
			ang3++;
		if (mapAngle_13B4E0[index.word] == 2)
			ang2++;
		if (mapAngle_13B4E0[index.word] == 5)
			ang5++;
		index._axis_2d.y++;
		if (mapAngle_13B4E0[index.word] == 3)
			ang3++;
		if (mapAngle_13B4E0[index.word] == 2)
			ang2++;
		if (mapAngle_13B4E0[index.word] == 5)
			ang5++;
		index._axis_2d.x--;
		if (mapAngle_13B4E0[index.word] == 3)
			ang3++;
		if (mapAngle_13B4E0[index.word] == 2)
			ang2++;
		if (mapAngle_13B4E0[index.word] == 5)
			ang5++;
		index._axis_2d.y--;
		if (!ang2 && ang3 && ang5)
		{
			if (mapAngle_13B4E0[index.word] == 3)
				mapAngle_13B4E0[index.word] = 4;
			index._axis_2d.x++;
			if (mapAngle_13B4E0[index.word] == 3)
				mapAngle_13B4E0[index.word] = 4;
			index._axis_2d.y++;
			if (mapAngle_13B4E0[index.word] == 3)
				mapAngle_13B4E0[index.word] = 4;
			index._axis_2d.x--;
			if (mapAngle_13B4E0[index.word] == 3)
				mapAngle_13B4E0[index.word] = 4;
			index._axis_2d.y--;			
		}
	}
}

//----- (00045060) --------------------------------------------------------
void sub_45060(uint8_t maxHeightCut, uint8_t maxHeightDiffCut)//226060
{
	//  X-X-X
	//  |   |
	//  X B X
	//  |/| |
	//  X X-X
	
	uint8_t maxHeight;
	uint8_t minHeight;
	uaxis_2d index;
	qmemcpy(mapTerrainType_10B4E0, mapAngle_13B4E0, 0x10000);
	for (int i = 0; i < 256 * 256; i++)
	{
		index.word = i;
		maxHeight = 0;
		minHeight = 255;
		if (mapHeightmap_11B4E0[index.word])
			maxHeight = mapHeightmap_11B4E0[index.word];
		if (mapHeightmap_11B4E0[index.word] < 255)
			minHeight = mapHeightmap_11B4E0[index.word];
		index._axis_2d.y--;
		if (maxHeight < mapHeightmap_11B4E0[index.word])
			maxHeight = mapHeightmap_11B4E0[index.word];
		if (minHeight > mapHeightmap_11B4E0[index.word])
			minHeight = mapHeightmap_11B4E0[index.word];
		index._axis_2d.x++;
		if (maxHeight < mapHeightmap_11B4E0[index.word])
			maxHeight = mapHeightmap_11B4E0[index.word];
		if (minHeight > mapHeightmap_11B4E0[index.word])
			minHeight = mapHeightmap_11B4E0[index.word];
		index._axis_2d.y++;
		if (maxHeight < mapHeightmap_11B4E0[index.word])
			maxHeight = mapHeightmap_11B4E0[index.word];
		if (minHeight > mapHeightmap_11B4E0[index.word])
			minHeight = mapHeightmap_11B4E0[index.word];
		index._axis_2d.y++;
		if (maxHeight < mapHeightmap_11B4E0[index.word])
			maxHeight = mapHeightmap_11B4E0[index.word];
		if (minHeight > mapHeightmap_11B4E0[index.word])
			minHeight = mapHeightmap_11B4E0[index.word];
		index._axis_2d.x--;
		if (maxHeight < mapHeightmap_11B4E0[index.word])
			maxHeight = mapHeightmap_11B4E0[index.word];
		if (minHeight > mapHeightmap_11B4E0[index.word])
			minHeight = mapHeightmap_11B4E0[index.word];
		index._axis_2d.x--;
		if (maxHeight < mapHeightmap_11B4E0[index.word])
			maxHeight = mapHeightmap_11B4E0[index.word];
		if (minHeight > mapHeightmap_11B4E0[index.word])
			minHeight = mapHeightmap_11B4E0[index.word];
		index._axis_2d.y--;
		if (maxHeight < mapHeightmap_11B4E0[index.word])
			maxHeight = mapHeightmap_11B4E0[index.word];
		if (minHeight > mapHeightmap_11B4E0[index.word])
			minHeight = mapHeightmap_11B4E0[index.word];
		index._axis_2d.y--;
		if (maxHeight < mapHeightmap_11B4E0[index.word])
			maxHeight = mapHeightmap_11B4E0[index.word];
		if (minHeight > mapHeightmap_11B4E0[index.word])
			minHeight = mapHeightmap_11B4E0[index.word];
		index._axis_2d.x++;
		index._axis_2d.y++;
		if (maxHeight < maxHeightCut && maxHeight - minHeight <= maxHeightDiffCut)
		{
			if (mapAngle_13B4E0[index.word])
				mapAngle_13B4E0[index.word] = 5;
		}
	}
}

//----- (00044320) --------------------------------------------------------
void sub_44320()//225320
{
	//  X-X
	//  | |
	//  B-X

	char ang0;
	char ang3;
	char ang5;

	uaxis_2d index;
	for (int i = 0; i < 256 * 256; i++)
	{
		index.word = i;
		ang0 = 0;
		ang3 = 0;
		ang5 = 0;
		if (!mapAngle_13B4E0[index.word])
			ang0 = 1;
		if (mapAngle_13B4E0[index.word] == 5)
			ang5 = 1;
		if (mapAngle_13B4E0[index.word] == 3)
			ang3 = 1;
		index._axis_2d.x++;
		if (!mapAngle_13B4E0[index.word])
			ang0++;
		if (mapAngle_13B4E0[index.word] == 5)
			ang5++;
		if (mapAngle_13B4E0[index.word] == 3)
			ang3++;
		index._axis_2d.y++;
		if (!mapAngle_13B4E0[index.word])
			ang0++;
		if (mapAngle_13B4E0[index.word] == 5)
			ang5++;
		if (mapAngle_13B4E0[index.word] == 3)
			ang3++;
		index._axis_2d.x--;
		if (!mapAngle_13B4E0[index.word])
			ang0++;
		if (mapAngle_13B4E0[index.word] == 5)
			ang5++;
		if (mapAngle_13B4E0[index.word] == 3)
			ang3++;
		index._axis_2d.y--;
		if (ang3 && ang5)
		{
			if (mapAngle_13B4E0[index.word] == 5)
				mapAngle_13B4E0[index.word] = 4;
			index._axis_2d.x++;
			if (mapAngle_13B4E0[index.word] == 5)
				mapAngle_13B4E0[index.word] = 4;
			index._axis_2d.y++;
			if (mapAngle_13B4E0[index.word] == 5)
				mapAngle_13B4E0[index.word] = 4;
			index._axis_2d.x--;
			if (mapAngle_13B4E0[index.word] == 5)
				mapAngle_13B4E0[index.word] = 4;
			index._axis_2d.y--;
		}
		if (ang3 && ang0)
		{
			if (mapAngle_13B4E0[index.word] == 3)
				mapAngle_13B4E0[index.word] = 4;
			index._axis_2d.x++;
			if (mapAngle_13B4E0[index.word] == 3)
				mapAngle_13B4E0[index.word] = 4;
			index._axis_2d.y++;
			if (mapAngle_13B4E0[index.word] == 3)
				mapAngle_13B4E0[index.word] = 4;
			index._axis_2d.x--;
			if (mapAngle_13B4E0[index.word] == 3)
				mapAngle_13B4E0[index.word] = 4;
			index._axis_2d.y--;
		}
		if (ang0 && ang5)
		{
			if (mapAngle_13B4E0[index.word])
				mapAngle_13B4E0[index.word] = 4;
			index._axis_2d.x++;
			if (mapAngle_13B4E0[index.word])
				mapAngle_13B4E0[index.word] = 4;
			index._axis_2d.y++;
			if (mapAngle_13B4E0[index.word])
				mapAngle_13B4E0[index.word] = 4;
			index._axis_2d.x--;
			if (mapAngle_13B4E0[index.word])
				mapAngle_13B4E0[index.word] = 4;
			index._axis_2d.y--;
		}
	}
}

//----- (00045210) --------------------------------------------------------
void sub_45210(uint8_t maxHeightCut, uint8_t maxHeightDiffCut)//226210
{
	//  X-X-X
	//  |   |
	//  X B X
	//  |/| |
	//  X X-X

	uint8_t ang2;
	uint8_t ang5;
	uint8_t maxHeight;
	uint8_t minHeight;

	qmemcpy(mapTerrainType_10B4E0, mapAngle_13B4E0, 0x10000);
	uaxis_2d index;
	for (int i = 0; i < 256 * 256; i++)
	{
		index.word = i;
		minHeight = 255;
		maxHeight = 0;
		ang2 = 0;
		ang5 = 0;
		if (mapHeightmap_11B4E0[index.word] > 0)
			maxHeight = mapHeightmap_11B4E0[index.word];
		if (mapHeightmap_11B4E0[index.word] < 255)
			minHeight = mapHeightmap_11B4E0[index.word];
		index._axis_2d.y--;
		if (maxHeight < mapHeightmap_11B4E0[index.word])
			maxHeight = mapHeightmap_11B4E0[index.word];
		if (minHeight > mapHeightmap_11B4E0[index.word])
			minHeight = mapHeightmap_11B4E0[index.word];
		if (mapAngle_13B4E0[index.word] == 5)
			ang5 = 1;
		if (mapAngle_13B4E0[index.word] == 2)
			ang2 = 1;
		index._axis_2d.x++;
		if (maxHeight < mapHeightmap_11B4E0[index.word])
			maxHeight = mapHeightmap_11B4E0[index.word];
		if (minHeight > mapHeightmap_11B4E0[index.word])
			minHeight = mapHeightmap_11B4E0[index.word];
		if (mapAngle_13B4E0[index.word] == 5)
			ang5++;
		if (mapAngle_13B4E0[index.word] == 2)
			ang2++;
		index._axis_2d.y++;
		if (maxHeight < mapHeightmap_11B4E0[index.word])
			maxHeight = mapHeightmap_11B4E0[index.word];
		if (minHeight > mapHeightmap_11B4E0[index.word])
			minHeight = mapHeightmap_11B4E0[index.word];
		if (mapAngle_13B4E0[index.word] == 5)
			ang5++;
		if (mapAngle_13B4E0[index.word] == 2)
			ang2++;
		index._axis_2d.y++;
		if (maxHeight < mapHeightmap_11B4E0[index.word])
			maxHeight = mapHeightmap_11B4E0[index.word];
		if (minHeight > mapHeightmap_11B4E0[index.word])
			minHeight = mapHeightmap_11B4E0[index.word];
		if (mapAngle_13B4E0[index.word] == 5)
			ang5++;
		if (mapAngle_13B4E0[index.word] == 2)
			ang2++;
		index._axis_2d.x--;
		if (maxHeight < mapHeightmap_11B4E0[index.word])
			maxHeight = mapHeightmap_11B4E0[index.word];
		if (minHeight > mapHeightmap_11B4E0[index.word])
			minHeight = mapHeightmap_11B4E0[index.word];
		if (mapAngle_13B4E0[index.word] == 5)
			ang5++;
		if (mapAngle_13B4E0[index.word] == 2)
			ang2++;
		index._axis_2d.x--;
		if (maxHeight < mapHeightmap_11B4E0[index.word])
			maxHeight = mapHeightmap_11B4E0[index.word];
		if (minHeight > mapHeightmap_11B4E0[index.word])
			minHeight = mapHeightmap_11B4E0[index.word];
		if (mapAngle_13B4E0[index.word] == 5)
			ang5++;
		if (mapAngle_13B4E0[index.word] == 2)
			ang2++;
		index._axis_2d.y--;
		if (maxHeight < mapHeightmap_11B4E0[index.word])
			maxHeight = mapHeightmap_11B4E0[index.word];
		if (minHeight > mapHeightmap_11B4E0[index.word])
			minHeight = mapHeightmap_11B4E0[index.word];
		if (mapAngle_13B4E0[index.word] == 5)
			ang5++;
		if (mapAngle_13B4E0[index.word] == 2)
			ang2++;
		index._axis_2d.y--;
		if (maxHeight < mapHeightmap_11B4E0[index.word])
			maxHeight = mapHeightmap_11B4E0[index.word];
		if (minHeight > mapHeightmap_11B4E0[index.word])
			minHeight = mapHeightmap_11B4E0[index.word];
		if (mapAngle_13B4E0[index.word] == 5)
			ang5++;
		if (mapAngle_13B4E0[index.word] == 2)
			ang2++;
		index._axis_2d.x++;
		index._axis_2d.y++;
		if (maxHeight < maxHeightCut)
		{
			if (maxHeight - minHeight <= maxHeightDiffCut && mapAngle_13B4E0[index.word] == 5)
			{
				if (ang5 + ang2 == 8)
					mapAngle_13B4E0[index.word] = 2;
			}
		}
	}
}

//----- (000454F0) --------------------------------------------------------
void sub_454F0(uint8_t maxHeightCut, uint8_t maxHeightDiffCut)//2264f0
{
	//    X
	//	 / \
	//  X-B X
	//    |/
	//    X

	uint8_t maxHeight;
	uint8_t minHeight;
	uaxis_2d index;
	for (int i = 0; i < 256 * 256; i++)
	{
		index.word = i;
		if (mapHeightmap_11B4E0[index.word] > maxHeightCut)
		{
			maxHeight = 0;
			minHeight = 255;
			if (mapHeightmap_11B4E0[index.word])
				maxHeight = mapHeightmap_11B4E0[index.word];
			if (mapHeightmap_11B4E0[index.word] < 255)
				minHeight = mapHeightmap_11B4E0[index.word];
			index._axis_2d.y--;
			if (maxHeight < mapHeightmap_11B4E0[index.word])
				maxHeight = mapHeightmap_11B4E0[index.word];
			if (minHeight > mapHeightmap_11B4E0[index.word])
				minHeight = mapHeightmap_11B4E0[index.word];
			index._axis_2d.x++;
			index._axis_2d.y++;
			if (maxHeight < mapHeightmap_11B4E0[index.word])
				maxHeight = mapHeightmap_11B4E0[index.word];
			if (minHeight > mapHeightmap_11B4E0[index.word])
				minHeight = mapHeightmap_11B4E0[index.word];
			index._axis_2d.x--;
			index._axis_2d.y++;
			if (maxHeight < mapHeightmap_11B4E0[index.word])
				maxHeight = mapHeightmap_11B4E0[index.word];
			if (minHeight > mapHeightmap_11B4E0[index.word])
				minHeight = mapHeightmap_11B4E0[index.word];
			index._axis_2d.x--;
			index._axis_2d.y--;
			if (maxHeight < mapHeightmap_11B4E0[index.word])
				maxHeight = mapHeightmap_11B4E0[index.word];
			if (minHeight > mapHeightmap_11B4E0[index.word])
				minHeight = mapHeightmap_11B4E0[index.word];
			index._axis_2d.x++;
			if (mapAngle_13B4E0[index.word])
			{
				if (maxHeight - minHeight < maxHeightDiffCut)
					mapAngle_13B4E0[index.word] = 6;
			}
		}
	}
}

//----- (00045600) --------------------------------------------------------
void sub_45600(uint8_t a1)//226600
{
	char ang4;
	char ang2;
	char ang3;
	char ang5;
	uint8_t maxHeight;
	uint8_t minHeight;

	qmemcpy(mapTerrainType_10B4E0, mapAngle_13B4E0, 0x10000);
	uaxis_2d index;

	//    X
	//	 / \
	//  X-B X
	//    |/
	//    X

	for (int i = 0; i < 256 * 256; i++)
	{
		index.word = i;
		maxHeight = 0;
		minHeight = 255;
		if (mapHeightmap_11B4E0[index.word])
			maxHeight = mapHeightmap_11B4E0[index.word];
		if (mapHeightmap_11B4E0[index.word] < 255)
			minHeight = mapHeightmap_11B4E0[index.word];
		index._axis_2d.y--;
		if (maxHeight < mapHeightmap_11B4E0[index.word])
			maxHeight = mapHeightmap_11B4E0[index.word];
		if (minHeight > mapHeightmap_11B4E0[index.word])
			minHeight = mapHeightmap_11B4E0[index.word];
		index._axis_2d.x++;
		index._axis_2d.y++;
		if (maxHeight < mapHeightmap_11B4E0[index.word])
			maxHeight = mapHeightmap_11B4E0[index.word];
		if (minHeight > mapHeightmap_11B4E0[index.word])
			minHeight = mapHeightmap_11B4E0[index.word];
		index._axis_2d.x--;
		index._axis_2d.y++;
		if (maxHeight < mapHeightmap_11B4E0[index.word])
			maxHeight = mapHeightmap_11B4E0[index.word];
		if (minHeight > mapHeightmap_11B4E0[index.word])
			minHeight = mapHeightmap_11B4E0[index.word];
		index._axis_2d.x--;
		index._axis_2d.y--;
		if (maxHeight < mapHeightmap_11B4E0[index.word])
			maxHeight = mapHeightmap_11B4E0[index.word];
		if (minHeight > mapHeightmap_11B4E0[index.word])
			minHeight = mapHeightmap_11B4E0[index.word];
		index._axis_2d.x++;
		if (mapAngle_13B4E0[index.word] && maxHeight - minHeight >= a1)
			mapAngle_13B4E0[index.word] = 1;
	}

	//  X-X-X
	//  |   |
	//  X B X
	//  |/| |
	//  X X-X

	for (int i = 0; i < 256 * 256; i++)
	{
		index.word = i;
		if (mapAngle_13B4E0[index.word] == 6)
		{
			ang4 = 0;
			ang2 = 0;
			ang3 = 0;
			ang5 = 0;
			index._axis_2d.y--;
			if (mapAngle_13B4E0[index.word] == 3)
				ang3 = 1;
			if (mapAngle_13B4E0[index.word] == 2)
				ang2 = 1;
			if (mapAngle_13B4E0[index.word] == 5)
				ang5 = 1;
			if (mapAngle_13B4E0[index.word] == 4)
				ang4 = 1;
			index._axis_2d.x++;
			if (mapAngle_13B4E0[index.word] == 3)
				ang3++;
			if (mapAngle_13B4E0[index.word] == 2)
				ang2++;
			if (mapAngle_13B4E0[index.word] == 5)
				ang5++;
			if (mapAngle_13B4E0[index.word] == 4)
				ang4++;
			index._axis_2d.y++;
			if (mapAngle_13B4E0[index.word] == 3)
				ang3++;
			if (mapAngle_13B4E0[index.word] == 2)
				ang2++;
			if (mapAngle_13B4E0[index.word] == 5)
				ang5++;
			if (mapAngle_13B4E0[index.word] == 4)
				ang4++;
			index._axis_2d.y++;
			if (mapAngle_13B4E0[index.word] == 3)
				ang3++;
			if (mapAngle_13B4E0[index.word] == 2)
				ang2++;
			if (mapAngle_13B4E0[index.word] == 5)
				ang5++;
			if (mapAngle_13B4E0[index.word] == 4)
				ang4++;
			index._axis_2d.x--;
			if (mapAngle_13B4E0[index.word] == 3)
				ang3++;
			if (mapAngle_13B4E0[index.word] == 2)
				ang2++;
			if (mapAngle_13B4E0[index.word] == 5)
				ang5++;
			if (mapAngle_13B4E0[index.word] == 4)
				ang4++;
			index._axis_2d.x--;
			if (mapAngle_13B4E0[index.word] == 3)
				ang3++;
			if (mapAngle_13B4E0[index.word] == 2)
				ang2++;
			if (mapAngle_13B4E0[index.word] == 5)
				ang5++;
			if (mapAngle_13B4E0[index.word] == 4)
				ang4++;
			index._axis_2d.y--;
			if (mapAngle_13B4E0[index.word] == 3)
				ang3++;
			if (mapAngle_13B4E0[index.word] == 2)
				ang2++;
			if (mapAngle_13B4E0[index.word] == 5)
				ang5++;
			if (mapAngle_13B4E0[index.word] == 4)
				ang4++;
			index._axis_2d.y--;
			if (mapAngle_13B4E0[index.word] == 3)
				ang3++;
			if (mapAngle_13B4E0[index.word] == 2)
				ang2++;
			if (mapAngle_13B4E0[index.word] == 5)
				ang5++;
			if (mapAngle_13B4E0[index.word] == 4)
				ang4++;
			index._axis_2d.x++;
			index._axis_2d.y++;
			if (ang3)
			{
				if (ang2 || ang5 || ang4)
					mapAngle_13B4E0[index.word] = 1;
			}
			else if (ang2 || (ang5 && ang4))
			{
				mapAngle_13B4E0[index.word] = 1;
			}
		}
	}
}

//----- (00043FC0) --------------------------------------------------------
void sub_43FC0()//224fc0
{
	int sameAngle;
	char centerAngle;

	//  X-X-X
	//  |   |
	//  X B X
	//  |/| |
	//  X X-X

	uaxis_2d index;
	for (int i = 0; i < 256 * 256; i++)
	{
		index.word = i;
		index._axis_2d.y--;
		centerAngle = mapAngle_13B4E0[index.word];
		index._axis_2d.x++;
		sameAngle = (centerAngle == mapAngle_13B4E0[index.word]);
		index._axis_2d.y++;
		sameAngle += (centerAngle == mapAngle_13B4E0[index.word]);
		index._axis_2d.y++;
		sameAngle += (centerAngle == mapAngle_13B4E0[index.word]);
		index._axis_2d.x--;
		sameAngle += (centerAngle == mapAngle_13B4E0[index.word]);
		index._axis_2d.x--;
		sameAngle += (centerAngle == mapAngle_13B4E0[index.word]);
		index._axis_2d.y--;
		sameAngle += (centerAngle == mapAngle_13B4E0[index.word]);
		index._axis_2d.y--;
		sameAngle += (centerAngle == mapAngle_13B4E0[index.word]);
		index._axis_2d.x++;
		index._axis_2d.y++;
		if (centerAngle)
		{
			if (sameAngle == 7)
				mapAngle_13B4E0[index.word] = centerAngle;
		}
	}
}

//----- (00043970) --------------------------------------------------------
void sub_43970()//224970
{
	uaxis_2d index;
	for (int i = 0; i < 256 * 256; i++)
	{
		index.word = i;
		mapHeightmap_11B4E0[index.word] = sub_439A0(index.word);
	}
}

//----- (00043EE0) --------------------------------------------------------
void sub_43EE0()//224ee0
{
	//  X-X
	//  | |
	//  B-X

	char ang0;
	char ang4;
	uint8_t heightM1;
	uint8_t heightM2;
	char angleM1;
	char angleM2;
	char angleM3;
	uaxis_2d index;
	for (int i = 0; i < 256 * 256; i++)
	{
		ang4 = 0;
		ang0 = 0;
		index.word = i;
		heightM1 = mapHeightmap_11B4E0[index.word];
		index._axis_2d.x++;
		angleM1 = mapAngle_13B4E0[index.word];
		if (angleM1)
		{
			if (angleM1 == 4)
				ang4 = 1;
		}
		else
		{
			heightM2 = mapHeightmap_11B4E0[index.word];
			ang0 = 1;
			if (heightM2 < heightM1)
				heightM1 = heightM2;
		}
		index._axis_2d.y++;
		angleM2 = mapAngle_13B4E0[index.word];
		if (angleM2)
		{
			if (angleM2 == 4)
				ang4++;
		}
		else
		{
			ang0++;
			if (mapHeightmap_11B4E0[index.word] < heightM1)
				heightM1 = mapHeightmap_11B4E0[index.word];
		}
		index._axis_2d.x--;
		angleM3 = mapAngle_13B4E0[index.word];
		if (angleM3)
		{
			if (angleM3 == 4)
				ang4++;
		}
		else
		{
			ang0++;
			if (mapHeightmap_11B4E0[index.word] < heightM1)
				heightM1 = mapHeightmap_11B4E0[index.word];
		}
		index._axis_2d.y--;
		if (ang4 && ang0 && !heightM1)
		{
			mapHeightmap_11B4E0[index.word] = 0;
			index._axis_2d.x++;
			mapHeightmap_11B4E0[index.word] = 0;
			index._axis_2d.y++;
			mapHeightmap_11B4E0[index.word] = 0;
			index._axis_2d.x--;
			mapHeightmap_11B4E0[index.word] = 0;
			index._axis_2d.y--;
		}
	};
}

//----- (00044580) --------------------------------------------------------
void sub_44580()//225580
{
	uint8_t* actBufPos;
	char point1;
	char point2;
	char point3;
	char point4;
	uint8_t actBufEnt;

	for (int i = 0; i < 2401 * 25; i += 25)
		pdwScreenBuffer_351628[i] = 0;
	for (int i = 0; i < 148; i++)
	{
		if (unk_D47E0[4 * i + 0] >= 0)
		{
			if (unk_D47E0[4 * i + 1] >= 0)
			{
				if (unk_D47E0[4 * i + 2] >= 0)
				{
					if (unk_D47E0[4 * i + 3] >= 0)
					{
						actBufPos = &pdwScreenBuffer_351628[25 * (49 * unk_D47E0[4 * i + 1] + 7 * unk_D47E0[4 * i + 2] + unk_D47E0[4 * i + 3] + 343 * unk_D47E0[4 * i + 0])];
						if (*actBufPos < 12)
						{
							actBufPos[*actBufPos + 13] = 0;
							actBufPos[*actBufPos + 1] = i;
							(*actBufPos)++;
						}
						actBufPos = &pdwScreenBuffer_351628[25 * (49 * unk_D47E0[4 * i + 0] + unk_D47E0[4 * i + 2] + 7 * unk_D47E0[4 * i + 3] + 343 * unk_D47E0[4 * i + 1])];
						if (*actBufPos < 12)
						{							
							actBufPos[*actBufPos + 13] = 16;
							actBufPos[*actBufPos + 1] = i;
							(*actBufPos)++;
						}
						actBufPos = &pdwScreenBuffer_351628[25 * (49 * unk_D47E0[4 * i + 3] + unk_D47E0[4 * i + 1] + 7 * unk_D47E0[4 * i + 0] + 343 * unk_D47E0[4 * i + 2])];
						if (*actBufPos < 12)
						{
							actBufPos[*actBufPos + 13] = 48;
							actBufPos[*actBufPos + 1] = i;
							(*actBufPos)++;
						}
						actBufPos = &pdwScreenBuffer_351628[25 * (49 * unk_D47E0[4 * i + 2] + unk_D47E0[4 * i + 0] + 7 * unk_D47E0[4 * i + 1] + 343 * unk_D47E0[4 * i + 3])];
						if (*actBufPos < 12)
						{
							actBufPos[*actBufPos + 13] = 32;
							actBufPos[*actBufPos + 1] = i;
							(*actBufPos)++;
						}
						actBufPos = &pdwScreenBuffer_351628[25 * (49 * unk_D47E0[4 * i + 2] + 8 * unk_D47E0[4 * i + 3] - unk_D47E0[4 * i + 3] + unk_D47E0[4 * i + 0] + 343 * unk_D47E0[4 * i + 1])];
						if (*actBufPos < 12)
						{
							actBufPos[*actBufPos + 13] = 96;
							actBufPos[*actBufPos + 1] = i;
							(*actBufPos)++;
						}
						actBufPos = &pdwScreenBuffer_351628[25 * (49 * unk_D47E0[4 * i + 1] + 8 * unk_D47E0[4 * i + 0] - unk_D47E0[4 * i + 0] + unk_D47E0[4 * i + 3] + 343 * unk_D47E0[4 * i + 2])];
						if (*actBufPos < 12)
						{
							actBufPos[*actBufPos + 13] = 112;
							actBufPos[*actBufPos + 1] = i;
							(*actBufPos)++;
						}
						actBufPos = &pdwScreenBuffer_351628[25 * (49 * unk_D47E0[4 * i + 0] + 7 * unk_D47E0[4 * i + 1] + unk_D47E0[4 * i + 2] + 343 * unk_D47E0[4 * i + 3])];
						if (*actBufPos < 12)
						{
							actBufPos[*actBufPos + 13] = 80;
							actBufPos[*actBufPos + 1] = i;
							(*actBufPos)++;
						}
						actBufPos = &pdwScreenBuffer_351628[25 * (343 * unk_D47E0[4 * i + 0] + 7 * unk_D47E0[4 * i + 2] + unk_D47E0[4 * i + 1] + 49 * unk_D47E0[4 * i + 3])];
						if (*actBufPos < 12)
						{
							actBufPos[*actBufPos + 13] = 64;
							actBufPos[*actBufPos + 1] = i;
							(*actBufPos)++;
						}
					}
				}
			}
		}
	}
	int index = 0;
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			for (int k = 0; k < 7; k++)
			{
				for (int l = 0; l < 7; l++)
				{
					actBufPos = &pdwScreenBuffer_351628[25 * (49 * j + 7 * k + l + 343 * i)];
					if (*actBufPos)
					{
						x_BYTE_F2CD0x[index][0] = actBufPos[1];
						x_BYTE_F2CD0x[index][1] = actBufPos[13];
					}
					else
					{
						x_BYTE_F2CD0x[index][0] = 1;
						x_BYTE_F2CD0x[index][1] = 0;
					}
					index++;
				}
			}
		}
	}
	//adress 225bbd

	//  X-X
	//  | |
	//  B-X

	uaxis_2d uindex;
	for (int i = 0; i < 256 * 256; i++)
	{
		uindex.word = i;
		if (!mapTerrainType_10B4E0[uindex.word])
		{
			point1 = mapAngle_13B4E0[uindex.word] & 7;
			uindex._axis_2d.x++;
			point2 = mapAngle_13B4E0[uindex.word] & 7;
			uindex._axis_2d.y++;
			point3 = mapAngle_13B4E0[uindex.word] & 7;
			uindex._axis_2d.x--;
			point4 = mapAngle_13B4E0[uindex.word] & 7;
			uindex._axis_2d.y--;
			actBufEnt = pdwScreenBuffer_351628[25 * (343 * point1 + 49 * point2 + point4 + 7 * point3)];
			if (actBufEnt)
			{
				x_WORD_17B4E0 = 9377 * x_WORD_17B4E0 + 9439;
				if (x_WORD_17B4E0 % (actBufEnt + 1) >= actBufEnt)
					actBufPos = &pdwScreenBuffer_351628[25 * (343 * point1 + 49 * point2 + point4 + 7 * point3)];
				else
					actBufPos = &pdwScreenBuffer_351628[x_WORD_17B4E0 % (actBufEnt + 1) + 25 * (343 * point1 + 49 * point2 + point4 + 7 * point3)];
				mapTerrainType_10B4E0[uindex.word] = actBufPos[1];
				mapAngle_13B4E0[uindex.word] = (mapAngle_13B4E0[uindex.word] & 7) + actBufPos[13];
			}
			else
			{
				mapTerrainType_10B4E0[uindex.word] = 1;
			}
		}
	}
}

//----- (00043B40) --------------------------------------------------------
void sub_43B40()//224b40
{
	uint8_t locHeight;
	uaxis_2d index;
	for (int i = 0; i < 256 * 256; i++)
	{
		index.word = i;
		locHeight = mapHeightmap_11B4E0[index.word];
		if (locHeight > x_BYTE_D41B7)
			locHeight = x_BYTE_D41B7;
		x_BYTE_14B4E0_second_heightmap[index.word] = x_BYTE_D41B7 - locHeight;
		if (x_BYTE_D41B7 - locHeight > mapHeightmap_11B4E0[index.word])
		{
			mapAngle_13B4E0[index.word] &= 0xF7u;
		}
		else
		{
			x_BYTE_14B4E0_second_heightmap[index.word] = mapHeightmap_11B4E0[index.word] - 1;
			mapAngle_13B4E0[index.word] |= 8;
		}
	}
	sub_43BB0();
}

//----- (00043D50) --------------------------------------------------------
void sub_43D50()//224d50
{

	//  X-X-X
	//  |   |
	//  X B X
	//  |/| |
	//  X X-X

	uint8_t point1;
	uint8_t point2;
	uaxis_2d index;
	for (int i = 0; i < 256 * 256; i++)
	{
		index.word = i;
		mapAngle_13B4E0[index.word] &= 0xF7u;
		if (!mapHeightmap_11B4E0[index.word])
		{
			index._axis_2d.y--;
			point1 = (mapHeightmap_11B4E0[index.word] != 0);
			index._axis_2d.x++;
			point1 += (mapHeightmap_11B4E0[index.word] != 0);
			index._axis_2d.y++;
			point1 += (mapHeightmap_11B4E0[index.word] != 0);
			index._axis_2d.y++;
			point1 += (mapHeightmap_11B4E0[index.word] != 0);
			index._axis_2d.x--;
			point1 += (mapHeightmap_11B4E0[index.word] != 0);
			index._axis_2d.x--;
			point1 += (mapHeightmap_11B4E0[index.word] != 0);
			index._axis_2d.y--;
			point1 += (mapHeightmap_11B4E0[index.word] != 0);
			index._axis_2d.y--;
			point2 = mapHeightmap_11B4E0[index.word] != 0;
			index._axis_2d.x++;
			index._axis_2d.y++;

			//  X-X
			//  | |
			//  B-X

			if (!(point2 + point1))
			{
				point1 = (mapTerrainType_10B4E0[index.word] != 0);
				index._axis_2d.x--;
				point1 += (mapTerrainType_10B4E0[index.word] != 0);
				index._axis_2d.y--;
				point1 += (mapTerrainType_10B4E0[index.word] != 0);
				index._axis_2d.x++;
				point1 += (mapTerrainType_10B4E0[index.word] != 0);
				index._axis_2d.y++;
				if (!point1)
					mapAngle_13B4E0[index.word] |= 8u;
			}
		}
	}
}

//----- (00044D00) --------------------------------------------------------
void sub_44D00()//225d00
{

	//     X
	//    /
	//   B
	//  /
	// X

	uaxis_2d tempIndex;
	uaxis_2d index;
	x_WORD_17B4E0 = 0;
	for (int i = 0; i < 256 * 256; i++)
	{
		index.word = i;
		index._axis_2d.x++;
		index._axis_2d.y++;
		tempIndex.word = index.word;
		index._axis_2d.x -= 2;
		index._axis_2d.y -= 2;
		tempIndex._axis_2d.x = mapHeightmap_11B4E0[index.word] - mapHeightmap_11B4E0[tempIndex.word] + 32;
		index._axis_2d.x++;
		index._axis_2d.y++;
		if (tempIndex._axis_2d.x == 32)
		{
			tempIndex.word = 9377 * x_WORD_17B4E0 + 9439;
			x_WORD_17B4E0 = tempIndex.word;
			tempIndex._axis_2d.y = (x_WORD_17B4E0 / 9u) >> 8;
			tempIndex._axis_2d.x = x_WORD_17B4E0 % 9 + 28;
		}
		else if ((int8_t)tempIndex._axis_2d.x >= 28)
		{
			if ((int8_t)tempIndex._axis_2d.x > 40)
				tempIndex._axis_2d.x = (tempIndex._axis_2d.x & 7) + 40;
		}
		else
		{
			tempIndex._axis_2d.x = (tempIndex._axis_2d.x & 3) + 28;
		}
		if (D41A0_0.terrain_2FECE.MapType != MapType_t::Day)
		{
			mapShading_12B4E0[index.word] = (64 - tempIndex._axis_2d.x);
		}
		else
		{
			mapShading_12B4E0[index.word] = tempIndex._axis_2d.x;
		}
	}
}

//----- (000B5EFA) --------------------------------------------------------
void sub_B5EFA(__int16 a1, uaxis_2d* indexx, int32_t a3, int16_t* nextRand)//296EFA
{
	//  X-.-X
	//   \  |
	//    E .
	//      |
	//  B---X

	int16_t sumEnt;
	uint16_t srandNumber;

	sumEnt = mapEntityIndex_15B4E0[indexx->word];
	indexx->_axis_2d.x += 2 * a1;
	sumEnt += mapEntityIndex_15B4E0[indexx->word];
	indexx->_axis_2d.y += 2 * a1;
	sumEnt += mapEntityIndex_15B4E0[indexx->word];
	indexx->_axis_2d.x -= 2 * a1;
	sumEnt += mapEntityIndex_15B4E0[indexx->word];
	indexx->_axis_2d.x += a1;
	indexx->_axis_2d.y -= a1;
	srandNumber = 9377 * *nextRand + 9439;
	*nextRand = srandNumber;
	if (!mapEntityIndex_15B4E0[indexx->word])
		mapEntityIndex_15B4E0[indexx->word] = srandNumber % (uint16_t)(2 * a3 + 1)
		+ srandNumber % (uint16_t)((a1 << 6) + 1) + (sumEnt >> 2) - 32 * a1 - a3;
	indexx->_axis_2d.x += a1;
	indexx->_axis_2d.y -= a1;
}

//----- (000B5F8F) --------------------------------------------------------
void sub_B5F8F(__int16 a1, uaxis_2d* indexx, int32_t a3, int16_t* nextRand)//296f8f
{

	//   X
	//   |\
	// B E X
	//  \ /
	//   X

	int16_t sumEnt;
	int16_t sumEnt2;
	uint16_t srandNumber;

	sumEnt = mapEntityIndex_15B4E0[indexx->word];
	sumEnt2 = sumEnt;
	indexx->_axis_2d.x += a1;
	indexx->_axis_2d.y -= a1;
	sumEnt += mapEntityIndex_15B4E0[indexx->word];
	indexx->_axis_2d.x += a1;
	indexx->_axis_2d.y += a1;
	sumEnt += mapEntityIndex_15B4E0[indexx->word];
	indexx->_axis_2d.x -= a1;
	indexx->_axis_2d.y += a1;
	sumEnt += mapEntityIndex_15B4E0[indexx->word];
	srandNumber = 9377 * *nextRand + 9439;
	sumEnt2 += mapEntityIndex_15B4E0[indexx->word];
	indexx->_axis_2d.y -= a1;
	if (!mapEntityIndex_15B4E0[indexx->word])
		mapEntityIndex_15B4E0[indexx->word] = srandNumber % (uint16_t)(2 * a3 + 1)
		+ srandNumber % (uint16_t)((a1 << 6) + 1) + (uint16_t)(sumEnt >> 2) - 32 * a1 - a3;
	
	//   X
	//  /|
	// X E-.
	//  \   \
	//   .-B R

	indexx->_axis_2d.x -= 2 * a1;
	indexx->_axis_2d.y += a1;
	sumEnt2 += mapEntityIndex_15B4E0[indexx->word];
	indexx->_axis_2d.x += a1;
	indexx->_axis_2d.y += a1;
	sumEnt2 += mapEntityIndex_15B4E0[indexx->word];
	indexx->_axis_2d.y -= a1;
	srandNumber = 9377 * srandNumber + 9439;
	*nextRand = srandNumber;
	if (!mapEntityIndex_15B4E0[indexx->word])
		mapEntityIndex_15B4E0[indexx->word] = srandNumber % (uint16_t)(2 * a3 + 1)
		+ srandNumber % (uint16_t)((a1 << 6) + 1) + (uint16_t)(sumEnt2 >> 2) - 32 * a1 - a3;
	indexx->_axis_2d.x += 2 * a1;
	indexx->_axis_2d.y -= a1;
}

//----- (00044EE0) --------------------------------------------------------
void sub_44EE0_smooth_tiles(uaxis_2d axis)//225ee0
{
	//  X-X-X
	//  |   |
	//  X B X
	//  | | |
	//  X X-X

	uaxis_2d tempAxis2;
	uaxis_2d tempAxis1;
	uint8_t centralHeight;
	uint8_t minHeight;

	tempAxis1.word = axis.word;
	for (int i = 0; i < 256 * 256; i++)
		mapTerrainType_10B4E0[i] = 3;
	centralHeight = mapHeightmap_11B4E0[axis.word];
	do
	{
		mapTerrainType_10B4E0[tempAxis1.word] = 0;
		tempAxis1._axis_2d.y--;
		minHeight = 255;
		if (mapTerrainType_10B4E0[tempAxis1.word] && mapHeightmap_11B4E0[tempAxis1.word] < 255)
		{
			minHeight = mapHeightmap_11B4E0[tempAxis1.word];
			tempAxis2.word = tempAxis1.word;
		}
		tempAxis1._axis_2d.x++;
		if (mapTerrainType_10B4E0[tempAxis1.word] && minHeight > mapHeightmap_11B4E0[tempAxis1.word])
		{
			minHeight = mapHeightmap_11B4E0[tempAxis1.word];
			tempAxis2.word = tempAxis1.word;
		}
		tempAxis1._axis_2d.y++;
		if (mapTerrainType_10B4E0[tempAxis1.word] && minHeight > mapHeightmap_11B4E0[tempAxis1.word])
		{
			minHeight = mapHeightmap_11B4E0[tempAxis1.word];
			tempAxis2.word = tempAxis1.word;
		}
		tempAxis1._axis_2d.y++;
		if (mapTerrainType_10B4E0[tempAxis1.word] && minHeight > mapHeightmap_11B4E0[tempAxis1.word])
		{
			minHeight = mapHeightmap_11B4E0[tempAxis1.word];
			tempAxis2.word = tempAxis1.word;
		}
		tempAxis1._axis_2d.x--;
		if (mapTerrainType_10B4E0[tempAxis1.word] && minHeight > mapHeightmap_11B4E0[tempAxis1.word])
		{
			minHeight = mapHeightmap_11B4E0[tempAxis1.word];
			tempAxis2.word = tempAxis1.word;
		}
		tempAxis1._axis_2d.x--;
		if (mapTerrainType_10B4E0[tempAxis1.word] && minHeight > mapHeightmap_11B4E0[tempAxis1.word])
		{
			minHeight = mapHeightmap_11B4E0[tempAxis1.word];
			tempAxis2.word = tempAxis1.word;
		}
		tempAxis1._axis_2d.y--;
		if (mapTerrainType_10B4E0[tempAxis1.word] && minHeight > mapHeightmap_11B4E0[tempAxis1.word])
		{
			minHeight = mapHeightmap_11B4E0[tempAxis1.word];
			tempAxis2.word = tempAxis1.word;
		}
		tempAxis1._axis_2d.y--;
		if (mapTerrainType_10B4E0[tempAxis1.word] && minHeight > mapHeightmap_11B4E0[tempAxis1.word])
		{
			minHeight = mapHeightmap_11B4E0[tempAxis1.word];
			tempAxis2.word = tempAxis1.word;
		}
		if (!mapAngle_13B4E0[tempAxis2.word] || minHeight == 255)
			break;
		if (minHeight > centralHeight)//if near tile is higger then central tile set central as near tile
			mapHeightmap_11B4E0[tempAxis2.word] = centralHeight;
		centralHeight = mapHeightmap_11B4E0[tempAxis2.word];
		tempAxis1.word = tempAxis2.word;
	} while (centralHeight);
	for (int i = 0; i < 256 * 256; i++)
		if (!mapTerrainType_10B4E0[i])
			mapAngle_13B4E0[i] = 0;
}

//----- (000439A0) --------------------------------------------------------
unsigned int sub_439A0(uint16_t index)//2249a0
{
	//    X
	//    |
	//  X-B-X
	//    |
	//    X

	uint8_t maxHeight;
	uint8_t minHeight;
	uint8_t centerPoint;
	unsigned int modSumaPoint;
	int sumaPoint = 0;
	unsigned int result = mapHeightmap_11B4E0[index];
	uaxis_2d uindex;
	uindex.word = index;
	if (mapAngle_13B4E0[uindex.word] & 7)
	{
		maxHeight = mapHeightmap_11B4E0[uindex.word];
		minHeight = maxHeight;
		centerPoint = mapHeightmap_11B4E0[uindex.word];
		uindex._axis_2d.y--;
		sumaPoint = mapHeightmap_11B4E0[uindex.word];
		if (mapHeightmap_11B4E0[uindex.word] > maxHeight)
			maxHeight = mapHeightmap_11B4E0[uindex.word];
		if (mapHeightmap_11B4E0[uindex.word] < minHeight)
			minHeight = mapHeightmap_11B4E0[uindex.word];
		uindex._axis_2d.x++;
		sumaPoint += mapHeightmap_11B4E0[uindex.word];
		if (mapHeightmap_11B4E0[uindex.word] > maxHeight)
			maxHeight = mapHeightmap_11B4E0[uindex.word];
		if (mapHeightmap_11B4E0[uindex.word] < minHeight)
			minHeight = mapHeightmap_11B4E0[uindex.word];
		uindex._axis_2d.y++;
		sumaPoint += mapHeightmap_11B4E0[uindex.word];
		if (mapHeightmap_11B4E0[uindex.word] > maxHeight)
			maxHeight = mapHeightmap_11B4E0[uindex.word];
		if (mapHeightmap_11B4E0[uindex.word] < minHeight)
			minHeight = mapHeightmap_11B4E0[uindex.word];
		uindex._axis_2d.y++;
		sumaPoint += mapHeightmap_11B4E0[uindex.word];
		if (mapHeightmap_11B4E0[uindex.word] > maxHeight)
			maxHeight = mapHeightmap_11B4E0[uindex.word];
		if (mapHeightmap_11B4E0[uindex.word] < minHeight)
			minHeight = mapHeightmap_11B4E0[uindex.word];
		uindex._axis_2d.x--;
		sumaPoint += mapHeightmap_11B4E0[uindex.word];
		if (mapHeightmap_11B4E0[uindex.word] > maxHeight)
			maxHeight = mapHeightmap_11B4E0[uindex.word];
		if (mapHeightmap_11B4E0[uindex.word] < minHeight)
			minHeight = mapHeightmap_11B4E0[uindex.word];
		uindex._axis_2d.x--;
		sumaPoint += mapHeightmap_11B4E0[uindex.word];
		if (mapHeightmap_11B4E0[uindex.word] > maxHeight)
			maxHeight = mapHeightmap_11B4E0[uindex.word];
		if (mapHeightmap_11B4E0[uindex.word] < minHeight)
			minHeight = mapHeightmap_11B4E0[uindex.word];
		uindex._axis_2d.y--;
		sumaPoint += mapHeightmap_11B4E0[uindex.word];
		if (mapHeightmap_11B4E0[uindex.word] > maxHeight)
			maxHeight = mapHeightmap_11B4E0[uindex.word];
		if (mapHeightmap_11B4E0[uindex.word] < minHeight)
			minHeight = mapHeightmap_11B4E0[uindex.word];
		uindex._axis_2d.y--;
		sumaPoint += mapHeightmap_11B4E0[uindex.word];
		if (mapHeightmap_11B4E0[uindex.word] > maxHeight)
			maxHeight = mapHeightmap_11B4E0[uindex.word];
		if (mapHeightmap_11B4E0[uindex.word] < minHeight)
			minHeight = mapHeightmap_11B4E0[uindex.word];
		modSumaPoint = sumaPoint >> 3;
		if ((uint8_t)(centerPoint - minHeight) <= 4)
		{
			if ((uint8_t)(maxHeight - centerPoint) <= 4)
				return result;
			if ((uint8_t)(maxHeight - centerPoint) <= 10)
				modSumaPoint = (centerPoint + modSumaPoint) >> 1;
		}
		else if ((uint8_t)(centerPoint - minHeight) <= 10)
		{
			return (modSumaPoint + centerPoint) >> 1;
		}
		result = modSumaPoint;
	}
	return result;
}

//----- (00043BB0) --------------------------------------------------------
void sub_43BB0()//224bb0
{
	int fuzzyHeight;
	unsigned int randSeed = 37487429;
	for (int i = 0; i < 256 * 256; i++)
	{
		if (!(mapAngle_13B4E0[i] & 8))
		{
			randSeed = 9377 * randSeed + 9439;
			fuzzyHeight = randSeed % 7 - 3 + x_BYTE_14B4E0_second_heightmap[i];
			if (fuzzyHeight < 0)
				fuzzyHeight = 0;
			if (fuzzyHeight > 254)
				fuzzyHeight = 254;
			x_BYTE_14B4E0_second_heightmap[i] = fuzzyHeight;
		}
	}
	for (int i = 0; i < 256 * 256; i++)
	{
		if (x_BYTE_14B4E0_second_heightmap[i] > mapHeightmap_11B4E0[i])
		{
			mapAngle_13B4E0[i] &= 0xF7u;
		}
		else
		{
			x_BYTE_14B4E0_second_heightmap[i] = mapHeightmap_11B4E0[i] - 1;
			mapAngle_13B4E0[i] |= 8;
		}
	}
}

//----- (0001B7A0) --------------------------------------------------------
int sub_1B7A0_tile_compare(axis_3d* a1)//1fc7a0
{

	//  X-X
	//    |
	//  B-X

	uaxis_2d uindex;
	uindex._axis_2d.x = a1->x >> 8;
	uindex._axis_2d.y = a1->y >> 8;
	uint8_t point1 = mapHeightmap_11B4E0[uindex.word];
	uindex._axis_2d.x++;
	uint8_t point2 = mapHeightmap_11B4E0[uindex.word];
	uindex._axis_2d.y++;
	uint8_t point3 = mapHeightmap_11B4E0[uindex.word];
	uindex._axis_2d.x--;
	uint8_t point4 = mapHeightmap_11B4E0[uindex.word];
	int result1 = abs(point1 + point4 - point2 - point3);
	int result2 = abs(point1 + point2 - point4 - point3);
	if (result1 > result2)
		return result1;
	return result2;
}

//----- (0001B830) --------------------------------------------------------
int sub_1B830(axis_3d* a1)//1fc830
{

	//  X-.-X
	//      |
	//      .
	//      |
	//  B-.-X

	uaxis_2d uindex;
	uindex._axis_2d.x = (a1->x >> 8) - 1;
	uindex._axis_2d.y = (a1->y >> 8) - 1;
	uint8_t point1 = mapHeightmap_11B4E0[uindex.word];
	uindex._axis_2d.x += 2;
	uint8_t point2 = mapHeightmap_11B4E0[uindex.word];
	uindex._axis_2d.y += 2;
	uint8_t point3 = mapHeightmap_11B4E0[uindex.word];
	uindex._axis_2d.x += 2;
	uint8_t point4 = mapHeightmap_11B4E0[uindex.word];
	int result1 = abs(point1 + point4 - point2 - point3);
	int result2 = abs(point1 + point2 - point4 - point3);
	if (result1 > result2)
		return result1;
	return result2;
}

//----- (00045BE0) --------------------------------------------------------
uint8_t sub_45BE0(uint8_t inType, uaxis_2d axis2d)//226be0
{
	//  X-X
	//  | |
	//  B-X

	uint8_t result;
	uint8_t type = 0;
	uint8_t minHeight = 255;
	uint8_t maxHeight = 0;
	if (mapHeightmap_11B4E0[axis2d.word])
	{
		maxHeight = mapHeightmap_11B4E0[axis2d.word];
		type = 0;
	}
	if (mapHeightmap_11B4E0[axis2d.word] < 255)
		minHeight = mapHeightmap_11B4E0[axis2d.word];
	axis2d._axis_2d.x++;
	if (mapHeightmap_11B4E0[axis2d.word] > maxHeight)
	{
		maxHeight = mapHeightmap_11B4E0[axis2d.word];
		type = 1;
	}
	if (mapHeightmap_11B4E0[axis2d.word] < minHeight)
		minHeight = mapHeightmap_11B4E0[axis2d.word];
	axis2d._axis_2d.y++;
	if (mapHeightmap_11B4E0[axis2d.word] > maxHeight)
	{
		maxHeight = mapHeightmap_11B4E0[axis2d.word];
		type = 2;
	}
	if (mapHeightmap_11B4E0[axis2d.word] < minHeight)
		minHeight = mapHeightmap_11B4E0[axis2d.word];
	axis2d._axis_2d.x--;
	if (mapHeightmap_11B4E0[axis2d.word] > maxHeight)
	{
		maxHeight = mapHeightmap_11B4E0[axis2d.word];
		type = 3;
	}
	if (mapHeightmap_11B4E0[axis2d.word] < minHeight)
		minHeight = mapHeightmap_11B4E0[axis2d.word];
	axis2d._axis_2d.y--;
	uint8_t maxHeight2 = 0;
	if (type && mapHeightmap_11B4E0[axis2d.word])
	{
		maxHeight2 = mapHeightmap_11B4E0[axis2d.word];
		inType = 0;
	}
	axis2d._axis_2d.x++;
	if (type != 1 && mapHeightmap_11B4E0[axis2d.word] > maxHeight2)
	{
		maxHeight2 = mapHeightmap_11B4E0[axis2d.word];
		inType = 1;
	}
	axis2d._axis_2d.y++;
	if (type != 2 && mapHeightmap_11B4E0[axis2d.word] > maxHeight2)
	{
		maxHeight2 = mapHeightmap_11B4E0[axis2d.word];
		inType = 2;
	}
	axis2d._axis_2d.x--;
	if (type != 3)
	{
		uint8_t minHeight2 = mapHeightmap_11B4E0[axis2d.word];
		if (minHeight2 > maxHeight2)
		{
			inType = 3;
			maxHeight2 = minHeight2;
		}
	}
	bool lowDiff = false;
	if (maxHeight - minHeight <= 8)
		lowDiff = true;
	if (maxHeight - maxHeight2 >= 8)
	{
		result = type;
		lowDiffHeightmap_D47DC = lowDiff;
		return result;
	}
	switch (type)
	{
	case 0:
		if (inType != 1)
			result = 7;
		else
			result = 4;
		break;
	case 1:
		if (inType == 2)
			result = 5;
		else
			result = 4;
		break;
	case 2:
		if (inType == 3)
			result = 6;
		else
			result = 5;
		break;
	case 3:
		if (inType)
			result = 6;
		else
			result = 7;
		break;
	default:
		result = 0;
		break;
	}
	lowDiffHeightmap_D47DC = lowDiff;
	return result;
}

//----- (00033F70) --------------------------------------------------------
bool sub_33F70(uint16_t inAxis)//214f70
{
	//    X
	//    |
	//  X-B-X
	//    |
	//    X

	int compHeight;
	bool result;
	uaxis_2d axis2d;
	uaxis_2d tempAxis2d;
	axis2d.word = inAxis;
	tempAxis2d.word = axis2d.word;
	tempAxis2d._axis_2d.x--;//x-1,y
	result = 1;
	if (mapTerrainType_10B4E0[tempAxis2d.word] == 8)
	{
		compHeight = mapHeightmap_11B4E0[axis2d.word] + 30;
		if (mapHeightmap_11B4E0[tempAxis2d.word] <= compHeight)
		{
			tempAxis2d._axis_2d.x += 2;//x+1,y
			if (mapHeightmap_11B4E0[axis2d.word] <= compHeight)
			{
				tempAxis2d._axis_2d.x--;
				tempAxis2d._axis_2d.y++;//x,y+1
				if (mapHeightmap_11B4E0[tempAxis2d.word] <= compHeight)
				{
					tempAxis2d._axis_2d.y -= 2;//x,y-1
					if (mapHeightmap_11B4E0[tempAxis2d.word] <= compHeight)
						result = 0;
				}
			}
		}
	}
	return result;
}

//----- (00045DC0) --------------------------------------------------------
void sub_45DC0(uint8_t inType, uaxis_2d axis2d, uint8_t type2)//226dc0
{
	char heightType;
	if (type2 >= 8u)
	{
		switch (type2)
		{
		case 8u:
			mapTerrainType_10B4E0[axis2d.word] = 8;
			break;
		case 9u:
			mapTerrainType_10B4E0[axis2d.word] = 9;
			break;
		case 0xAu:
			heightType = sub_45BE0(inType, axis2d);
			if (lowDiffHeightmap_D47DC)
				heightType += 8;
			mapTerrainType_10B4E0[axis2d.word] = unk_D4A30[0x80 + 2 * heightType];
			mapAngle_13B4E0[axis2d.word] = mapAngle_13B4E0[axis2d.word] & 0x8F | unk_D4A30[0x80 + 2 * heightType + 1];
			break;
		case 0xBu:
			heightType = sub_45BE0(inType, axis2d) + 0x10;
			if (lowDiffHeightmap_D47DC)
				heightType += 8;
			mapTerrainType_10B4E0[axis2d.word] = unk_D4A30[0x80 + 2 * heightType];
			mapAngle_13B4E0[axis2d.word] = mapAngle_13B4E0[axis2d.word] & 0x8F | unk_D4A30[0x80 + 2 * heightType + 1];
			break;
		case 0xCu:
			heightType = sub_45BE0(inType, axis2d) + 0x20;
			if (lowDiffHeightmap_D47DC)
				heightType += 8;
			mapTerrainType_10B4E0[axis2d.word] = unk_D4A30[0x80 + 2 * heightType];
			mapAngle_13B4E0[axis2d.word] = mapAngle_13B4E0[axis2d.word] & 0x8F | unk_D4A30[0x80 + 2 * heightType + 1];
			break;
		case 0xDu:
			heightType = sub_45BE0(inType, axis2d) + 0x30;
			if (lowDiffHeightmap_D47DC)
				heightType += 8;
			mapTerrainType_10B4E0[axis2d.word] = unk_D4A30[0x80 + 2 * heightType];
			mapAngle_13B4E0[axis2d.word] = mapAngle_13B4E0[axis2d.word] & 0x8F | unk_D4A30[0x80 + 2 * heightType + 1];
			break;
		case 0xEu:
			heightType = sub_45BE0(inType, axis2d) + 0x40;
			if (lowDiffHeightmap_D47DC)
				heightType += 8;
			mapTerrainType_10B4E0[axis2d.word] = unk_D4A30[0x80 + 2 * heightType];
			mapAngle_13B4E0[axis2d.word] = mapAngle_13B4E0[axis2d.word] & 0x8F | unk_D4A30[0x80 + 2 * heightType + 1];
			break;
		case 0xFu:
			mapTerrainType_10B4E0[axis2d.word] = 11;
			break;
		case 0x10u:
			if (mapTerrainType_10B4E0[axis2d.word] == 10 || mapTerrainType_10B4E0[axis2d.word] == 11 || mapTerrainType_10B4E0[axis2d.word] == 12)
				break;
			heightType = sub_45BE0(inType, axis2d);
			mapTerrainType_10B4E0[axis2d.word] = unk_D4A30[2 * heightType];
			mapAngle_13B4E0[axis2d.word] = mapAngle_13B4E0[axis2d.word] & 0x8F | unk_D4A30[2 * heightType + 1];
			break;
		case 0x11u:
			heightType = sub_45BE0(inType, axis2d);
			mapTerrainType_10B4E0[axis2d.word] = unk_D4A30[0x40 + 2 * heightType];
			mapAngle_13B4E0[axis2d.word] = mapAngle_13B4E0[axis2d.word] & 0x8F | unk_D4A30[0x40 + 2 * heightType + 1];
			break;
		case 0x12u:
			heightType = (8 * ((axis2d._axis_2d.y + axis2d._axis_2d.x) & 1) + sub_45BE0(inType, axis2d));
			mapTerrainType_10B4E0[axis2d.word] = unk_D4A30[0x50 + 2 * heightType];
			mapAngle_13B4E0[axis2d.word] = mapAngle_13B4E0[axis2d.word] & 0x8F | unk_D4A30[0x50 + 2 * heightType + 1];
			break;
		case 0x13u:
			heightType = (8 * ((axis2d._axis_2d.y + axis2d._axis_2d.x) & 1) + sub_45BE0(inType, axis2d));
			mapTerrainType_10B4E0[axis2d.word] = unk_D4A30[0x60 + 2 * heightType];
			mapAngle_13B4E0[axis2d.word] = mapAngle_13B4E0[axis2d.word] & 0x8F | unk_D4A30[0x60 + 2 * heightType + 1];
			break;
		case 0x14u:
			heightType = sub_45BE0(inType, axis2d);
			mapTerrainType_10B4E0[axis2d.word] = unk_D4A30[0x10 + 2 * heightType];
			mapAngle_13B4E0[axis2d.word] = mapAngle_13B4E0[axis2d.word] & 0x8F | unk_D4A30[0x10 + 2 * heightType + 1];
			break;
		case 0x15u:
			heightType = sub_45BE0(inType, axis2d);
			mapTerrainType_10B4E0[axis2d.word] = unk_D4A30[0x20 + 2 * heightType];
			mapAngle_13B4E0[axis2d.word] = mapAngle_13B4E0[axis2d.word] & 0x8F | unk_D4A30[0x20 + 2 * heightType + 1];
			break;
		case 0x16u:
			heightType = sub_45BE0(inType, axis2d);
			mapTerrainType_10B4E0[axis2d.word] = unk_D4A30[0x30 + 2 * heightType];
			mapAngle_13B4E0[axis2d.word] = mapAngle_13B4E0[axis2d.word] & 0x8F | unk_D4A30[0x30 + 2 * heightType + 1];
			break;
		default:
			break;
		}
		mapAngle_13B4E0[axis2d.word] |= 0x80;
		if (isCaveLevel_D41B6)
		{
			if (x_BYTE_14B4E0_second_heightmap[axis2d.word] > mapHeightmap_11B4E0[axis2d.word])
			{
				mapAngle_13B4E0[axis2d.word] &= 0xF7;
			}
			else
			{
				mapAngle_13B4E0[axis2d.word] |= 8;
				x_BYTE_14B4E0_second_heightmap[axis2d.word] = mapHeightmap_11B4E0[axis2d.word] - 1;
			}
			axis2d._axis_2d.x++;
			if (x_BYTE_14B4E0_second_heightmap[axis2d.word] > mapHeightmap_11B4E0[axis2d.word])
			{
				mapAngle_13B4E0[axis2d.word] &= 0xF7u;
			}
			else
			{
				x_BYTE_14B4E0_second_heightmap[axis2d.word] = mapHeightmap_11B4E0[axis2d.word] - 1;
				mapAngle_13B4E0[axis2d.word] |= 8;
			}
			axis2d._axis_2d.y++;
			if (x_BYTE_14B4E0_second_heightmap[axis2d.word] > mapHeightmap_11B4E0[axis2d.word])
			{
				mapAngle_13B4E0[axis2d.word] &= 0xF7u;
			}
			else
			{
				x_BYTE_14B4E0_second_heightmap[axis2d.word] = mapHeightmap_11B4E0[axis2d.word] - 1;
				mapAngle_13B4E0[axis2d.word] |= 8;
			}
			axis2d._axis_2d.x--;
			if (x_BYTE_14B4E0_second_heightmap[axis2d.word] <= mapHeightmap_11B4E0[axis2d.word])
			{
				x_BYTE_14B4E0_second_heightmap[axis2d.word] = mapHeightmap_11B4E0[axis2d.word] - 1;
				mapAngle_13B4E0[axis2d.word] |= 8;
				return;
			}
		}
		else
		{
			mapAngle_13B4E0[axis2d.word] &= 0xF7;
			axis2d._axis_2d.x++;
			mapAngle_13B4E0[axis2d.word] &= 0xF7;
			axis2d._axis_2d.y++;
			mapAngle_13B4E0[axis2d.word] &= 0xF7;
			axis2d._axis_2d.x--;
		}
		mapAngle_13B4E0[axis2d.word] &= 0xF7u;
		return;
	}
	mapAngle_13B4E0[axis2d.word] = type2 | mapAngle_13B4E0[axis2d.word] & 0xF0;
	sub_462A0(axis2d, axis2d);
}

//----- (000462A0) --------------------------------------------------------
void sub_462A0(uaxis_2d inAxis2dA, uaxis_2d inAxis2dB)//2272a0
{
	uaxis_2d tempAxis;
	uint8_t nextAngle;
	int terModIndex;
	char tempShad;
	char yAdd;
	char xAdd;
	uint8_t point1;
	uint8_t point2;
	uint8_t point3;
	uint8_t point4;

	tempAxis.word = inAxis2dA.word;
	for (int indexY = inAxis2dB._axis_2d.y - inAxis2dA._axis_2d.y + 1; indexY != 0; indexY--)
	{
		for (int indexX = inAxis2dB._axis_2d.x - inAxis2dA._axis_2d.x + 1; indexX; indexX--)
		{
			if ((int8_t)mapAngle_13B4E0[tempAxis.word] >= 0)
				mapTerrainType_10B4E0[tempAxis.word] = 1;
			tempAxis._axis_2d.x--;
			if ((int8_t)mapAngle_13B4E0[tempAxis.word] >= 0)
				mapTerrainType_10B4E0[tempAxis.word] = 1;
			tempAxis._axis_2d.y--;
			if ((int8_t)mapAngle_13B4E0[tempAxis.word] >= 0)
				mapTerrainType_10B4E0[tempAxis.word] = 1;
			tempAxis._axis_2d.x++;
			if ((int8_t)mapAngle_13B4E0[tempAxis.word] >= 0)
				mapTerrainType_10B4E0[tempAxis.word] = 1;
			tempAxis._axis_2d.x++;
			tempAxis._axis_2d.y++;			
		}
		tempAxis._axis_2d.x -= inAxis2dB._axis_2d.x - inAxis2dA._axis_2d.x + 1;
		tempAxis._axis_2d.y++;
	}
	xAdd = inAxis2dB._axis_2d.x - inAxis2dA._axis_2d.x + 2;
	yAdd = inAxis2dB._axis_2d.y - inAxis2dA._axis_2d.y + 2;	
	inAxis2dA._axis_2d.x--;
	inAxis2dA._axis_2d.y--;
	tempAxis.word = inAxis2dA.word;
	for (int indexY = yAdd; indexY; indexY--)
	{
		for (int indexX = xAdd; indexX; indexX--)
		{
			if (mapTerrainType_10B4E0[tempAxis.word] == 1)
			{
				point1 = mapAngle_13B4E0[tempAxis.word];
				tempAxis._axis_2d.x++;
				point2 = mapAngle_13B4E0[tempAxis.word];
				tempAxis._axis_2d.y++;
				point3 = mapAngle_13B4E0[tempAxis.word];
				tempAxis._axis_2d.x--;
				point4 = mapAngle_13B4E0[tempAxis.word];
				tempAxis._axis_2d.y--;
				terModIndex = (point4 & 7) + 7 * (point3 & 7) + 49 * (point2 & 7) + 343 * (point1 & 7);
				mapTerrainType_10B4E0[tempAxis.word] = x_BYTE_F2CD0x[terModIndex][0];
				if (x_BYTE_F2CD0x[terModIndex][0] >= 8u)
				{
					nextAngle = x_BYTE_F2CD0x[terModIndex][1] + (mapAngle_13B4E0[tempAxis.word] & 0x87);
				}
				else
				{
					x_WORD_17B4E0 = 9377 * x_WORD_17B4E0 + 9439;
					nextAngle = (mapAngle_13B4E0[tempAxis.word] & 0x87) + 16 * (x_WORD_17B4E0 % 7u);
				}
				mapAngle_13B4E0[tempAxis.word] = nextAngle;
			}
			tempAxis._axis_2d.x++;
		}
		tempAxis._axis_2d.x -= xAdd;
		tempAxis._axis_2d.y++;
	}
	xAdd++;
	yAdd++;
	tempAxis.word = inAxis2dA.word;
	if (yAdd != 0)
	{
		for (int indexY = yAdd; indexY; indexY--)
		{
			for (int indexX = xAdd; indexX; indexX--)
			{
				tempAxis._axis_2d.x++;
				tempAxis._axis_2d.y++;
				point1 = mapHeightmap_11B4E0[tempAxis.word];
				tempAxis._axis_2d.x -= 2;
				tempAxis._axis_2d.y -= 2;
				point2 = mapHeightmap_11B4E0[tempAxis.word];
				tempAxis._axis_2d.x++;
				tempShad = point2 - point1 + 32;
				tempAxis._axis_2d.y++;
				if (tempShad >= 28)
				{
					if (tempShad > 40)
						tempShad = (tempShad & 7) + 40;
				}
				else
				{
					tempShad = (tempShad & 3) + 28;
				}
				if (D41A0_0.terrain_2FECE.MapType != MapType_t::Day)
					mapShading_12B4E0[tempAxis.word] = 32 - tempShad + 32;
				else
					mapShading_12B4E0[tempAxis.word] = tempShad;
				if (isCaveLevel_D41B6 && (x_BYTE_14B4E0_second_heightmap[tempAxis.word] <= mapHeightmap_11B4E0[tempAxis.word]))
				{
					x_BYTE_14B4E0_second_heightmap[tempAxis.word] = mapHeightmap_11B4E0[tempAxis.word] - 1;
					mapAngle_13B4E0[tempAxis.word] |= 8u;
				}
				else
				{
					mapAngle_13B4E0[tempAxis.word] &= 0xF7u;
				}
				tempAxis._axis_2d.x++;
			}
			tempAxis._axis_2d.x -= xAdd;
			tempAxis._axis_2d.y++;
		}
	}
}

//----- (000104A0) --------------------------------------------------------
signed int sub_104A0(axis_3d* axis3d)//1f14a0
{
	return 1 << (mapAngle_13B4E0[(axis3d->x >> 8) + ((axis3d->y >> 8) << 8)] & 0xF);
}

//----- (000104D0) --------------------------------------------------------
uint32_t sub_104D0_terrain_tile_is_water(axis_3d* axis3d)//1f14d0
{
	uaxis_2d axis2d;
	axis2d._axis_2d.x = axis3d->x >> 8;
	axis2d._axis_2d.y = axis3d->y >> 8;
	return sub_10590_terrain_tile_type(mapTerrainType_10B4E0[axis2d.word]);
}

//----- (00010590) --------------------------------------------------------
uint32_t sub_10590_terrain_tile_type(char tileType)//1f1590
{
	uint32_t result = 0;
	switch (tileType)
	{
	case 0:
		result = 1;
		break;
	case 1:
		result = 2;
		break;
	case 2:
		result = 4;
		break;
	case 3:
		result = 8;
		break;
	case 4:
		result = 0x10;
		break;
	case 5:
		result = 0x20;
		break;
	case 8:
		result = 0x100;
		break;
	case 9:
		result = 0x200;
		break;
	case 10:
		result = 0x100000;
		break;
	case 11:
		result = 0x200000;
		break;
	case 12:
		result = 0x400000;
		break;
	case 13:
	case 14:
		return result;
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
	case 33:
	case 34:
		result = 0x400;
		break;
	case 21:
	case 22:
	case 24:
		result = 0x20000;
		break;
	case 23:
		result = 0x40000;
		break;
	case 25:
	case 27:
		result = 0x80000;
		break;
	case 26:
		result = 0x10000;
		break;
	default:
		result = 0x800000;
		break;
	}
	return result;
}

//----- (00010C40) --------------------------------------------------------
int getTerrainAlt_10C40(axis_3d* axis3d)//1f1c40
{
	return sub_B5C60_getTerrainAlt2(axis3d->x, axis3d->y);
}

//----- (00011E70) --------------------------------------------------------
bool sub_11E70(type_event_0x6E8E* event, axis_3d* axis3d)//1f2e70
{
	return event->array_0x52_82.fov + getTerrainAlt_10C40(axis3d) + event->dword_0xA0_160x->word_160_0xc_12 > sub_10C60(axis3d);
}

//----- (00010C60) --------------------------------------------------------
int sub_10C60(axis_3d* axis3d)//1f1c60
{
	return sub_B5D68(axis3d->x, axis3d->y);
}

//----- (000B5D68) --------------------------------------------------------
int sub_B5D68(uint16_t inX, uint16_t inY)//296d68
{
	uint8_t point1;
	uint8_t point2;
	int finalComp;
	int result;

	uaxis_2d tempAxis;
	uaxis_2d tempInX;
	uaxis_2d tempInY;
	tempInX.word = inX;
	tempInY.word = inY;
	tempAxis._axis_2d.x = tempInX._axis_2d.y;
	tempAxis._axis_2d.y = tempInY._axis_2d.y;

	if ((tempInX._axis_2d.y + tempInY._axis_2d.y) & 1)
	{
		if (tempInY._axis_2d.x > (tempInY._axis_2d.x + tempInX._axis_2d.x) % 256)
		{
			tempAxis._axis_2d.y++;
			point1 = x_BYTE_14B4E0_second_heightmap[tempAxis.word];
			tempAxis._axis_2d.x++;
			point2 = x_BYTE_14B4E0_second_heightmap[tempAxis.word];
			tempInY._axis_2d.x = 255 - tempInY._axis_2d.x;
			tempAxis._axis_2d.y--;
			finalComp = tempInY._axis_2d.x * (x_BYTE_14B4E0_second_heightmap[tempAxis.word] - point2) + tempInX._axis_2d.x * (point2 - point1);
		}
		else
		{
			point1 = x_BYTE_14B4E0_second_heightmap[tempAxis.word];
			tempAxis._axis_2d.x++;
			point2 = x_BYTE_14B4E0_second_heightmap[tempAxis.word];
			tempAxis._axis_2d.x--;
			tempAxis._axis_2d.y++;
			finalComp = tempInY._axis_2d.x * (x_BYTE_14B4E0_second_heightmap[tempAxis.word] - point1) + tempInX._axis_2d.x * (point2 - point1);
		}
		result = (finalComp >> 3) + 32 * point1;
	}
	else
	{
		if (tempInX._axis_2d.x <= tempInY._axis_2d.x)
		{
			point1 = x_BYTE_14B4E0_second_heightmap[tempAxis.word];
			tempAxis._axis_2d.y++;
			point2 = x_BYTE_14B4E0_second_heightmap[tempAxis.word];
			tempAxis._axis_2d.x++;
			finalComp = tempInY._axis_2d.x * (point2 - point1) + tempInX._axis_2d.x * (x_BYTE_14B4E0_second_heightmap[tempAxis.word] - point2);
		}
		else
		{
			point1 = x_BYTE_14B4E0_second_heightmap[tempAxis.word];
			tempAxis._axis_2d.x++;
			point2 = x_BYTE_14B4E0_second_heightmap[tempAxis.word];
			tempAxis._axis_2d.y++;
			finalComp = tempInY._axis_2d.x * (x_BYTE_14B4E0_second_heightmap[tempAxis.word] - point2) + tempInX._axis_2d.x * (point2 - point1);
		}
		result = (finalComp >> 3) + 32 * point1;
	}
	return result;
}

void Convert_to_shadow_D41A0_BYTESTR_0(type_D41A0_BYTESTR_0* from, type_shadow_D41A0_BYTESTR_0* to) {
	for (int i = 0; i < 4; i++)to->stub0[i] = from->stub0[i];
	to->dword_0x4 = from->dword_0x4;
	to->dword_0x8 = from->rand_0x8;
	to->word_0xc = from->LevelIndex_0xc;
	to->word_0xe = from->word_0xe;
	for (int i = 0; i < 0x1d; i++)to->array_0x10[i] = from->array_0x10[i];
	to->dword_0x2d = from->dword_0x2d;
	to->word_0x31 = from->word_0x31;
	to->word_0x33 = from->word_0x33;
	to->dword_0x35 = from->dword_0x35;
	for (int i = 0; i < 508; i++)to->array_0x39[i] = from->array_0x39[i];
	to->dword_0x235 = from->maptypeMusic_0x235;
	to->byte_0x239 = from->byte_0x239;
	to->dword_0x23a = from->dword_0x23a;
	to->dword_0x23e = from->dword_0x23e;
	to->dword_0x242 = from->dword_0x242;
	for (int i = 0; i < 0x3e8; i++)to->pointers_0x246[i] = ((uint8_t*)from->pointers_0x246[i] - Zero_pointer);
	to->dword_0x11e6 = from->dword_0x11e6;
	for (int i = 0; i < 0x3e8; i++)to->dword_0x11EA[i] = ((uint8_t*)from->dword_0x11EA[i] - Zero_pointer);
	to->m_GameSettings = from->m_GameSettings;
	to->dword_0x219A = from->dword_0x219A;
	to->dword_0x219E = from->dword_0x219E;
	to->dword_0x21A2 = from->dword_0x21A2;
	to->dword_0x21A6 = from->dword_0x21A6;
	to->str_0x21AA = from->str_0x21AA;
	to->str_0x21AE = from->str_0x21AE;
	to->str_0x21B2 = from->str_0x21B2;
	to->str_0x21B6 = from->str_0x21B6;
	for (int i = 0; i < 0x1a8; i++)to->stub3b[i] = from->stub3b[i];
	for (int i = 0; i < 8; i++)to->array_0x2362[i] = from->array_0x2362[i];
	for (int i = 0; i < 0x14d; i++)to->stub3c[i] = from->stub3c[i];
	for (int i = 0; i < 0x6ff; i++)to->stub3d[i] = from->stub3d[i];
	for (int i = 0; i < 8; i++)to->array_0x2BDE[i] = from->array_0x2BDE[i];
	for (int i = 0; i < 8; i++)to->array_0x6E3E[i] = from->array_0x6E3E[i];
	for (int i = 0; i < 0x3e8; i++) {
		to->struct_0x6E8E[i].next_0 = ((uint8_t*)from->struct_0x6E8E[i].next_0 - Zero_pointer);
		to->struct_0x6E8E[i].dword_0x4 = from->struct_0x6E8E[i].maxLife_0x4;
		to->struct_0x6E8E[i].dword_0x8 = from->struct_0x6E8E[i].life_0x8;
		to->struct_0x6E8E[i].struct_byte_0xc_12_15 = from->struct_0x6E8E[i].struct_byte_0xc_12_15;
		to->struct_0x6E8E[i].dword_0x10_16 = from->struct_0x6E8E[i].dword_0x10_16;
		to->struct_0x6E8E[i].word_0x14_20 = from->struct_0x6E8E[i].rand_0x14_20;
		to->struct_0x6E8E[i].word_0x16_22 = from->struct_0x6E8E[i].oldMapEntity_0x16_22;
		to->struct_0x6E8E[i].word_0x18_24_next_entity = from->struct_0x6E8E[i].nextEntity_0x18_24;
		to->struct_0x6E8E[i].word_0x1A_26 = from->struct_0x6E8E[i].id_0x1A_26;
		to->struct_0x6E8E[i].word_0x1C_28 = from->struct_0x6E8E[i].word_0x1C_28;
		to->struct_0x6E8E[i].word_0x1E_30 = from->struct_0x6E8E[i].word_0x1E_30;
		to->struct_0x6E8E[i].word_0x20_32 = from->struct_0x6E8E[i].word_0x20_32;
		to->struct_0x6E8E[i].word_0x22_34 = from->struct_0x6E8E[i].word_0x22_34;
		to->struct_0x6E8E[i].word_0x24_36 = from->struct_0x6E8E[i].word_0x24_36;
		to->struct_0x6E8E[i].word_0x26_38 = from->struct_0x6E8E[i].word_0x26_38;
		to->struct_0x6E8E[i].word_0x28_40 = from->struct_0x6E8E[i].parentId_0x28_40;
		to->struct_0x6E8E[i].word_0x2A_42 = from->struct_0x6E8E[i].word_0x2A_42;
		to->struct_0x6E8E[i].word_0x2C_44 = from->struct_0x6E8E[i].word_0x2C_44;
		to->struct_0x6E8E[i].word_0x2E_46 = from->struct_0x6E8E[i].word_0x2E_46;
		to->struct_0x6E8E[i].word_0x30_48 = from->struct_0x6E8E[i].word_0x30_48;
		to->struct_0x6E8E[i].word_0x32_50 = from->struct_0x6E8E[i].word_0x32_50;
		to->struct_0x6E8E[i].word_0x34_52 = from->struct_0x6E8E[i].word_0x34_52;
		to->struct_0x6E8E[i].word_0x36_54 = from->struct_0x6E8E[i].word_0x36_54;
		to->struct_0x6E8E[i].byte_0x38_56 = from->struct_0x6E8E[i].byte_0x38_56;
		to->struct_0x6E8E[i].byte_0x39_57 = from->struct_0x6E8E[i].byte_0x39_57;
		to->struct_0x6E8E[i].byte_0x3A_58 = from->struct_0x6E8E[i].byte_0x3A_58;
		to->struct_0x6E8E[i].byte_0x3B_59 = from->struct_0x6E8E[i].byte_0x3B_59;
		to->struct_0x6E8E[i].byte_0x3C_60 = from->struct_0x6E8E[i].byte_0x3C_60;
		to->struct_0x6E8E[i].byte_0x3D_61 = from->struct_0x6E8E[i].byte_0x3D_61;
		to->struct_0x6E8E[i].byte_0x3E_62 = from->struct_0x6E8E[i].byte_0x3E_62;
		to->struct_0x6E8E[i].type_0x3F_63 = from->struct_0x6E8E[i].class_0x3F_63;
		to->struct_0x6E8E[i].subtype_0x40_64 = from->struct_0x6E8E[i].model_0x40_64;
		to->struct_0x6E8E[i].byte_0x41_65 = from->struct_0x6E8E[i].xtype_0x41_65;
		to->struct_0x6E8E[i].byte_0x42_66 = from->struct_0x6E8E[i].xsubtype_0x42_66;
		to->struct_0x6E8E[i].byte_0x43_67 = from->struct_0x6E8E[i].byte_0x43_67;
		to->struct_0x6E8E[i].byte_0x44_68 = from->struct_0x6E8E[i].byte_0x44_68;
		to->struct_0x6E8E[i].byte_0x45_69 = from->struct_0x6E8E[i].state_0x45_69;
		to->struct_0x6E8E[i].byte_0x46_70 = from->struct_0x6E8E[i].byte_0x46_70;
		to->struct_0x6E8E[i].byte_0x47_71_xx = from->struct_0x6E8E[i].byte_0x47_71_xx;
		to->struct_0x6E8E[i].byte_0x48_72 = from->struct_0x6E8E[i].StageVar1_0x48_72;
		to->struct_0x6E8E[i].byte_0x49_73 = from->struct_0x6E8E[i].StageVar2_0x49_73;
		to->struct_0x6E8E[i].word_0x4A_74 = from->struct_0x6E8E[i].word_0x4A_74;
		to->struct_0x6E8E[i].array_0x4C_76 = from->struct_0x6E8E[i].axis_0x4C_76;
		to->struct_0x6E8E[i].array_0x52_82 = from->struct_0x6E8E[i].array_0x52_82;
		to->struct_0x6E8E[i].word_0x5A_90 = from->struct_0x6E8E[i].word_0x5A_90;
		to->struct_0x6E8E[i].byte_0x5C_92 = from->struct_0x6E8E[i].byte_0x5C_92;
		to->struct_0x6E8E[i].byte_0x5D_93 = from->struct_0x6E8E[i].byte_0x5D_93;
		to->struct_0x6E8E[i].str_0x5E_94 = from->struct_0x6E8E[i].str_0x5E_94;
		to->struct_0x6E8E[i].word_0x82_130 = from->struct_0x6E8E[i].actSpeed_0x82_130;
		to->struct_0x6E8E[i].word_0x84_132 = from->struct_0x6E8E[i].minSpeed_0x84_132;
		to->struct_0x6E8E[i].word_0x86_134 = from->struct_0x6E8E[i].maxSpeed_0x86_134;
		to->struct_0x6E8E[i].dword_0x88_136 = from->struct_0x6E8E[i].dword_0x88_136;
		to->struct_0x6E8E[i].dword_0x8C_140 = from->struct_0x6E8E[i].maxMana_0x8C_140;
		to->struct_0x6E8E[i].dword_0x90_144 = from->struct_0x6E8E[i].mana_0x90_144;
		to->struct_0x6E8E[i].word_0x94_148 = from->struct_0x6E8E[i].word_0x94_148;
		to->struct_0x6E8E[i].word_0x96_150 = from->struct_0x6E8E[i].word_0x96_150;
		to->struct_0x6E8E[i].word_0x98_152 = from->struct_0x6E8E[i].word_0x98_152;
		to->struct_0x6E8E[i].word_0x9A_154x = from->struct_0x6E8E[i].word_0x9A_154x;
		to->struct_0x6E8E[i].dword_0xA0_160x = ((uint8_t*)from->struct_0x6E8E[i].dword_0xA0_160x - Zero_pointer);
		to->struct_0x6E8E[i].dword_0xA4_164x = ((uint8_t*)from->struct_0x6E8E[i].dword_0xA4_164x - Zero_pointer);
	}

	to->str_2FECE.word_2FECE = from->terrain_2FECE.word_2FECE;
	to->str_2FECE.word_2FED0 = from->terrain_2FECE.word_2FED0;
	to->str_2FECE.byte_0x2FED2 = from->terrain_2FECE.byte_0x2FED2;
	to->str_2FECE.byte_0x2FED3 = from->terrain_2FECE.byte_0x2FED3;
	to->str_2FECE.MapType = static_cast<std::underlying_type<MapType_t>::type>(from->terrain_2FECE.MapType);
	to->str_2FECE.word_0x2FED5 = from->terrain_2FECE.word_0x2FED5;
	to->str_2FECE.word_0x2FED7 = from->terrain_2FECE.word_0x2FED7;
	for (int i = 0; i < 8; i++)to->str_2FECE.array_0x2FED9[i] = from->terrain_2FECE.player_0x2FED9[i];
	for (int i = 0; i < 4; i++)to->str_2FECE.stubb[i] = from->terrain_2FECE.stubb[i];
	to->str_2FECE.word_0x2FEE5 = from->terrain_2FECE.seed_0x2FEE5;
	for (int i = 0; i < 2; i++)to->str_2FECE.stubb2[i] = from->terrain_2FECE.stubb2[i];
	to->str_2FECE.word_0x2FEE9 = from->terrain_2FECE.offset_0x2FEE9;
	for (int i = 0; i < 2; i++)to->str_2FECE.stubc[i] = from->terrain_2FECE.stubc[i];
	to->str_2FECE.word_0x2FEED = from->terrain_2FECE.raise_0x2FEED;
	for (int i = 0; i < 2; i++)to->str_2FECE.stubc2[i] = from->terrain_2FECE.stubc2[i];
	to->str_2FECE.word_0x2FEF1 = from->terrain_2FECE.gnarl_0x2FEF1;
	for (int i = 0; i < 2; i++)to->str_2FECE.stubc3[i] = from->terrain_2FECE.stubc3[i];
	to->str_2FECE.word_0x2FEF5 = from->terrain_2FECE.river_0x2FEF5;
	to->str_2FECE.word_0x2FEF9 = from->terrain_2FECE.lriver_0x2FEF9;
	for (int i = 0; i < 2; i++)to->str_2FECE.stube[i] = from->terrain_2FECE.stube[i];
	to->str_2FECE.word_0x2FEFD = from->terrain_2FECE.source_0x2FEFD;
	for (int i = 0; i < 2; i++)to->str_2FECE.stubf[i] = from->terrain_2FECE.stubf[i];
	to->str_2FECE.word_0x2FF01 = from->terrain_2FECE.snLin_0x2FF01;
	for (int i = 0; i < 2; i++)to->str_2FECE.stubg[i] = from->terrain_2FECE.stubg[i];
	to->str_2FECE.word_0x2FF05 = from->terrain_2FECE.snFlt_0x2FF05;
	for (int i = 0; i < 2; i++)to->str_2FECE.stubh[i] = from->terrain_2FECE.stubh[i];
	to->str_2FECE.word_0x2FF09 = from->terrain_2FECE.bhLin_0x2FF09;
	for (int i = 0; i < 2; i++)to->str_2FECE.stubi[i] = from->terrain_2FECE.stubi[i];
	to->str_2FECE.word_0x2FF0D = from->terrain_2FECE.bhFlt_0x2FF0D;
	for (int i = 0; i < 2; i++)to->str_2FECE.stubj[i] = from->terrain_2FECE.stubj[i];
	to->str_2FECE.word_0x2FF11 = from->terrain_2FECE.rkSte_0x2FF11;
	for (int i = 0; i < 1022; i++)to->str_2FECE.stubk[i] = from->terrain_2FECE.stubk[i];
	for (int i = 0; i < 1200; i++)to->str_2FECE.array_0x30311[i] = from->terrain_2FECE.entity_0x30311[i];
	to->str_2FECE.next_0x360D1 = from->terrain_2FECE.next_0x360D1;
	for (int i = 0; i < 8; i++)to->str_2FECE.next_0x360D2[i] = from->terrain_2FECE.next_0x360D2[i];
	for (int i = 0; i < 8; i++)to->str_2FECE.str_0x36442[i] = from->terrain_2FECE.stages_0x36442[i];
	for (int i = 0; i < 11; i++)
	{
		to->str_2FECE.array_0x3647A[i].str_0x3647A_byte_0 = from->terrain_2FECE.StageVars_0x3647A[i].index_0x3647A_0;
		to->str_2FECE.array_0x3647A[i].str_0x3647A_byte_1 = from->terrain_2FECE.StageVars_0x3647A[i].stage_0x3647A_1;
		to->str_2FECE.array_0x3647A[i].str_0x3647A_2 = from->terrain_2FECE.StageVars_0x3647A[i].str_0x3647A_2;
		to->str_2FECE.array_0x3647A[i].str_0x3647C_4.axis = from->terrain_2FECE.StageVars_0x3647A[i].str_0x3647C_4.axis;
	}
	to->str_0x364D2 = from->str_0x364D2;
	to->word_0x3653E = from->word_0x3653E;
	to->word_0x36540 = from->word_0x36540;
	to->word_0x36542 = from->word_0x36542;
	to->word_0x36544 = from->word_0x36544;
	to->word_0x36546 = from->word_0x36546;
	to->word_0x36548 = from->word_0x36548;
	to->word_0x3654A = from->word_0x3654A;
	for (int i = 0; i < 8; i++) {
		to->struct_0x3654C[i].str_3654C_byte0 = from->stages_0x3654C[i].stages_3654C_byte0;
		to->struct_0x3654C[i].str_3654D_byte1 = from->stages_0x3654C[i].str_3654D_byte1;
		to->struct_0x3654C[i].str_3654E_axis = from->stages_0x3654C[i].str_3654E_axis;
		to->struct_0x3654C[i].str_36552_un.dword = from->stages_0x3654C[i].str_36552_un.dword;
	}
	for (int i = 0; i < 8; i++)to->struct_0x3659C[i] = from->struct_0x3659C[i];
	for (int i = 0; i < 11; i++) {
		to->array_0x365F4[i].str_0x3647A_byte_0 = from->StageVars2_0x365F4[i].index_0x3647A_0;
		to->array_0x365F4[i].str_0x3647A_byte_1 = from->StageVars2_0x365F4[i].stage_0x3647A_1;
		to->array_0x365F4[i].str_0x3647A_2 = from->StageVars2_0x365F4[i].str_0x3647A_2;
		to->array_0x365F4[i].str_0x3647C_4.axis = from->StageVars2_0x365F4[i].str_0x3647C_4.axis;
	}
	for (int i = 0; i < 0x32; i++) {
		to->str_0x3664C[i].byte_0 = from->str_0x3664C[i].byte_0;
		to->str_0x3664C[i].byte_1 = from->str_0x3664C[i].byte_1;
		to->str_0x3664C[i].byte_2 = from->str_0x3664C[i].byte_2;
		to->str_0x3664C[i].byte_3 = from->str_0x3664C[i].byte_3;
		to->str_0x3664C[i].axis3d_4 = from->str_0x3664C[i].axis3d_4;
		to->str_0x3664C[i].dword_A = ((uint8_t*)from->str_0x3664C[i].event_A.pointer_0x6E8E - Zero_pointer);
		for (int j = 0; j < 25; j++)to->str_0x3664C[i].array_E[j] = from->str_0x3664C[i].array_E[j];
	}
	to->byte_0x36DEA_fly_asistant = from->byte_0x36DEA_fly_asistant;
	to->byte_0x36DEB_xx = from->byte_0x36DEB_xx;
	to->word_0x36DEC_mousex = from->word_0x36DEC_mousex;
	to->word_0x36DEE_mousey = from->word_0x36DEE_mousey;
	to->word_0x36DF0_mousexx = from->word_0x36DF0_mousexx;
	to->dword_0x36DF2 = from->dword_0x36DF2;
	to->dword_0x36DF6 = ((uint8_t*)from->dword_0x36DF6 - Zero_pointer);
	to->word_0x36DFA = from->word_0x36DFA;
	to->word_0x36DFC = from->word_0x36DFC;
	to->word_0x36DFE = from->word_0x36DFE;
	to->byte_0x36E00 = from->countStageVars_0x36E00;
	to->byte_0x36E01 = from->stageIndex_0x36E01;
	to->byte_0x36E02 = from->byte_0x36E02;
	to->byte_0x36E03 = from->byte_0x36E03;
	to->byte_0x36E04 = from->byte_0x36E04;
	for (int i = 0; i < 6; i++)to->stub3k[i] = from->stub3k[i];
	to->byte_0x36E0B = from->byte_0x36E0B;
	for (int i = 0; i < 11; i++)to->stubend[i] = from->stubend[i];
}



