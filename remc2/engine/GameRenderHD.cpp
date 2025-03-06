#include "GameRenderHD.h"

#include <array>
#include <thread>

#include "../portability/bitmap_pos_struct.h"
#include "../utilities/BitmapIO.h"
#include "../utilities/Maths.h"
#include "../utilities/RendererTests.h"
#include "../utilities/SafeQueue.h"
#include "Terrain.h"
#include "TextureMaps.h"
#include "Type_D404C.h"
#include "Type_D93C0_Bldgprmbuffer.h"
#include "Type_D94F0_Bldgprmbuffer.h"
#include "Type_E9C38_Smalltit.h"
#include "Type_F2C20ar.h"
#include "Type_WORD_D951C.h"
#include "XUnk_D4350.h"
#include "defs.h"


GameRenderHD::GameRenderHD(uint8_t* ptrScreenBuffer, uint8_t* pColorPalette, uint8_t renderThreads, bool assignToSpecificCores) : 
	m_ptrDWORD_E9C38_smalltit(new uint8_t[GAME_RES_MAX_WIDTH * GAME_RES_MAX_HEIGHT]),
	m_ptrScreenBuffer_351628(ptrScreenBuffer),
	m_ptrColorPalette(pColorPalette),
	m_assignToSpecificCores(assignToSpecificCores)
{
	SetRenderThreads(renderThreads);
	m_preBlurBuffer_E9C3C = new uint8_t[((GAME_RES_MAX_WIDTH * GAME_RES_MAX_HEIGHT) * 3)]; // Allow x 3 padding for sprite rendering
	m_ptrBlurBuffer_E9C3C = &m_preBlurBuffer_E9C3C[(GAME_RES_MAX_WIDTH * GAME_RES_MAX_HEIGHT)];
}

GameRenderHD::~GameRenderHD()
{
	delete[] m_ptrDWORD_E9C38_smalltit;
	delete[] m_preBlurBuffer_E9C3C;

	if (m_renderThreads.size() > 0)
	{
		StopWorkerThreads();
	}
}

void GameRenderHD::DrawWorld_411A0(int posX, int posY, int16_t yaw, int16_t posZ, int16_t pitch, int16_t roll, int16_t fov)
{
	uint16_t v8; // ax
	int v9; // ecx
	int v10; // ebx
	int v11; // edx
	int v12; // ecx
	int v13; // ebx
	int v14; // edx
	int v15; // ecx
	int v16; // ebx
	int v17; // edx
	int v18; // ecx
	int v19; // ebx
	int v20; // edx
	int vYaw; // esi
	int v22; // edx
	int v23; // ebx
	uint32_t v24; // edx
	int v25; // ebx
	int v26; // edi
	int v28; // ebx
	uint32_t v29; // edx
	int v30; // ebx
	int v31; // edi
	uint8_t* v32; // ST2C_4
	__int64 v34; // rax
	uint8_t* v35; // edi
	int v36; // eax
	x_BYTE* v37; // esi
	signed int v38; // ecx
	uint16_t v39; // bx
	uint16_t v40; // dx
	uint16_t v41; // bx
	uint16_t v42; // dx
	x_BYTE* v43; // edi
	int v44; // esi
	int v45; // ecx
	int v46; // eax
	int v47; // ebx
	int v48; // edx
	int v49; // [esp+0h] [ebp-1Ch]
	int v50; // [esp+4h] [ebp-18h]
	signed int v51; // [esp+8h] [ebp-14h]
	char v52; // [esp+Ch] [ebp-10h]
	uint8_t* v53; // [esp+14h] [ebp-8h]
	int i; // [esp+18h] [ebp-4h]
	int vPosX; // [esp+34h] [ebp+18h]
	int vPosY; // [esp+38h] [ebp+1Ch]
	LOBYTE(v8) = HIBYTE(posX);
	HIBYTE(v8) = HIBYTE(posY);
	if ((signed int)(uint8_t)posX < 128)
		LOBYTE(v8) = HIBYTE(posX) - 1;
	if ((signed int)(uint8_t)posY < 128)
		HIBYTE(v8) = HIBYTE(posY) - 1;
	v9 = mapHeightmap_11B4E0[v8];
	LOBYTE(v8) += 2;
	v10 = v9;
	v11 = v9;
	v12 = mapHeightmap_11B4E0[v8];
	HIBYTE(v8) += 2;
	v13 = v10 - v12;
	v14 = v12 + v11;
	v15 = mapHeightmap_11B4E0[v8];
	LOBYTE(v8) -= 2;
	v16 = v13 - v15;
	v17 = v14 - v15;
	v18 = mapHeightmap_11B4E0[v8];
	v19 = 2 * (v18 + v16);
	v20 = 2 * (v17 - v18);
	if (v19 <= 100)
	{
		if (v19 < -100)
			v19 = -100;
	}
	else
	{
		v19 = 100;
	}
	if (v20 <= 100)
	{
		if (v20 < -100)
			v20 = -100;
	}
	else
	{
		v20 = 100;
	}
	vYaw = yaw & 0x7FF;
	x_DWORD_D4794 += (v19 - x_DWORD_D4794) >> 3;
	x_DWORD_D4798 += (v20 - x_DWORD_D4798) >> 3;
	vPosX = x_DWORD_D4794 + posX;
	vPosY = x_DWORD_D4798 + posY;

	if (D41A0_0.m_GameSettings.str_0x2192.xxxx_0x2193 && D41A0_0.m_GameSettings.m_Display.m_uiScreenSize && screenWidth_18062C == 640)
	{
		//VR interlaced render
		viewPort.SetRenderViewPortSize_BCD45(
			m_ptrScreenBuffer_351628,
			2 * screenWidth_18062C,
			screenWidth_18062C / 2 - 8,
			screenHeight_180624 / 2 - 40);
		v22 = Maths::sin_DB750[vYaw];
		x_DWORD_D4790 = 20;
		v23 = 5 * v22;
		v24 = Maths::sin_DB750[512 + vYaw];
		x_DWORD_D4324 = -5;
		v25 = 4 * v23 >> 16;
		v26 = 20 * (signed int)v24 >> 16;
		DrawTerrainAndParticles_3C080(vPosX - v26, vPosY - v25, vYaw, posZ, pitch, roll, fov);
		viewPort.SetRenderViewPortSize_BCD45(m_ptrScreenBuffer_351628 + (screenWidth_18062C / 2), 0, 0, 0);
		x_DWORD_D4324 = 5;
		DrawTerrainAndParticles_3C080(vPosX + v26, vPosY + v25, vYaw, posZ, pitch, roll, fov);
		x_DWORD_D4324 = 0;
		viewPort.SetRenderViewPortSize_BCD45(m_ptrScreenBuffer_351628, screenWidth_18062C, screenWidth_18062C, screenHeight_180624);
	}
	else if (D41A0_0.m_GameSettings.m_Display.m_uiScreenSize != 1 || D41A0_0.m_GameSettings.str_0x2192.xxxx_0x2193)
	{
		v52 = D41A0_0.m_GameSettings.m_Display.xxxx_0x2191;
		if (x_WORD_180660_VGA_type_resolution == 1)
		{
			if (!D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].byte_0x3DF_2BE4_12221)
			{
				if (x_D41A0_BYTEARRAY_4_struct.m_wHighSpeedSystem)
				{
					if (m_ptrBlurBuffer_E9C3C)
					{
						if (D41A0_0.m_GameSettings.m_Graphics.m_wViewPortSize == 40)
						{
							v34 = x_DWORD_EA3E4[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].word_0x00a_2BE4_11240]->actSpeed_0x82_130;
							if ((signed int)((HIDWORD(v34) ^ v34) - HIDWORD(v34)) > 80)
								D41A0_0.m_GameSettings.m_Display.xxxx_0x2191 = 1;
						}
					}
				}
			}
		}
		if (D41A0_0.str_0x21AE.xxxx_0x21B1 && D41A0_0.m_GameSettings.m_Display.xxxx_0x2191 && m_ptrBlurBuffer_E9C3C)
		{
			//Blur
			v35 = ViewPortRenderBufferStart_DE558;
			viewPort.SetRenderViewPortSize_BCD45(m_ptrBlurBuffer_E9C3C, 0, 0, 0);
			DrawTerrainAndParticles_3C080(vPosX, vPosY, vYaw, posZ, pitch, roll, fov);
			//Apply Blur
			viewPort.SetRenderViewPortSize_BCD45(v35, 0, 0, 0);
			v51 = (signed int)(uint16_t)viewPort.Width_DE564 >> 2;
			v49 = iScreenWidth_DE560 - (uint16_t)viewPort.Width_DE564;
			v50 = (uint16_t)viewPort.Height_DE568;
			
			if (D41A0_0.m_GameSettings.m_Display.xxxx_0x2191 != 1)
			{
				v37 = (x_BYTE*)m_ptrBlurBuffer_E9C3C;
				goto LABEL_33;
			}
			v37 = (x_BYTE*)m_ptrBlurBuffer_E9C3C;
			v38 = (signed int)(uint16_t)viewPort.Width_DE564 >> 2;
			LOBYTE(v39) = *(x_BYTE*)(m_ptrBlurBuffer_E9C3C + 2);
			HIBYTE(v39) = v35[2];
			LOBYTE(v40) = *(x_BYTE*)(m_ptrBlurBuffer_E9C3C + 3);
			LOBYTE(v36) = x_BYTE_F6EE0_tablesx[16384 + v39];
			HIBYTE(v40) = v35[3];
			HIBYTE(v36) = x_BYTE_F6EE0_tablesx[16384 + v40];
			v36 <<= 16;
			LOBYTE(v39) = *(x_BYTE*)m_ptrBlurBuffer_E9C3C;
			HIBYTE(v39) = *v35;
			LOBYTE(v40) = *(x_BYTE*)(m_ptrBlurBuffer_E9C3C + 1);
			LOBYTE(v36) = x_BYTE_F6EE0_tablesx[16384 + v39];
			HIBYTE(v40) = v35[1];
			for (BYTE1(v36) = x_BYTE_F6EE0_tablesx[16384 + v40]; ; BYTE1(v36) = x_BYTE_F6EE0_tablesx[16384 + v42])
			{
				*(x_DWORD*)v35 = v36;
				v35 += 4;
				v37 += 4;
				if (!--v38)
				{
					HIWORD(v36) = HIWORD(v49);
					v37 += v49;
					v35 += v49;
					if (!--v50)
						goto LABEL_44;
				LABEL_33:
					v38 = v51;
				}
				HIBYTE(v41) = v37[2];
				LOBYTE(v41) = v35[2];
				HIBYTE(v42) = v37[3];
				LOBYTE(v36) = x_BYTE_F6EE0_tablesx[16384 + v41];
				LOBYTE(v42) = v35[3];
				HIBYTE(v36) = x_BYTE_F6EE0_tablesx[16384 + v42];
				v36 <<= 16;
				HIBYTE(v41) = *v37;
				LOBYTE(v41) = *v35;
				HIBYTE(v42) = v37[1];
				LOBYTE(v36) = x_BYTE_F6EE0_tablesx[16384 + v41];
				LOBYTE(v42) = v35[1];
			}
		}

		DrawTerrainAndParticles_3C080(vPosX, vPosY, vYaw, posZ, pitch, roll, fov);

		if (D41A0_0.m_GameSettings.str_0x2192.xxxx_0x2192)
		{
			v53 = ViewPortRenderBufferStart_DE558;
			for (i = (uint16_t)viewPort.Height_DE568 - 1; i; i--)
			{
				v43 = (x_BYTE*)v53;
				v44 = iScreenWidth_DE560;
				v45 = (uint16_t)viewPort.Width_DE564 - 1;
				HIWORD(v46) = 0;
				HIWORD(v47) = 0;
				HIWORD(v48) = 0;
				do
				{
					LOBYTE(v46) = v43[0];
					LOBYTE(v47) = v43[1];
					BYTE1(v46) = v43[v44];
					LOBYTE(v48) = x_BYTE_F6EE0_tablesx[16384 + v46];
					BYTE1(v47) = v43[v44 + 1];
					BYTE1(v48) = x_BYTE_F6EE0_tablesx[16384 + v47];
					*v43++ = x_BYTE_F6EE0_tablesx[16384 + v48];
					v45--;
				} while (v45);
				v53 += iScreenWidth_DE560;
			}
		}
	LABEL_44:
		D41A0_0.m_GameSettings.m_Display.xxxx_0x2191 = v52;
	}
	else
	{
		v28 = 5 * Maths::sin_DB750[vYaw];
		v29 = Maths::sin_DB750[512 + vYaw];
		x_DWORD_D4790 = 20;
		x_DWORD_D4324 = iScreenWidth_DE560 / 40;
		v30 = 4 * v28 >> 16;
		v31 = 20 * (signed int)v29 >> 16;
		DrawTerrainAndParticles_3C080(v31 + vPosX, v30 + vPosY, vYaw, posZ, pitch, roll, fov);
		v32 = ViewPortRenderBufferStart_DE558;
		viewPort.SetRenderViewPortSize_BCD45(m_ptrBlurBuffer_E9C3C, 0, 0, 0);
		x_DWORD_D4324 = 0 - (iScreenWidth_DE560 / 40);
		DrawTerrainAndParticles_3C080(vPosX - v31, vPosY - v30, vYaw, posZ, pitch, roll, fov);
		viewPort.SetRenderViewPortSize_BCD45(v32, 0, 0, 0);
		x_DWORD_D4324 = 0;
	}
}

void GameRenderHD::WriteWorldToBMP()
{
	std::string path = GetSubDirectoryPath("BufferOut");
	if (myaccess(path.c_str(), 0) < 0)
	{
		mymkdir(path.c_str());
	}

	path = GetSubDirectoryPath("BufferOut");
	path.append("/PaletteOut.bmp");
	BitmapIO::WritePaletteAsImageBMP(path.c_str(), 256, m_ptrColorPalette);
	path = GetSubDirectoryPath("BufferOut");
	path.append("/BufferOut.bmp");
	BitmapIO::WriteImageBufferAsImageBMP(path.c_str(), screenWidth_18062C, screenHeight_180624, m_ptrColorPalette, m_ptrScreenBuffer_351628);
}

void GameRenderHD::WriteWholeBufferToBmp()
{
	std::string path = GetSubDirectoryPath("BufferOut");
	if (myaccess(path.c_str(), 0) < 0)
	{
		mymkdir(path.c_str());
	}

	int stride = (screenWidth_18062C * screenHeight_180624);

	path = GetSubDirectoryPath("BufferOut");
	path.append("/WholeBufferOutTop.bmp");
	BitmapIO::WriteImageBufferAsImageBMP(path.c_str(), screenWidth_18062C, screenHeight_180624, m_ptrColorPalette, m_ptrScreenBuffer_351628 - stride);

	path = GetSubDirectoryPath("BufferOut");
	path.append("/WholeBufferOutMiddle.bmp");
	BitmapIO::WriteImageBufferAsImageBMP(path.c_str(), screenWidth_18062C, screenHeight_180624, m_ptrColorPalette, m_ptrScreenBuffer_351628);

	path = GetSubDirectoryPath("BufferOut");
	path.append("/WholeBufferOutBottom.bmp");
	BitmapIO::WriteImageBufferAsImageBMP(path.c_str(), screenWidth_18062C, screenHeight_180624, m_ptrColorPalette, m_ptrScreenBuffer_351628 + stride);
}

void GameRenderHD::ClearGraphicsBuffer(uint8_t colorIdx)
{
	if (colorIdx > 255)
	{
		colorIdx = 255;
	}
	memset32(m_ptrScreenBuffer_351628, colorIdx, screenWidth_18062C * screenHeight_180624);
}

void GameRenderHD::DrawSky_40950_TH(int16_t roll)
{
	if (m_renderThreads.size() > 0)
	{
		uint8_t drawEveryNthLine = m_renderThreads.size() + 1;
		uint8_t i = 0;

		for (i = 0; i < m_renderThreads.size(); i++)
		{
			m_renderThreads[i]->Run([this, roll, i, drawEveryNthLine] {
				this->DrawSky_40950(roll, i, drawEveryNthLine);
			});
		}

		this->DrawSky_40950(roll, i, drawEveryNthLine);

		WaitForRenderFinish();

	}
	else
	{
		DrawSky_40950(roll, 0, 1);
	}
}

/*
* Sky texture is currently 256x256
*/
void GameRenderHD::DrawSky_40950(int16_t roll, uint8_t startLine, uint8_t drawEveryNthLine)
{
	if (drawEveryNthLine < 1)
	{
		drawEveryNthLine = 1;
	}

	int skyTextSize = 256;
	if (x_BYTE_D41B5_texture_size == 128)
	{
		skyTextSize = 1024;
	}
	int lineWidthSQ = skyTextSize * skyTextSize;

	bsaxis_2d errLine[3840]; // for 4K
	uint32 beginX;
	uint32 beginY;
	int roundRoll = roll & 0x7FF;
	int sinRoll = (Maths::sin_DB750[roundRoll] * skyTextSize) / viewPort.Width_DE564;
	int cosRoll = (Maths::sin_DB750[512 + roundRoll] * skyTextSize) / viewPort.Width_DE564;
	int errorX = 0;
	int errorY = 0;
	int8_t oldErrorX = 0;
	int8_t oldErrorY = 0;

	// prepare sky texture lookup table
	for (uint16_t width = 0; width < viewPort.Width_DE564; width++)
	{
		errLine[width].x = (errorX >> 16) - oldErrorX;
		errLine[width].y = (errorY >> 16) - oldErrorY;
		oldErrorX = (errorX >> 16);
		oldErrorY = (errorY >> 16);
		errorY += sinRoll;
		errorX += cosRoll;
	}

	uint8_t* viewPortRenderBufferStart = (ViewPortRenderBufferStart_DE558 + (startLine * iScreenWidth_DE560));
	int addX = (-(str_F2C20ar.sin_0x0d * str_F2C20ar.dword0x22) >> 16) + str_F2C20ar.dword0x24;
	int addY = str_F2C20ar.dword0x10 - (str_F2C20ar.cos_0x11 * str_F2C20ar.dword0x22 >> 16);
	beginX = (yaw_F2CC0 << 15) * (skyTextSize / 256) - (addX * cosRoll - addY * sinRoll);
	beginY = -(cosRoll * addY + sinRoll * addX);

	beginX -= (sinRoll * startLine);
	beginY += (cosRoll * startLine);

	for (int height = 0; height < viewPort.Height_DE568; height+= drawEveryNthLine)
	{
		uint8* viewPortLineRenderBufferStart = viewPortRenderBufferStart;

		uint32 texturePixelIndexX = (beginX >> 16);
		uint32 texturePixelIndexY = (beginY >> 16);
		if (skyTextSize == 0x100)
		{
			texturePixelIndexX %= (skyTextSize - 1);
			texturePixelIndexY %= (skyTextSize - 1);
		}

		//Scales sky texture to viewport
		for (uint16_t width = 0; width < viewPort.Width_DE564; width++)
		{
			*viewPortLineRenderBufferStart = off_D41A8_sky[(texturePixelIndexX + skyTextSize * texturePixelIndexY) % lineWidthSQ];
			texturePixelIndexX = (texturePixelIndexX + errLine[width].x + skyTextSize) % skyTextSize;
			texturePixelIndexY = (texturePixelIndexY + errLine[width].y + skyTextSize) % skyTextSize;
			viewPortLineRenderBufferStart++;
		}
		viewPortRenderBufferStart = viewPortRenderBufferStart + (iScreenWidth_DE560 * drawEveryNthLine);
		beginX -= (sinRoll * drawEveryNthLine);
		beginY += (cosRoll * drawEveryNthLine);
	}	
}

/*
* Draws Terrain, Sprites and Particals using a Painter's algorithm.
*/
void GameRenderHD::DrawTerrainAndParticles_3C080(__int16 posX, __int16 posY, __int16 yaw, signed int posZ, int pitch, int16_t roll, int fov)
{
	int sinIdx = 0;
	int sinIdx2 = 0;
	int v9; // eax
	int v10; // edx
	int v11; // ecx
	int v12; // edx
	int v13; // edi
	char v14; // dh
	int v15x;
	char v16; // dl
	char v17; // dl
	int v18x;
	char v19; // dh
	int v20; // ebx
	//int v21; // ecx
	char v22; // ch
	int v23; // eax
	uint8_t* v25x; // edi
	uint16_t v26; // dx
	int v27; // ebx
	int v28; // eax
	__int16 v29; // si
	int v30; // edx
	__int16 v31; // cx
	int v32; // eax
	int v33; // ecx
	signed int v34; // esi
	int v35; // ebx
	uint16_t v36; // dx
	int v37; // eax
	__int16 v38; // ax
	int v39; // eax
	int v40; // edi
	//int v41x; // edx
	uint16_t v42; // bx
	int v43x;
	uint8_t* v44; // eax
	char v45; // bh
	signed int v46; // edx
	int v47x;
	int v52; // ecx
	int v53; // ebx
	signed int v54; // esi
	signed int v55; // esi
	int v56x;
	signed int v109; // esi
	int v110; // ebx
	uint16_t v111; // dx
	__int16 tickIdx; // ax
	int v113; // eax
	//int v114x;
	//signed int v115; // edx
	int v116; // eax
	int v117x;
	uint16_t v118; // bx
	uint8_t v119; // al
	int v120x;
	uint8_t* v121; // eax
	int v122; // bh
	signed int v123; // ebx
	int v124x;
	int pnt1_16; // esi
	int pnt4_28; // ecx
	int pnt2_20; // ecx
	int v129; // ecx
	int v130; // edx
	signed int v131; // esi
	signed int v132; // esi
	//int v159; // eax
	//char v194; // ch
	//char v196; // ch
	int v197; // ecx
	signed int v198; // esi
	int v199; // ebx
	uint16_t v200; // di
	__int16 v201; // ax
	int v202; // eax
	int v203; // eax
	uint16_t v204; // bx
	int v205x;
	int v206x;
	uint8_t* v207; // eax
	int v208; // eax
	signed int v209; // ebx
	signed int v216; // esi
	std::vector<int> projectedVertexBuffer(33);  //[33]; // [esp+0h] [ebp-62h]//v248x[0]
	uint8_t* v277; // [esp+84h] [ebp+22h]
	//uint8_t* v278;
	int v278x;
	uint16_t v279; // [esp+8Ch] [ebp+2Ah]
	int l; // [esp+90h] [ebp+2Eh]
	char v283; // [esp+9Ch] [ebp+3Ah]
	char k; // [esp+A0h] [ebp+3Eh]
	char v285; // [esp+A4h] [ebp+42h]
	char i; // [esp+A8h] [ebp+46h]
	int jj; // [esp+ACh] [ebp+4Ah]

	int a1 = 0;
	int a2 = 0;

	shadows_F2CC7 = D41A0_0.m_GameSettings.m_Graphics.m_wShadows;//21d080
	notDay_D4320 = D41A0_0.terrain_2FECE.MapType != MapType_t::Day;
	str_F2C20ar.dword0x10 = (signed int)(uint16_t)viewPort.Height_DE568 >> 1;
	x_WORD_F2CC4 = posX;
	yaw_F2CC0 = yaw & 0x7FF;
	x_WORD_F2CC2 = posY;
	v9 = (yaw & 0x7FF) + 256;
	str_F2C20ar.dword0x20 = posZ;
	v10 = Maths::sin_DB750[256 + v9];
	str_F2C20ar.dword0x24 = x_DWORD_D4324 + ((signed int)(uint16_t)viewPort.Width_DE564 >> 1);
	str_F2C20ar.dword0x0f = v10;
	v11 = Maths::sin_DB750[v9 - 256];
	v12 = ((((yaw & 0x7FF) + 256) & 0x1FF) - 256) & 0x7FF;
	projectedVertexBuffer[32] = (v9 >> 9) & 3;
	projectedVertexBuffer[30] = Maths::sin_DB750[512 + v12];
	str_F2C20ar.dword0x17 = v11;
	v13 = Maths::sin_DB750[v12];
	SetBillboards_3B560(-roll & 0x7FF);//21d1aa
	str_F2C20ar.dword0x18 = 7
		* Maths::sub_7277A_radix_3d(
			(uint16_t)viewPort.Width_DE564 * (uint16_t)viewPort.Width_DE564
			+ (uint16_t)viewPort.Height_DE568 * (uint16_t)viewPort.Height_DE568)
		* fov >> 11;
	v277 = unk_D4328x + 10 * projectedVertexBuffer[32];

	//This is based on rotation direction there is always a direction
	switch ((uint8_t)projectedVertexBuffer[32])//fixed? //rotations
	{
	case 0u: // 270 -> 0
		a2 = (uint8_t)posY - 256;
		a1 = -(uint8_t)posX - 4864;
		break;
	case 1u: // 0 -> 90
		a1 = -(uint8_t)posY - 4864;
		a2 = -(uint8_t)posX;
		break;
	case 2u: // 90 -> 180
		a1 = (uint8_t)posX - 4864;
		a2 = -(uint8_t)posY;
		break;
	case 3u: // 180 -> 270
		a1 = (uint8_t)posY - 4864;
		a2 = (uint8_t)posX - 256;
		break;
	default:
		break;
	}

	v14 = 40;//21d231
	v15x = 0;
	
	Logger->trace("------DrawTerrainAndParticles_3C080: {}-------", viewPort.Width_DE564);
	do//filling first pointer of m_ptrDWORD_E9C38_smalltit(3f52a4)//prepare billboards
	{
		projectedVertexBuffer[29] = a1 * v13 >> 16;
		v16 = 21;
		projectedVertexBuffer[28] = a1 * projectedVertexBuffer[30] >> 16;
		while (v16)
		{
			Str_E9C38_smalltit[v15x].x_0 = projectedVertexBuffer[28];
			Str_E9C38_smalltit[v15x].y_12 = projectedVertexBuffer[29];
			if (a1 < 0)
				Str_E9C38_smalltit[v15x].triangleFeatures_38 = 0;
			else
				Str_E9C38_smalltit[v15x].triangleFeatures_38 = 4;
			v15x += 40;
			v16--;
		}
		v15x -= 839;
		a1 += 256;
		v14--;
	} while (v14);

	v17 = 21;//21d29c not drawing
	v18x = 0;
	while (v17)
	{
		projectedVertexBuffer[27] = a2 * v13 >> 16;
		v19 = 40;
		v20 = a2 * projectedVertexBuffer[30] >> 16;
		while (v19)
		{
			Str_E9C38_smalltit[v18x].x_0 -= projectedVertexBuffer[27];
			Str_E9C38_smalltit[v18x].y_12 += v20;// +v21;
			v18x++;
			v19--;
		}
		a2 += 256;
		v17--;
	}

	str_F2C20ar.dword0x15_tileRenderCutOffDistance = 26214400; //Distance cut-off for tile render
	v278x = 0;
	str_F2C20ar.dword0x12 = 8912896;
	v22 = v277[0];
	str_F2C20ar.dword0x22 = pitch * (uint16_t)viewPort.Width_DE564 >> 8;
	LOBYTE(v279) = v22 + HIBYTE(posX);
	HIBYTE(v279) = v277[1] + HIBYTE(posY);
	v23 = roll & 0x7FF;
	str_F2C20ar.cos_0x11 = Maths::sin_DB750[512 + v23];
	str_F2C20ar.dword0x16 = 23658496;
	str_F2C20ar.sin_0x0d = Maths::sin_DB750[v23];
	str_F2C20ar.dword0x13 = 14745600;
	if (!D41A0_0.m_GameSettings.m_Graphics.m_wSky || isCaveLevel_D41B6)
	{
		v26 = viewPort.Width_DE564;
		v27 = iScreenWidth_DE560 - viewPort.Width_DE564;
		v28 = (v26 - (__CFSHL__((signed int)v26 >> 31, 2) + 4 * ((signed int)v26 >> 31))) >> 2;
		v29 = viewPort.Height_DE568;
		v25x = ViewPortRenderBufferStart_DE558;
		v30 = (v26 - (__CFSHL__((signed int)v26 >> 31, 2) + 4 * ((signed int)v26 >> 31))) >> 2;
		LOBYTE(v28) = keyColor1_D4B7C;
		HIBYTE(v28) = keyColor1_D4B7C;
		v31 = v28;
		v32 = v28 << 16;
		LOWORD(v32) = v31;
		do
		{
			memset32(v25x, v32, v30 * 4);
			v25x += 4 * v30 + v27;
			v29--;
		} while (v29);
	}
	else
	{
		if (m_renderThreads.size() > 0)
		{
			DrawSky_40950_TH(roll);
		}
		else
		{
			DrawSky_40950(roll, 0, 1);
		}
	}
	//Cave Level Render
	if (isCaveLevel_D41B6)//21d3e3 cleaned screen
	{
		for (i = 21; ; i--)
		{
			if (!i)
			{
				//Geometry tiles Distance 0 = near player
				v46 = 840;
				v47x = 0;
				while (v46)
				{
					//Rotation and Translation X
					pnt1_16 = CalculateRotationTranslationX(str_F2C20ar.cos_0x11, Str_E9C38_smalltit[v47x].pnt1_16, str_F2C20ar.sin_0x0d, Str_E9C38_smalltit[v47x].pnt2_20);
					projectedVertexBuffer[25] = CalculateRotationTranslationX(str_F2C20ar.cos_0x11, Str_E9C38_smalltit[v47x].pnt1_16, str_F2C20ar.sin_0x0d, Str_E9C38_smalltit[v47x].pnt4_28);

					//Rotation and Translation Y
					projectedVertexBuffer[24] = CalculateRotationTranslationY(Str_E9C38_smalltit[v47x].pnt1_16, str_F2C20ar.sin_0x0d, str_F2C20ar.cos_0x11, Str_E9C38_smalltit[v47x].pnt2_20);
					pnt4_28 = CalculateRotationTranslationY(Str_E9C38_smalltit[v47x].pnt1_16, str_F2C20ar.sin_0x0d, str_F2C20ar.cos_0x11, Str_E9C38_smalltit[v47x].pnt4_28);

					Str_E9C38_smalltit[v47x].pnt1_16 = pnt1_16;
					v52 = projectedVertexBuffer[24];
					Str_E9C38_smalltit[v47x].pnt4_28 = pnt4_28;
					Str_E9C38_smalltit[v47x].pnt2_20 = v52;
					v53 = Str_E9C38_smalltit[v47x].pnt1_16;
					Str_E9C38_smalltit[v47x].pnt3_24 = projectedVertexBuffer[25];
					if (v53 >= 0)
					{
						if ((signed int)(uint16_t)viewPort.Width_DE564 <= Str_E9C38_smalltit[v47x].pnt1_16)
							Str_E9C38_smalltit[v47x].triangleFeatures_38 |= 0x10u;
					}
					else
					{
						Str_E9C38_smalltit[v47x].triangleFeatures_38 |= 8u;
					}
					v54 = Str_E9C38_smalltit[v47x].pnt2_20;
					if (v54 >= 0)
					{
						if ((uint16_t)viewPort.Height_DE568 <= v54)
							Str_E9C38_smalltit[v47x].triangleFeatures_38 |= 0x40u;
					}
					else
					{
						Str_E9C38_smalltit[v47x].triangleFeatures_38 |= 0x20u;
					}
					if (Str_E9C38_smalltit[v47x].pnt3_24 >= 0)
					{
						if ((signed int)(uint16_t)viewPort.Width_DE564 <= Str_E9C38_smalltit[v47x].pnt3_24)
							Str_E9C38_smalltit[v47x].triangleFeatures_38 |= 0x200u;
					}
					else
					{
						Str_E9C38_smalltit[v47x].triangleFeatures_38 |= 0x100u;
					}
					v55 = Str_E9C38_smalltit[v47x].pnt4_28;
					if (v55 >= 0)
					{
						if ((uint16_t)viewPort.Height_DE568 <= v55)
							Str_E9C38_smalltit[v47x].triangleFeatures_38 |= 0x800u;
					}
					else
					{
						Str_E9C38_smalltit[v47x].triangleFeatures_38 |= 0x400u;
					}
					v47x++;
					v46--;
				}
				SubDrawCaveTerrainAndParticles(projectedVertexBuffer, pitch);
				return;
			}
			for (k = 40; k; k--)
			{
				v33 = ((uint8_t)mapShading_12B4E0[v279] << 8) + 128;
				v34 = Str_E9C38_smalltit[v278x].y_12;
				v35 = v34 * v34 + Str_E9C38_smalltit[v278x].x_0 * Str_E9C38_smalltit[v278x].x_0;
				Str_E9C38_smalltit[v278x].haveBillboard_36 = 0;
				if (v34 <= -256 || v35 >= str_F2C20ar.dword0x15_tileRenderCutOffDistance)
				{
					Str_E9C38_smalltit[v278x].triangleFeatures_38 |= 2u;
					goto LABEL_46;
				}
				if (v34 < 128)
					v34 = 128;
				Str_E9C38_smalltit[v278x].pnt1_16 = str_F2C20ar.dword0x18 * Str_E9C38_smalltit[v278x].x_0 / v34;
				v36 = v279;
				Str_E9C38_smalltit[v278x].alt_4 = 32 * mapHeightmap_11B4E0[v279] - posZ;
				Str_E9C38_smalltit[v278x].inverse_alt_8 = ((uint8_t)x_BYTE_14B4E0_second_heightmap[v36] << 15 >> 10) - posZ;
				v37 = 0;
				if (!mapTerrainType_10B4E0[v36])
				{
					v38 = 32 * D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dword_0x012_2BE0_11248;
					v37 = (Maths::sin_DB750[(v38 + (HIBYTE(v279) << 7)) & 0x7FF] >> 8)
						* (Maths::sin_DB750[(((uint8_t)v279 << 7) + v38) & 0x7FF] >> 8);
					Str_E9C38_smalltit[v278x].alt_4 -= v37 >> 13;
					if (v33 >= 14464)
						v37 = 0;
				}
				v39 = (v33 << 8) + 8 * v37;
				if (v35 <= str_F2C20ar.dword0x13)
					goto LABEL_39;
				if (v35 < str_F2C20ar.dword0x16)
				{
					v39 = v39 * (signed __int64)(str_F2C20ar.dword0x16 - v35) / str_F2C20ar.dword0x12;
				LABEL_39:
					Str_E9C38_smalltit[v278x].pnt5_32 = v39;
					goto LABEL_40;
				}
				Str_E9C38_smalltit[v278x].pnt5_32 = 0;
			LABEL_40:
				if (mapAngle_13B4E0[v279] & 8)
					Str_E9C38_smalltit[v278x].triangleFeatures_38 |= 0x80u;
				v40 = str_F2C20ar.dword0x18;
				//v41x = v278x;
				Str_E9C38_smalltit[v278x].pnt2_20 = str_F2C20ar.dword0x22 + str_F2C20ar.dword0x18 * Str_E9C38_smalltit[v278x].alt_4 / v34;
				Str_E9C38_smalltit[v278x].pnt4_28 = str_F2C20ar.dword0x22 + v40 * Str_E9C38_smalltit[v278x].inverse_alt_8 / v34;
				LOBYTE(v42) = v277[2] + v279;
				HIBYTE(v42) = v277[3] + HIBYTE(v279);
				v43x = v278x;
				Str_E9C38_smalltit[v278x].textIndex_41 = mapTerrainType_10B4E0[v42];
				if (D41A0_0.m_GameSettings.str_0x2196.flat_0x2199)
					Str_E9C38_smalltit[v43x].triangleFeatures_38 |= 0x1000u;
				Str_E9C38_smalltit[v278x].textAtyp_43 = Maths::x_BYTE_D41D8[Str_E9C38_smalltit[v278x].textIndex_41];
				Str_E9C38_smalltit[v278x].textUV_42 = projectedVertexBuffer[32] + (((signed int)(uint8_t)mapAngle_13B4E0[v42] >> 2) & 0x1C);
				LOBYTE(v42) = v277[4] + v42;
				HIBYTE(v42) += v277[5];
				Str_E9C38_smalltit[v278x].haveBillboard_36 = mapEntityIndex_15B4E0[v42];
			LABEL_46:
				v44 = v277;
				Str_E9C38_smalltit[v278x].triangleFeatures_38 |= ((x_BYTE)v279 + HIBYTE(v279)) & 1;
				LOBYTE(v279) = v44[8] + v279;
				HIBYTE(v279) += v277[9];
				v278x++;
			}
			v45 = v277[6] + v279;
			HIBYTE(v279) += v277[7];
			LOBYTE(v279) = v45;
		}
	}
	//Draw Terrain with Reflections
	if (D41A0_0.m_GameSettings.m_Graphics.m_wReflections)
	{
		Logger->trace("Start Drawing Terrain Frame with Reflection");
		for (l = 21; ; l--)
		{
			if (!l)
			{
				//Geometry tiles Distance 0 = near player
				v123 = 840;
				v124x = 0;
				while (v123)
				{
					//Rotation and Translation X
					pnt1_16 = CalculateRotationTranslationX(str_F2C20ar.cos_0x11, Str_E9C38_smalltit[v124x].pnt1_16, str_F2C20ar.sin_0x0d, Str_E9C38_smalltit[v124x].pnt2_20);
					projectedVertexBuffer[25] = CalculateRotationTranslationX(str_F2C20ar.cos_0x11, Str_E9C38_smalltit[v124x].pnt1_16, str_F2C20ar.sin_0x0d, Str_E9C38_smalltit[v124x].pnt4_28);

					//Rotation and Translation Y
					projectedVertexBuffer[24] = CalculateRotationTranslationY(Str_E9C38_smalltit[v124x].pnt1_16, str_F2C20ar.sin_0x0d, str_F2C20ar.cos_0x11, Str_E9C38_smalltit[v124x].pnt2_20);
					pnt4_28 = CalculateRotationTranslationY(Str_E9C38_smalltit[v124x].pnt1_16, str_F2C20ar.sin_0x0d, str_F2C20ar.cos_0x11, Str_E9C38_smalltit[v124x].pnt4_28);

					Str_E9C38_smalltit[v124x].pnt1_16 = pnt1_16;
					v129 = projectedVertexBuffer[24];
					Str_E9C38_smalltit[v124x].pnt4_28 = pnt4_28;
					Str_E9C38_smalltit[v124x].pnt2_20 = v129;
					v130 = Str_E9C38_smalltit[v124x].pnt1_16;
					Str_E9C38_smalltit[v124x].pnt3_24 = projectedVertexBuffer[25];
					if (v130 >= 0)
					{
						if ((signed int)(uint16_t)viewPort.Width_DE564 <= Str_E9C38_smalltit[v124x].pnt1_16)
							Str_E9C38_smalltit[v124x].triangleFeatures_38 |= 0x10u;
					}
					else
					{
						Str_E9C38_smalltit[v124x].triangleFeatures_38 |= 8u;
					}
					v131 = Str_E9C38_smalltit[v124x].pnt2_20;
					if (v131 >= 0)
					{
						if ((uint16_t)viewPort.Height_DE568 <= v131)
							Str_E9C38_smalltit[v124x].triangleFeatures_38 |= 0x40u;
					}
					else
					{
						Str_E9C38_smalltit[v124x].triangleFeatures_38 |= 0x20u;
					}
					if (Str_E9C38_smalltit[v124x].pnt3_24 >= 0)
					{
						if ((signed int)(uint16_t)viewPort.Width_DE564 <= Str_E9C38_smalltit[v124x].pnt3_24)
							Str_E9C38_smalltit[v124x].triangleFeatures_38 |= 0x200u;
					}
					else
					{
						Str_E9C38_smalltit[v124x].triangleFeatures_38 |= 0x100u;
					}
					v132 = Str_E9C38_smalltit[v124x].pnt4_28;
					if (v132 >= 0)
					{
						if ((uint16_t)viewPort.Height_DE568 <= v132)
							Str_E9C38_smalltit[v124x].triangleFeatures_38 |= 0x800u;
					}
					else
					{
						Str_E9C38_smalltit[v124x].triangleFeatures_38 |= 0x400u;
					}
					v124x++;
					v123--;
				}
				if (posZ < 4096)
				{
					SubDrawInverseTerrainAndParticles(projectedVertexBuffer, pitch);
				}
				//Draw rest of terrain
				SubDrawTerrainAndParticles(projectedVertexBuffer, pitch);
				Logger->trace("Finished Drawing Terrain Frame with Reflection");
				return;
			}

			//Populate vertexes?
			for (jj = 40; jj; --jj)
			{
				projectedVertexBuffer[31] = ((uint8_t)mapShading_12B4E0[v279] << 8) + 128;
				v109 = Str_E9C38_smalltit[v278x].y_12;
				v110 = v109 * v109 + Str_E9C38_smalltit[v278x].x_0 * Str_E9C38_smalltit[v278x].x_0;
				Str_E9C38_smalltit[v278x].haveBillboard_36 = 0;
				if (v109 <= -256 || v110 >= str_F2C20ar.dword0x15_tileRenderCutOffDistance)
				{
					Str_E9C38_smalltit[v278x].triangleFeatures_38 |= 2u; // reflections on
					goto LABEL_140;
				}
				if (v109 < 128)
					v109 = 128;
				Str_E9C38_smalltit[v278x].pnt1_16 = str_F2C20ar.dword0x18 * Str_E9C38_smalltit[v278x].x_0 / v109;
				v111 = v279;
				Str_E9C38_smalltit[v278x].alt_4 = 32 * mapHeightmap_11B4E0[v279] - posZ;
				//Used for Reflection Wave Index.
				tickIdx = (uint16_t)D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dword_0x012_2BE0_11248 << 6;
				sinIdx = (tickIdx + (HIBYTE(v279) << 7)) & 0x7FF;
				sinIdx2 = (tickIdx + ((uint8_t)v279 << 7)) & 0x7FF;
				projectedVertexBuffer[26] = Maths::sin_DB750[sinIdx] >> 8;
				v113 = projectedVertexBuffer[26] * (Maths::sin_DB750[sinIdx2] >> 8);
				projectedVertexBuffer[26] = mapHeightmap_11B4E0[v111];
				Str_E9C38_smalltit[v278x].inverse_alt_8 = -(projectedVertexBuffer[26] * ((v113 >> 4) + 0x8000) >> 10) - posZ;
				if (!(mapAngle_13B4E0[v111] & 8) || (Str_E9C38_smalltit[v278x].alt_4 -= v113 >> 10, projectedVertexBuffer[31] >= 14464))
				{
					v113 = 0;
				}
				v116 = (projectedVertexBuffer[31] << 8) + 8 * v113;
				if (v110 <= str_F2C20ar.dword0x13)
					goto LABEL_133;
				if (v110 < str_F2C20ar.dword0x16)
				{
					v116 = v116 * (signed __int64)(str_F2C20ar.dword0x16 - v110) / str_F2C20ar.dword0x12;
				LABEL_133:
					Str_E9C38_smalltit[v278x].pnt5_32 = v116;
					goto LABEL_134;
				}
				Str_E9C38_smalltit[v278x].pnt5_32 = 0;
			LABEL_134:
				v117x = v278x;
				Str_E9C38_smalltit[v278x].pnt2_20 = str_F2C20ar.dword0x22 + str_F2C20ar.dword0x18 * Str_E9C38_smalltit[v278x].alt_4 / v109;
				Str_E9C38_smalltit[v278x].pnt4_28 = str_F2C20ar.dword0x22 + str_F2C20ar.dword0x18 * Str_E9C38_smalltit[v117x].inverse_alt_8 / v109;
				LOBYTE(v118) = v277[2] + v279;
				HIBYTE(v118) = v277[3] + HIBYTE(v279);
				v119 = mapTerrainType_10B4E0[v118];
				Str_E9C38_smalltit[v278x].textIndex_41 = v119;
				if (Maths::x_BYTE_D41D8[164 + v119])
					Str_E9C38_smalltit[v278x].triangleFeatures_38 |= 0x80u;
				if (D41A0_0.m_GameSettings.str_0x2196.flat_0x2199)
					Str_E9C38_smalltit[v278x].triangleFeatures_38 |= 0x1000u;
				v120x = v278x;
				Str_E9C38_smalltit[v278x].textAtyp_43 = Maths::x_BYTE_D41D8[Str_E9C38_smalltit[v278x].textIndex_41];
				Str_E9C38_smalltit[v120x].textUV_42 = projectedVertexBuffer[32] + (((signed int)(uint8_t)mapAngle_13B4E0[v118] >> 2) & 0x1C);
				LOBYTE(v118) = v277[4] + v118;
				HIBYTE(v118) += v277[5];
				Str_E9C38_smalltit[v278x].haveBillboard_36 = mapEntityIndex_15B4E0[v118];
			LABEL_140:
				v121 = v277;
				Str_E9C38_smalltit[v278x].triangleFeatures_38 |= ((x_BYTE)v279 + HIBYTE(v279)) & 1;
				LOBYTE(v279) = v121[8] + v279;
				HIBYTE(v279) += v277[9];
				v278x += 1;
			}
			v122 = v277[6] + v279;
			HIBYTE(v279) += v277[7];
			LOBYTE(v279) = v122;
		}
	}
	v283 = 21;//21eb44 nothing changed
LABEL_259:
	if (v283)
	{
		v285 = 40;
		while (1)
		{
			if (!v285)
			{
				LOBYTE(v279) = v277[6] + v279;
				HIBYTE(v279) += v277[7];
				v283--;
				goto LABEL_259;
			}
			v197 = ((uint8_t)mapShading_12B4E0[v279] << 8) + 128;
			v198 = Str_E9C38_smalltit[v278x].y_12;
			v199 = v198 * v198 + Str_E9C38_smalltit[v278x].x_0 * Str_E9C38_smalltit[v278x].x_0;
			Str_E9C38_smalltit[v278x].haveBillboard_36 = 0;
			if (v198 > -256 && v199 < str_F2C20ar.dword0x15_tileRenderCutOffDistance)
				break;
			Str_E9C38_smalltit[v278x].triangleFeatures_38 |= 2u;
		LABEL_256:
			v206x = v278x;
			v207 = v277;
			Str_E9C38_smalltit[v278x].triangleFeatures_38 |= ((x_BYTE)v279 + HIBYTE(v279)) & 1;
			LOBYTE(v279) = v207[8] + v279;
			HIBYTE(v279) += v277[9];
			v285--;
			v278x = v206x + 1;
		}
		if (v198 < 128)
			v198 = 128;
		v200 = v279;
		Str_E9C38_smalltit[v278x].pnt1_16 = str_F2C20ar.dword0x18 * Str_E9C38_smalltit[v278x].x_0 / v198;
		Str_E9C38_smalltit[v278x].alt_4 = 32 * mapHeightmap_11B4E0[v200] - posZ;
		v201 = (uint16_t)D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].dword_0x012_2BE0_11248 << 6;
		projectedVertexBuffer[26] = Maths::sin_DB750[(v201 + (HIBYTE(v279) << 7)) & 0x7FF] >> 8;
		v202 = projectedVertexBuffer[26] * (Maths::sin_DB750[(((uint8_t)v279 << 7) + v201) & 0x7FF] >> 8);
		if (!(mapAngle_13B4E0[v200] & 8) || (Str_E9C38_smalltit[v278x].alt_4 -= v202 >> 10, v197 >= 14464))
			v202 = 0;
		v203 = (v197 << 8) + 8 * v202;
		if (v199 > str_F2C20ar.dword0x13)
		{
			if (v199 >= str_F2C20ar.dword0x16)
			{
				Str_E9C38_smalltit[v278x].pnt5_32 = 0;
			LABEL_254:
				Str_E9C38_smalltit[v278x].pnt2_20 = str_F2C20ar.dword0x22 + str_F2C20ar.dword0x18 * Str_E9C38_smalltit[v278x].alt_4 / v198;
				LOBYTE(v204) = v277[2] + v279;
				HIBYTE(v204) = v277[3] + HIBYTE(v279);
				v205x = v278x;
				Str_E9C38_smalltit[v278x].textIndex_41 = mapTerrainType_10B4E0[v204];
				Str_E9C38_smalltit[v205x].textAtyp_43 = Maths::x_BYTE_D41D8[Str_E9C38_smalltit[v205x].textIndex_41];
				Str_E9C38_smalltit[v205x].textUV_42 = projectedVertexBuffer[32] + (((signed int)(uint8_t)mapAngle_13B4E0[v204] >> 2) & 0x1C);
				LOBYTE(v204) = v277[4] + v204;
				HIBYTE(v204) += v277[5];
				Str_E9C38_smalltit[v278x].haveBillboard_36 = mapEntityIndex_15B4E0[v204];
				goto LABEL_256;
			}
			v203 = v203 * (signed __int64)(str_F2C20ar.dword0x16 - v199) / str_F2C20ar.dword0x12;
		}
		Str_E9C38_smalltit[v278x].pnt5_32 = v203;
		goto LABEL_254;
	}//21edb7 nothing changed
	v208 = roll & 0x7FF;//21edb7

	//Geometry tiles Distance 0 = near player
	v209 = 840;
	str_F2C20ar.sin_0x0d = Maths::sin_DB750[v208];
	str_F2C20ar.cos_0x11 = Maths::sin_DB750[512 + v208];
	v56x = 0;
	while (v209)
	{
		//Rotation and Translation X
		pnt1_16 = CalculateRotationTranslationX(str_F2C20ar.cos_0x11, Str_E9C38_smalltit[v56x].pnt1_16, str_F2C20ar.sin_0x0d, Str_E9C38_smalltit[v56x].pnt2_20);

		//Rotation and Translation Y
		pnt2_20 = CalculateRotationTranslationY(Str_E9C38_smalltit[v56x].pnt1_16, str_F2C20ar.sin_0x0d, str_F2C20ar.cos_0x11, Str_E9C38_smalltit[v56x].pnt2_20);

		Str_E9C38_smalltit[v56x].pnt2_20 = pnt2_20;
		Str_E9C38_smalltit[v56x].pnt1_16 = pnt1_16;

		if (Str_E9C38_smalltit[v56x].pnt1_16 >= 0)
		{
			if ((signed int)(uint16_t)viewPort.Width_DE564 <= Str_E9C38_smalltit[v56x].pnt1_16)
				Str_E9C38_smalltit[v56x].triangleFeatures_38 |= 0x10u;
		}
		else
		{
			Str_E9C38_smalltit[v56x].triangleFeatures_38 |= 8u;
		}
		v216 = Str_E9C38_smalltit[v56x].pnt2_20;
		if (v216 >= 0)
		{
			if ((uint16_t)viewPort.Height_DE568 <= v216)
				Str_E9C38_smalltit[v56x].triangleFeatures_38 |= 0x40u;
		}
		else
		{
			Str_E9C38_smalltit[v56x].triangleFeatures_38 |= 0x20u;
		}
		v56x++;
		v209--;
	}
	//adress 3de7d
	//Draw Terrain with no reflection
	SubDrawTerrainAndParticles(projectedVertexBuffer, pitch);
}

int32_t GameRenderHD::CalculateRotationTranslationX(int64_t cos_0x11, int64_t pnt1, int64_t sin_0x0d, int64_t pnt2)
{
	int64_t rotation = ((cos_0x11 * pnt1 - sin_0x0d * pnt2) >> 16);
	return rotation + str_F2C20ar.dword0x24;
}

int32_t GameRenderHD::CalculateRotationTranslationY(int64_t pnt1, int64_t sin_0x0d, int64_t cos_0x11, int64_t pnt2)
{
	int64_t rotation = ((pnt1 * sin_0x0d + cos_0x11 * pnt2) >> 16);
	return str_F2C20ar.dword0x10 - rotation;
}

void GameRenderHD::SubDrawCaveTerrainAndParticles(std::vector<int>& projectedVertexBuffer, int pitch)
{
	int v57x = 800;
	char v58; // ah
	int jx;
	char v60; // dl
	char v62; // ch
	char v63; // ah
	char v64; // dl
	char v65; // dh
	char v66; // ch
	char v67; // dl
	char v71; // dl
	char v73; // ch
	char v74; // ah
	char v75; // dl
	char v76; // dh
	char v77; // ch
	char v78; // dl
	char v79; // dh
	int v82x;
	int v83x;
	char v84; // dl
	char v85; // cl
	char v87; // al
	char v88; // dl
	char v89; // dh
	char v92; // cl
	char v93; // dl
	char v97; // dl
	char v99; // ah
	char v100; // dl
	char v101; // dh
	char v102; // ch
	char v105; // dl
	char v106; // dh
	char v281 = 20; // [esp+94h] [ebp+32h]
	char v293; // [esp+C4h] [ebp+62h]

	do
	{
		v58 = 39;
		//Draw Left Side of Cave
		for (jx = v57x; ; jx++)
		{
			v293 = v58;
			if (!v58)
				break;
			projectedVertexBuffer[18] = Str_E9C38_smalltit[jx].pnt3_24;
			projectedVertexBuffer[19] = Str_E9C38_smalltit[jx].pnt4_28;
			projectedVertexBuffer[22] = Str_E9C38_smalltit[jx].pnt5_32;
			v60 = Str_E9C38_smalltit[jx].triangleFeatures_38 & 0xff;

			if (Str_E9C38_smalltit[jx + 1].triangleFeatures_38 & 4)
				break;
			projectedVertexBuffer[12] = Str_E9C38_smalltit[jx + 1].pnt3_24;
			projectedVertexBuffer[13] = Str_E9C38_smalltit[jx + 1].pnt4_28;
			projectedVertexBuffer[16] = Str_E9C38_smalltit[jx + 1].pnt5_32;
			v62 = Str_E9C38_smalltit[jx + 1].triangleFeatures_38 & 0xff;

			projectedVertexBuffer[6] = Str_E9C38_smalltit[jx - 39].pnt3_24;
			projectedVertexBuffer[7] = Str_E9C38_smalltit[jx - 39].pnt4_28;
			projectedVertexBuffer[10] = Str_E9C38_smalltit[jx - 39].pnt5_32;
			v63 = Str_E9C38_smalltit[jx - 39].triangleFeatures_38 & 0xff;
			v64 = v63 | v62 | v60;
			v65 = v63 & v62 & v60;

			projectedVertexBuffer[0] = Str_E9C38_smalltit[jx - 40].pnt3_24;
			projectedVertexBuffer[1] = Str_E9C38_smalltit[jx - 40].pnt4_28;
			projectedVertexBuffer[4] = Str_E9C38_smalltit[jx - 40].pnt5_32;
			v66 = Str_E9C38_smalltit[jx - 40].triangleFeatures_38 & 0xff;
			v67 = v66 | v64;

			if ((v66 & v65 & 0x80u) == 0)
			{
				if (Str_E9C38_smalltit[jx].triangleFeatures_38 & 0x1000)
				{
					x_BYTE_E126D = 7;
					x_BYTE_E126C = (projectedVertexBuffer[10] + projectedVertexBuffer[16] + projectedVertexBuffer[22] + projectedVertexBuffer[4]) >> 18;
				}
				else
				{
					x_BYTE_E126D = 5;
				}
				if (!(v67 & 2))
				{
					DrawInverseSquareInProjectionSpace(&projectedVertexBuffer[0], jx, x_DWORD_DDF50_texture_adresses.at(1));
				}
			}
			projectedVertexBuffer[18] = Str_E9C38_smalltit[jx].pnt1_16;
			projectedVertexBuffer[19] = Str_E9C38_smalltit[jx].pnt2_20;
			projectedVertexBuffer[22] = Str_E9C38_smalltit[jx].pnt5_32;
			v71 = Str_E9C38_smalltit[jx].triangleFeatures_38 & 0xff;

			if (Str_E9C38_smalltit[jx + 1].triangleFeatures_38 & 4)
				break;
			projectedVertexBuffer[12] = Str_E9C38_smalltit[jx + 1].pnt1_16;
			projectedVertexBuffer[13] = Str_E9C38_smalltit[jx + 1].pnt2_20;
			projectedVertexBuffer[16] = Str_E9C38_smalltit[jx + 1].pnt5_32;
			v73 = Str_E9C38_smalltit[jx + 1].triangleFeatures_38 & 0xff;

			projectedVertexBuffer[6] = Str_E9C38_smalltit[jx - 39].pnt1_16;
			projectedVertexBuffer[7] = Str_E9C38_smalltit[jx - 39].pnt2_20;
			projectedVertexBuffer[10] = Str_E9C38_smalltit[jx - 39].pnt5_32;
			v74 = Str_E9C38_smalltit[jx - 39].triangleFeatures_38 & 0xff;
			v75 = v74 | v73 | v71;
			v76 = v74 & v73 & v71;

			projectedVertexBuffer[0] = Str_E9C38_smalltit[jx - 40].pnt1_16;
			projectedVertexBuffer[1] = Str_E9C38_smalltit[jx - 40].pnt2_20;
			projectedVertexBuffer[4] = Str_E9C38_smalltit[jx - 40].pnt5_32;
			v77 = Str_E9C38_smalltit[jx - 40].triangleFeatures_38 & 0xff;
			v78 = v77 | v75;
			v79 = v77 & v76;

			if (v79 >= 0)
			{
				if (Str_E9C38_smalltit[jx].triangleFeatures_38 & 0x1000)
				{
					x_BYTE_E126D = 7;
					x_BYTE_E126C = (projectedVertexBuffer[10] + projectedVertexBuffer[16] + projectedVertexBuffer[22] + projectedVertexBuffer[4]) >> 18;
				}
				else
				{
					x_BYTE_E126D = 5;
				}
				if (!(v78 & 2) && !(v79 & 0x78))
				{
					DrawSquareInProjectionSpace(projectedVertexBuffer, jx);
				}
				if (Str_E9C38_smalltit[jx].haveBillboard_36)
					DrawSprites_3E360(jx, str_DWORD_F66F0x, x_BYTE_E88E0x, x_DWORD_F5730, x_DWORD_EA3E4, str_unk_1804B0ar, viewPort, pitch);
			}
			v58 = v293 - 1;
		}
		//Draw Right Side of Cave
		if (v293)
		{
			v82x = jx;
			v83x = v57x + 38;
			do
			{
				projectedVertexBuffer[18] = Str_E9C38_smalltit[v83x].pnt3_24;
				projectedVertexBuffer[19] = Str_E9C38_smalltit[v83x].pnt4_28;
				projectedVertexBuffer[22] = Str_E9C38_smalltit[v83x].pnt5_32;
				v84 = Str_E9C38_smalltit[v83x].triangleFeatures_38 & 0xff;

				projectedVertexBuffer[12] = Str_E9C38_smalltit[v83x + 1].pnt3_24;
				projectedVertexBuffer[13] = Str_E9C38_smalltit[v83x + 1].pnt4_28;
				projectedVertexBuffer[16] = Str_E9C38_smalltit[v83x + 1].pnt5_32;
				v85 = Str_E9C38_smalltit[v83x + 1].triangleFeatures_38 & 0xff;

				projectedVertexBuffer[6] = Str_E9C38_smalltit[v83x - 39].pnt3_24;
				projectedVertexBuffer[7] = Str_E9C38_smalltit[v83x - 39].pnt4_28;
				projectedVertexBuffer[10] = Str_E9C38_smalltit[v83x - 39].pnt5_32;
				v87 = Str_E9C38_smalltit[v83x - 39].triangleFeatures_38 & 0xff;
				v88 = v87 | v85 | v84;
				v89 = v87 & v85 & v84;

				projectedVertexBuffer[0] = Str_E9C38_smalltit[v83x - 40].pnt3_24;
				projectedVertexBuffer[1] = Str_E9C38_smalltit[v83x - 40].pnt4_28;
				projectedVertexBuffer[4] = Str_E9C38_smalltit[v83x - 40].pnt5_32;
				v92 = Str_E9C38_smalltit[v83x - 40].triangleFeatures_38 & 0xff;
				v93 = v92 | v88;
				if ((v92 & v89 & 0x80u) == 0)
				{
					if (Str_E9C38_smalltit[v83x].triangleFeatures_38 & 0x1000)
					{
						x_BYTE_E126D = 7;
						x_BYTE_E126C = (projectedVertexBuffer[10] + projectedVertexBuffer[16] + projectedVertexBuffer[22] + projectedVertexBuffer[4]) >> 18;
					}
					else
					{
						x_BYTE_E126D = 5;
					}
					if (!(v93 & 2))
					{
						DrawInverseSquareInProjectionSpace(&projectedVertexBuffer[0], v83x, x_DWORD_DDF50_texture_adresses.at(1));
					}
				}
				projectedVertexBuffer[18] = Str_E9C38_smalltit[v83x].pnt1_16;
				projectedVertexBuffer[19] = Str_E9C38_smalltit[v83x].pnt2_20;
				projectedVertexBuffer[22] = Str_E9C38_smalltit[v83x].pnt5_32;
				v97 = Str_E9C38_smalltit[v83x].triangleFeatures_38 & 0xff;

				projectedVertexBuffer[12] = Str_E9C38_smalltit[v83x + 1].pnt1_16;
				projectedVertexBuffer[13] = Str_E9C38_smalltit[v83x + 1].pnt2_20;
				projectedVertexBuffer[16] = Str_E9C38_smalltit[v83x + 1].pnt5_32;
				v99 = Str_E9C38_smalltit[v83x + 1].triangleFeatures_38 & 0xff;
				v100 = v99 | v97;
				v101 = v99 & v97;

				projectedVertexBuffer[6] = Str_E9C38_smalltit[v83x - 39].pnt1_16;
				projectedVertexBuffer[7] = Str_E9C38_smalltit[v83x - 39].pnt2_20;
				projectedVertexBuffer[10] = Str_E9C38_smalltit[v83x - 39].pnt5_32;
				v102 = Str_E9C38_smalltit[v83x - 39].triangleFeatures_38 & 0xff;

				projectedVertexBuffer[0] = Str_E9C38_smalltit[v83x - 40].pnt1_16;
				projectedVertexBuffer[1] = Str_E9C38_smalltit[v83x - 40].pnt2_20;
				projectedVertexBuffer[4] = Str_E9C38_smalltit[v83x - 40].pnt5_32;
				v105 = (Str_E9C38_smalltit[v83x - 40].triangleFeatures_38 & 0xff) | v102 | v100;
				v106 = (Str_E9C38_smalltit[v83x - 40].triangleFeatures_38 & 0xff) & v102 & v101;

				if (v106 >= 0)
				{
					if (Str_E9C38_smalltit[v83x].triangleFeatures_38 & 0x1000)
					{
						x_BYTE_E126D = 7;
						x_BYTE_E126C = (projectedVertexBuffer[10] + projectedVertexBuffer[16] + projectedVertexBuffer[22] + projectedVertexBuffer[4]) >> 18;
					}
					else
					{
						x_BYTE_E126D = 5;
					}
					if (!(v105 & 2) && !(v106 & 0x78))
					{
						DrawSquareInProjectionSpace(projectedVertexBuffer, v83x);
					}
					if (Str_E9C38_smalltit[v83x].haveBillboard_36)
						DrawSprites_3E360(v83x, str_DWORD_F66F0x, x_BYTE_E88E0x, x_DWORD_F5730, x_DWORD_EA3E4, str_unk_1804B0ar, viewPort, pitch);
				}
				v83x--;
			} while (v83x >= v82x);
		}
		v57x -= 40;
		v281--;
	} while (v281);
}

void GameRenderHD::SubDrawInverseTerrainAndParticles(std::vector<int>& projectedVertexBuffer, int pitch)
{
	int v25z;
	int v133x = 800;
	int v134x;
	int v135; // eax
	char v136; // dl
	char v137; // ch
	char v138; // dl
	int v139; // eax
	int v140x;
	//int v141; // eax
	char v142; // ch
	int v143x;
	char v144; // dl
	int v147x;
	char v148; // dl
	char v149; // dl
	int v150; // eax
	int v151x;
	int v152; // eax
	char v153; // cl
	int v154; // eax
	int v155x;
	char v156; // dl
	int m; // [esp+B0h] [ebp+4Eh]
	int n; // [esp+B8h] [ebp+56h]

	Logger->trace("Start Drawing Reflection");

	for (m = 20; m; --m)
	{
		//Draw Left Side of Reflection
		Logger->trace("Start Drawing Left Side of Reflection");
		v134x = v133x;
		for (n = 39; n; --n)
		{
			//ProjectionPolygon 4: X
			projectedVertexBuffer[18] = Str_E9C38_smalltit[v134x].pnt3_24;
			//ProjectionPolygon 4: Y
			projectedVertexBuffer[19] = Str_E9C38_smalltit[v134x].pnt4_28;

			v135 = Str_E9C38_smalltit[v134x].pnt5_32;
			v134x++;
			projectedVertexBuffer[22] = v135;
			v136 = Str_E9C38_smalltit[v134x - 1].triangleFeatures_38;
			if (Str_E9C38_smalltit[v134x].triangleFeatures_38 & 4)
				break;

			//ProjectionPolygon 3: X
			projectedVertexBuffer[12] = Str_E9C38_smalltit[v134x].pnt3_24;
			//ProjectionPolygon 3: Y
			projectedVertexBuffer[13] = Str_E9C38_smalltit[v134x].pnt4_28;

			projectedVertexBuffer[16] = Str_E9C38_smalltit[v134x].pnt5_32;
			v137 = Str_E9C38_smalltit[v134x].triangleFeatures_38;

			//ProjectionPolygon 2: X
			projectedVertexBuffer[6] = Str_E9C38_smalltit[v134x - 40].pnt3_24;
			//ProjectionPolygon 2: Y
			projectedVertexBuffer[7] = Str_E9C38_smalltit[v134x - 40].pnt4_28;

			projectedVertexBuffer[10] = Str_E9C38_smalltit[v134x - 40].pnt5_32;
			v138 = Str_E9C38_smalltit[v134x - 40].triangleFeatures_38 | v137 | v136;

			//ProjectionPolygon 1: X
			projectedVertexBuffer[0] = Str_E9C38_smalltit[v134x - 41].pnt3_24;
			v139 = Str_E9C38_smalltit[v134x - 41].pnt4_28;
			v140x = v134x - 40;
			v140x--;
			//ProjectionPolygon 1: Y
			projectedVertexBuffer[1] = v139;

			projectedVertexBuffer[4] = Str_E9C38_smalltit[v140x].pnt5_32;
			v142 = Str_E9C38_smalltit[v140x].triangleFeatures_38;
			v143x = v140x + 40;
			v144 = v142 | v138;
			if (Str_E9C38_smalltit[v143x].textIndex_41)
			{
				if (Str_E9C38_smalltit[v143x].triangleFeatures_38 & 0x1000)
				{
					x_BYTE_E126D = 7;
					x_BYTE_E126C = (projectedVertexBuffer[10] + projectedVertexBuffer[16] + projectedVertexBuffer[22] + projectedVertexBuffer[4]) >> 18;
				}
				else
				{
					x_BYTE_E126D = 5;
				}
				if (!(v144 & 2))
				{
					DrawInverseSquareInProjectionSpace(&projectedVertexBuffer[0], v143x);
				}
			}
			if (Str_E9C38_smalltit[v143x].haveBillboard_36)
				sub_3FD60(v143x, x_BYTE_E88E0x, x_DWORD_EA3E4, str_unk_1804B0ar, str_DWORD_F66F0x, x_DWORD_F5730, viewPort, pitch);
			v134x = v143x + 1;
		}
		//Draw Right Side of Reflection
		Logger->trace("Start Drawing Right Side of Reflection");
		if (n)
		{
			v25z = v134x - 1;
			v147x = v133x + 38;
			do
			{
				//ProjectionPolygon 4: X
				projectedVertexBuffer[18] = Str_E9C38_smalltit[v147x].pnt3_24;
				//ProjectionPolygon 4: Y
				projectedVertexBuffer[19] = Str_E9C38_smalltit[v147x].pnt4_28;

				projectedVertexBuffer[22] = Str_E9C38_smalltit[v147x].pnt5_32;
				v148 = Str_E9C38_smalltit[v147x].triangleFeatures_38;

				//ProjectionPolygon 3: X
				projectedVertexBuffer[12] = Str_E9C38_smalltit[v147x + 1].pnt3_24;
				//ProjectionPolygon 3: Y
				projectedVertexBuffer[13] = Str_E9C38_smalltit[v147x + 1].pnt4_28;

				projectedVertexBuffer[16] = Str_E9C38_smalltit[v147x + 1].pnt5_32;
				v149 = Str_E9C38_smalltit[v147x + 1].triangleFeatures_38 | v148;

				//ProjectionPolygon 2: X
				projectedVertexBuffer[6] = Str_E9C38_smalltit[v147x - 39].pnt3_24;
				v150 = Str_E9C38_smalltit[v147x - 39].pnt4_28;
				v151x = v147x + 1;
				//ProjectionPolygon 2: Y
				projectedVertexBuffer[7] = v150;
				v152 = Str_E9C38_smalltit[v151x - 40].pnt5_32;

				v151x -= 40;
				projectedVertexBuffer[10] = v152;
				v153 = Str_E9C38_smalltit[v151x].triangleFeatures_38;

				//ProjectionPolygon 1: X
				projectedVertexBuffer[0] = Str_E9C38_smalltit[v151x - 1].pnt3_24;
				v154 = Str_E9C38_smalltit[v151x - 1].pnt4_28;
				v151x--;
				//ProjectionPolygon 1: Y
				projectedVertexBuffer[1] = v154;

				projectedVertexBuffer[4] = Str_E9C38_smalltit[v151x].pnt5_32;
				LOBYTE(v154) = Str_E9C38_smalltit[v151x].triangleFeatures_38;
				v155x = v151x + 40;
				v156 = v154 | v153 | v149;
				if (Str_E9C38_smalltit[v155x].textIndex_41)
				{
					if (Str_E9C38_smalltit[v155x].triangleFeatures_38 & 0x1000)
					{
						x_BYTE_E126D = 7;
						x_BYTE_E126C = (projectedVertexBuffer[10] + projectedVertexBuffer[16] + projectedVertexBuffer[22] + projectedVertexBuffer[4]) >> 18;
					}
					else
					{
						x_BYTE_E126D = 5;
					}
					if (!(v156 & 2))
					{
						DrawInverseSquareInProjectionSpace(&projectedVertexBuffer[0], v155x);
					}
				}
				if (Str_E9C38_smalltit[v155x].haveBillboard_36)
					sub_3FD60(v155x, x_BYTE_E88E0x, x_DWORD_EA3E4, str_unk_1804B0ar, str_DWORD_F66F0x, x_DWORD_F5730, viewPort, pitch);
				v147x = v155x - 1;
			} while (v147x >= v25z);
		}
		v133x -= 40;
	}
}

void GameRenderHD::SubDrawTerrainAndParticles(std::vector<int>& projectedVertexBuffer, int pitch)
{
	int v160 = 800;

	int v161;
	int v162; // eax
	char v163; // dl
	char v164; // dh
	char v165; // ah
	char v166; // dl
	char v167; // dh
	int v168; // eax
	int v169x;
	char v170; // ch
	int v171; // eax
	int v172x;
	char v173; // dl
	char v174; // dh
	int v177x;
	int v178x;
	char v179; // dl
	char v180; // ch
	char v181; // dh
	char v182; // ah
	char v183; // dl
	char v184; // dh
	int v185; // eax
	int v186x;
	int v187; // eax
	int v188; // eax
	char v189; // ch
	int v190x;
	char v191; // dl
	char v192; // dh

	char v282 = 20;
	
	char ii;
	do
	{
		v161 = v160;
		//Draw Left Side of Terrain
		for (ii = 39; ii; --ii)
		{
			projectedVertexBuffer[18] = Str_E9C38_smalltit[v161].pnt1_16;
			projectedVertexBuffer[19] = Str_E9C38_smalltit[v161].pnt2_20;
			v162 = Str_E9C38_smalltit[v161].pnt5_32;
			v161++;
			projectedVertexBuffer[22] = v162;
			v163 = Str_E9C38_smalltit[v161 - 1].triangleFeatures_38;
			v164 = Str_E9C38_smalltit[v161 - 1].triangleFeatures_38;
			if (Str_E9C38_smalltit[v161].triangleFeatures_38 & 4)
				break;
			projectedVertexBuffer[12] = Str_E9C38_smalltit[v161].pnt1_16;
			projectedVertexBuffer[13] = Str_E9C38_smalltit[v161].pnt2_20;
			projectedVertexBuffer[16] = Str_E9C38_smalltit[v161].pnt5_32;
			v165 = Str_E9C38_smalltit[v161].triangleFeatures_38;
			v166 = v165 | v163;
			v167 = v165 & v164;
			projectedVertexBuffer[6] = Str_E9C38_smalltit[v161 - 40].pnt1_16;
			projectedVertexBuffer[7] = Str_E9C38_smalltit[v161 - 40].pnt2_20;
			v168 = Str_E9C38_smalltit[v161 - 40].pnt5_32;
			v169x = v161 - 40;
			projectedVertexBuffer[10] = v168;
			v170 = Str_E9C38_smalltit[v169x].triangleFeatures_38;
			projectedVertexBuffer[0] = Str_E9C38_smalltit[v169x - 1].pnt1_16;
			v171 = Str_E9C38_smalltit[v169x - 1].pnt2_20;
			v169x--;
			projectedVertexBuffer[1] = v171;
			projectedVertexBuffer[4] = Str_E9C38_smalltit[v169x].pnt5_32;
			BYTE1(v171) = Str_E9C38_smalltit[v169x].triangleFeatures_38;
			v172x = v169x + 40;
			v173 = BYTE1(v171) | v170 | v166;
			v174 = BYTE1(v171) & v170 & v167;
			if ((int8_t)(Str_E9C38_smalltit[v172x].triangleFeatures_38 & 0xff) >= 0)
			{
				if (Str_E9C38_smalltit[v172x].triangleFeatures_38 & 0x1000)
				{
					x_BYTE_E126D = 7;
					x_BYTE_E126C = ((signed int)projectedVertexBuffer[10] + projectedVertexBuffer[16] + projectedVertexBuffer[22] + projectedVertexBuffer[4]) >> 18;
				}
				else
				{
					x_BYTE_E126D = 5;
				}
				if (!(v173 & 2) && !(v174 & 0x78))
				{
					DrawSquareInProjectionSpace(projectedVertexBuffer, v172x);
				}
			}
			else
			{
				// reflections
				x_BYTE_E126D = 26;
				if (!(v173 & 2) && !(v174 & 0x78))
				{
					DrawSquareInProjectionSpace(projectedVertexBuffer, v172x);
				}
			}
			if (Str_E9C38_smalltit[v172x].haveBillboard_36)
				DrawSprites_3E360(v172x, str_DWORD_F66F0x, x_BYTE_E88E0x, x_DWORD_F5730, x_DWORD_EA3E4, str_unk_1804B0ar, viewPort, pitch);
			v161 = v172x + 1;
		}
		//Draw Right Side of Terrain
		if (ii)
		{
			v177x = v161 - 1;
			v178x = v160 + 38;
			do
			{
				projectedVertexBuffer[18] = Str_E9C38_smalltit[v178x].pnt1_16;
				projectedVertexBuffer[19] = Str_E9C38_smalltit[v178x].pnt2_20;
				projectedVertexBuffer[22] = Str_E9C38_smalltit[v178x].pnt5_32;
				v179 = Str_E9C38_smalltit[v178x].triangleFeatures_38;
				projectedVertexBuffer[12] = Str_E9C38_smalltit[v178x + 1].pnt1_16;
				projectedVertexBuffer[13] = Str_E9C38_smalltit[v178x + 1].pnt2_20;
				projectedVertexBuffer[16] = Str_E9C38_smalltit[v178x + 1].pnt5_32;
				v180 = Str_E9C38_smalltit[v178x + 1].triangleFeatures_38;
				projectedVertexBuffer[6] = Str_E9C38_smalltit[v178x - 39].pnt1_16;
				projectedVertexBuffer[7] = Str_E9C38_smalltit[v178x - 39].pnt2_20;
				v181 = v179;
				projectedVertexBuffer[10] = Str_E9C38_smalltit[v178x - 39].pnt5_32;
				v182 = Str_E9C38_smalltit[v178x - 39].triangleFeatures_38;
				v183 = v182 | v180 | v179;
				v184 = v182 & v180 & v181;
				v185 = Str_E9C38_smalltit[v178x - 40].pnt1_16;
				v186x = v178x + 1;
				projectedVertexBuffer[0] = v185;
				v187 = Str_E9C38_smalltit[v186x - 41].pnt2_20;
				v186x -= 40;
				projectedVertexBuffer[1] = v187;
				v188 = Str_E9C38_smalltit[v186x - 1].pnt5_32;
				v186x--;
				projectedVertexBuffer[4] = v188;
				v189 = Str_E9C38_smalltit[v186x].triangleFeatures_38;
				v190x = v186x + 40;
				v191 = v189 | v183;
				v192 = v189 & v184;
				if ((int8_t)(Str_E9C38_smalltit[v190x].triangleFeatures_38 & 0xff) >= 0)
				{
					if (Str_E9C38_smalltit[v190x].triangleFeatures_38 & 0x1000)
					{
						x_BYTE_E126D = 7;
						x_BYTE_E126C = ((signed int)projectedVertexBuffer[10] + projectedVertexBuffer[16] + projectedVertexBuffer[22] + projectedVertexBuffer[4]) >> 18;
					}
					else
					{
						x_BYTE_E126D = 5;
					}
					if (!(v191 & 2) && !(v192 & 0x78))
					{
						DrawSquareInProjectionSpace(projectedVertexBuffer, v190x);
					}
				}
				else
				{
					x_BYTE_E126D = 26;
					if (!(v191 & 2) && !(v192 & 0x78))
					{
						DrawSquareInProjectionSpace(projectedVertexBuffer, v190x);
					}
				}
				if (Str_E9C38_smalltit[v190x].haveBillboard_36)
					DrawSprites_3E360(v190x, str_DWORD_F66F0x, x_BYTE_E88E0x, x_DWORD_F5730, x_DWORD_EA3E4, str_unk_1804B0ar, viewPort, pitch);
				v178x = v190x - 1;
			} while (v178x >= v177x);
		}
		v160 -= 40;
		v282--;
	} while (v282);
}

uint16_t GameRenderHD::sub_3FD60(int a2x, uint8_t x_BYTE_E88E0x[], type_event_0x6E8E* x_DWORD_EA3E4[], type_str_unk_1804B0ar str_unk_1804B0ar, type_particle_str** str_DWORD_F66F0x[], int32_t x_DWORD_F5730[], ViewPort viewPort, uint16_t screenWidth)
{
	uint16_t result; // ax
	type_event_0x6E8E* v3x; // eax
	int v4; // edx
	int v5; // eax
	int v6; // ecx
	int v7; // esi
	int v8; // edx
	type_WORD_D951C* v9x; // esi
	int v10; // ecx
	int v11; // ST0C_4
	char v12; // al
	int v16; // ebx
	type_particle_str** v17x; // edi
	int v18; // eax
	int v19; // ebx
	int v20; // edx
	int v21; // eax
	int v22; // eax
	int v23; // eax
	int v24; // eax
	int v25; // eax
	int v26; // ebx
	int v27; // eax
	int v28; // eax
	int v29; // ebx
	int v30; // eax
	int v31; // eax
	int v32; // ebx
	int v33; // eax
	int v34; // eax
	int v35; // eax
	int v36; // eax
	int v38; // eax
	uint8_t v39; // al
	int v40; // [esp+0h] [ebp-Ch]
	type_event_0x6E8E* v41x; // [esp+4h] [ebp-8h]
	int v42; // [esp+8h] [ebp-4h]

	//fix
	v41x = 0;
	type_particle_str* a1x = 0;
	//fix

	result = Str_E9C38_smalltit[a2x].haveBillboard_36;
	do
	{
		if (result < 0x3E8u)
		{
			v3x = x_DWORD_EA3E4[result];
			v41x = v3x;
			if (!(v3x->struct_byte_0xc_12_15.byte[0] & 0x21))
			{
				v4 = (int16_t)(v3x->axis_0x4C_76.x - x_WORD_F2CC4);
				v5 = (int16_t)(x_WORD_F2CC2 - v3x->axis_0x4C_76.y);
				v42 = -v3x->axis_0x4C_76.z - str_F2C20ar.dword0x20;
				v6 = (v4 * str_F2C20ar.dword0x0f - v5 * str_F2C20ar.dword0x17) >> 16;
				v40 = (str_F2C20ar.dword0x17 * v4 + str_F2C20ar.dword0x0f * v5) >> 16;
				v7 = (str_F2C20ar.dword0x17 * v4 + str_F2C20ar.dword0x0f * v5) >> 16;
				v8 = v40 * v40 + v6 * v6;
				if (v7 > 64 && v8 < str_F2C20ar.dword0x15_tileRenderCutOffDistance)
				{
					if (v8 <= str_F2C20ar.dword0x13)
					{
						str_F2C20ar.dword0x00 = 0x2000;
					}
					else if (v8 < str_F2C20ar.dword0x16)
					{
						str_F2C20ar.dword0x00 = 32 * (str_F2C20ar.dword0x16 - (v40 * v40 + v6 * v6)) / str_F2C20ar.dword0x12 << 8;
					}
					else
					{
						str_F2C20ar.dword0x00 = 0;
					}
					v9x = &str_WORD_D951C[v41x->word_0x5A_90];
					v10 = v6 * str_F2C20ar.dword0x18 / v40;
					v11 = str_F2C20ar.dword0x18 * v42 / v40 + str_F2C20ar.dword0x22;
					str_F2C20ar.dword0x04_screenY = (((int64_t)v10 * str_F2C20ar.cos_0x11 - str_F2C20ar.sin_0x0d * (int64_t)v11) >> 16) + str_F2C20ar.dword0x24;
					str_F2C20ar.dword0x03_screenX = str_F2C20ar.dword0x10 - ((str_F2C20ar.sin_0x0d * (int64_t)v10 + (int64_t)v11 * str_F2C20ar.cos_0x11) >> 16);
					v12 = v9x->byte_12;
					x_BYTE_F2CC6 = 0;
					switch (v12)
					{
					case 0:
						if (str_DWORD_F66F0x[v9x->word_0])
						{
							//v14 = v9x->word_0;
							//v15 = 4 * v9x->word_0;
							goto LABEL_16;
						}
						if (MainInitTmaps_71520(v9x->word_0))
						{
							//v14 = v9x->word_0;
							//v15 = 4 * v14;
						LABEL_16:
							x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v9x->word_0].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
							a1x = *str_DWORD_F66F0x[v9x->word_0];
							goto LABEL_47;
						}
						break;
					case 1:
						if (!str_DWORD_F66F0x[v9x->word_0] && !MainInitTmaps_71520(v9x->word_0))
							break;
						x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v9x->word_0].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
						a1x = *str_DWORD_F66F0x[v9x->word_0];
						goto LABEL_47;
					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
					case 7:
					case 8:
					case 9:
					case 10:
					case 11:
					case 12:
					case 13:
					case 14:
					case 15:
					case 16:
						goto LABEL_26;
					case 17:
						v26 = (((v41x->word_0x1C_28 - (uint16_t)yaw_F2CC0) >> 3) & 0xF0) >> 4;
						if (v26 < 8)
						{
							if (str_DWORD_F66F0x[v26 + v9x->word_0])
							{
								x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v26 + v9x->word_0].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
							}
							else
							{
								if (!MainInitTmaps_71520(v26 + v9x->word_0))
									break;
								x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v26 + v9x->word_0].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
							}
							a1x = *str_DWORD_F66F0x[v26 + v9x->word_0];
							goto LABEL_47;
						}
						if (str_DWORD_F66F0x[v9x->word_0 + 15 - v26])
						{
							x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v9x->word_0 + 15 - v26].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
						}
						else
						{
							if (!MainInitTmaps_71520(v9x->word_0 + 15 - v26))
								break;
							x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v9x->word_0 + 15 - v26].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
						}
						a1x = *str_DWORD_F66F0x[v9x->word_0 + 15 - v26];
						str_F2C20ar.dword0x08_width = a1x->width;
						str_F2C20ar.dword0x06_height = a1x->height;
						v28 = (signed __int64)(str_F2C20ar.dword0x18 * v9x->rotSpeed_8) / v40;
						str_F2C20ar.dword0x0c_realHeight = v28;
						str_F2C20ar.dword0x09_realWidth = v28 * str_F2C20ar.dword0x08_width / str_F2C20ar.dword0x06_height;
						v23 = -str_F2C20ar.dword0x08_width;
						goto LABEL_69;
					case 18:
						v29 = (((v41x->word_0x1C_28 - (uint16_t)yaw_F2CC0) >> 3) & 0xF0) >> 4;
						v30 = v29 + v9x->word_0;
						if (str_DWORD_F66F0x[v30])
						{
							x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v30].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
						}
						else
						{
							if (!MainInitTmaps_71520(v29 + v9x->word_0))
								break;
							x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v29 + v9x->word_0].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
						}
						a1x = *str_DWORD_F66F0x[v9x->word_0 + v29];
						str_F2C20ar.dword0x08_width = a1x->width;
						str_F2C20ar.dword0x06_height = a1x->height;
						v31 = (int64_t)(str_F2C20ar.dword0x18 * v9x->rotSpeed_8) / v40;
						str_F2C20ar.dword0x0c_realHeight = v31;
						str_F2C20ar.dword0x09_realWidth = v31 * str_F2C20ar.dword0x08_width / str_F2C20ar.dword0x06_height;
						v23 = str_F2C20ar.dword0x08_width;
						goto LABEL_69;
					case 19:
						v19 = (((v41x->word_0x1C_28 - (uint16_t)yaw_F2CC0) >> 3) & 0xF0) >> 4;
						if (v19 >= 8)
						{
							v24 = v9x->word_0 + (uint8_t)x_BYTE_D4750[12 + v19];
							if (!str_DWORD_F66F0x[v24])
							{
								if (!MainInitTmaps_71520(v9x->word_0 + (uint8_t)x_BYTE_D4750[12 + v19]))
									break;
								v24 = v9x->word_0 + (uint8_t)x_BYTE_D4750[12 + v19];
							}
							x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v24].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
							a1x = *str_DWORD_F66F0x[v9x->word_0 + (uint8_t)x_BYTE_D4750[12 + v19]];
							str_F2C20ar.dword0x08_width = a1x->width;
							str_F2C20ar.dword0x06_height = a1x->height;
							v25 = (int64_t)(str_F2C20ar.dword0x18 * v9x->rotSpeed_8) / v40;
							str_F2C20ar.dword0x0c_realHeight = v25;
							str_F2C20ar.dword0x09_realWidth = v25 * str_F2C20ar.dword0x08_width / str_F2C20ar.dword0x06_height;
							v23 = -str_F2C20ar.dword0x08_width;
						}
						else
						{
							v20 = (uint8_t)x_BYTE_D4750[12 + v19];
							v21 = v20 + v9x->word_0;
							if (str_DWORD_F66F0x[v21])
							{
								x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v21].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
							}
							else
							{
								if (!MainInitTmaps_71520(v9x->word_0 + (uint8_t)v20))
									break;
								x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v9x->word_0 + (uint8_t)x_BYTE_D4750[12 + v19]].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
							}
							a1x = *str_DWORD_F66F0x[v9x->word_0 + (uint8_t)x_BYTE_D4750[12 + v19]];
							str_F2C20ar.dword0x08_width = a1x->width;
							str_F2C20ar.dword0x06_height = a1x->height;
							v22 = (int64_t)(str_F2C20ar.dword0x18 * v9x->rotSpeed_8) / v40;
							str_F2C20ar.dword0x0c_realHeight = v22;
							str_F2C20ar.dword0x09_realWidth = v22 * str_F2C20ar.dword0x08_width / str_F2C20ar.dword0x06_height;
							v23 = str_F2C20ar.dword0x08_width;
						}
						goto LABEL_69;
					case 20:
						v32 = (((v41x->word_0x1C_28 - (uint16_t)yaw_F2CC0) >> 3) & 0xF0) >> 4;
						if (v32 >= 8)
						{
							v35 = v9x->word_0 + (uint8_t)x_BYTE_D4750[28 + v32];
							if (str_DWORD_F66F0x[v35])
							{
								x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v35].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
							}
							else
							{
								if (!MainInitTmaps_71520(v9x->word_0 + (uint8_t)x_BYTE_D4750[28 + v32]))
									break;
								x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v9x->word_0 + (uint8_t)x_BYTE_D4750[28 + v32]].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
							}
							a1x = *str_DWORD_F66F0x[(uint8_t)x_BYTE_D4750[28 + v32] + v9x->word_0];
							str_F2C20ar.dword0x08_width = a1x->width;
							str_F2C20ar.dword0x06_height = a1x->height;
							v36 = (int64_t)(str_F2C20ar.dword0x18 * v9x->rotSpeed_8) / v40;
							str_F2C20ar.dword0x0c_realHeight = v36;
							str_F2C20ar.dword0x09_realWidth = v36 * str_F2C20ar.dword0x08_width / str_F2C20ar.dword0x06_height;
							v23 = -str_F2C20ar.dword0x08_width;
						}
						else
						{
							v33 = v9x->word_0 + (uint8_t)x_BYTE_D4750[28 + v32];
							if (str_DWORD_F66F0x[v33])
							{
								x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v33].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
							}
							else
							{
								if (!MainInitTmaps_71520(v9x->word_0 + (uint8_t)x_BYTE_D4750[28 + v32]))
									break;
								x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v9x->word_0 + (uint8_t)x_BYTE_D4750[28 + v32]].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
							}
							a1x = *str_DWORD_F66F0x[v9x->word_0 + (uint8_t)x_BYTE_D4750[28 + v32]];
							str_F2C20ar.dword0x08_width = a1x->width;
							str_F2C20ar.dword0x06_height = a1x->height;
							v34 = (int64_t)(str_F2C20ar.dword0x18 * v9x->rotSpeed_8) / v40;
							str_F2C20ar.dword0x0c_realHeight = v34;
							str_F2C20ar.dword0x09_realWidth = v34 * str_F2C20ar.dword0x08_width / str_F2C20ar.dword0x06_height;
							v23 = str_F2C20ar.dword0x08_width;
						}
						goto LABEL_69;
					case 21:
						v16 = v9x->word_0;
						if (str_DWORD_F66F0x[v16])
						{
							x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v16].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
						}
						else
						{
							if (!MainInitTmaps_71520(v16))
								break;
							x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v9x->word_0].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
						}
						v17x = str_DWORD_F66F0x[v9x->word_0];
						x_BYTE_F2CC6 = 1;
						a1x = *v17x;
						goto LABEL_47;
					case 22:
					case 23:
					case 24:
					case 25:
					case 26:
					case 27:
					case 28:
					case 29:
					case 30:
					case 31:
					case 32:
					case 33:
					case 34:
					case 35:
					case 36:
						x_BYTE_F2CC6 = 1;
					LABEL_26:
						v18 = v41x->byte_0x5C_92 + v9x->word_0;
						if (str_DWORD_F66F0x[v18])
						{
							x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v18].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
						}
						else
						{
							if (!MainInitTmaps_71520(v9x->word_0 + v41x->byte_0x5C_92))
								break;
							x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v9x->word_0 + v41x->byte_0x5C_92].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
						}
						a1x = *str_DWORD_F66F0x[v9x->word_0 + v41x->byte_0x5C_92];
					LABEL_47:
						str_F2C20ar.dword0x08_width = a1x->width;
						str_F2C20ar.dword0x06_height = a1x->height;
						v27 = (int64_t)(str_F2C20ar.dword0x18 * v9x->rotSpeed_8) / v40;
						str_F2C20ar.dword0x0c_realHeight = v27;
						str_F2C20ar.dword0x09_realWidth = v27 * str_F2C20ar.dword0x08_width / str_F2C20ar.dword0x06_height;
						v23 = str_F2C20ar.dword0x08_width;
					LABEL_69:
						str_F2C20ar.dword0x05 = v23;
					LABEL_70:
						str_F2C20ar.dword0x02_data = a1x->textureBuffer;
						v38 = str_F2C20ar.dword0x00;
						a1x->word_0 |= 8;
						if (v38 == 0x2000)
							v39 = x_BYTE_D4750[v9x->byte_10];
						else
							v39 = x_BYTE_D4750[6 + v9x->byte_10];
						str_F2C20ar.dword0x01_rotIdx = v39;
						str_F2C20ar.dword0x09_realWidth++;
						str_F2C20ar.dword0x0c_realHeight++;
						DrawSprite_41BD3(2u);
						break;
					default:
						goto LABEL_70;
					}
				}
			}
		}
		result = v41x->oldMapEntity_0x16_22;
	} while (result);
	return result;
}

void GameRenderHD::sub_88740(type_event_0x6E8E* a1x, int16_t posX, int16_t posY)
{
	int v3; // esi
	type_event_0x6E8E* v4x; // edx
	uint8_t v5; // al
	uint8_t v6; // al
	uint8_t v7; // al
	uint8_t v8; // al
	//char v9; // cl
	signed int v10; // eax
	uint8_t v11; // al
	//unsigned int v12; // edi
	signed int v13; // eax
	//int v14; // esi
	//char v15; // dl
	//char v16; // dh
	//char v17; // bl
	char v18; // [esp+0h] [ebp-4h]

	v3 = 0;
	if (str_unk_1804B0ar.PopupStatusByte_0x9e & 1)
		return;
	v4x = x_DWORD_EA3E4[D41A0_0.array_0x2BDE[D41A0_0.LevelIndex_0xc].word_0x00a_2BE4_11240];
	v5 = a1x->class_0x3F_63;
	if (v5 < 5u)
	{
		if (v5 >= 2u)
		{
			if (v5 <= 2u)
			{
				v6 = a1x->model_0x40_64;
				if (v6 >= 1u)
				{
					if (v6 <= 1u)
					{
						v3 = 27;
					}
					else if (v6 == 2)
					{
						v3 = 22;
					}
				}
			}
			else if (v5 == 3)
			{
				v11 = a1x->model_0x40_64;
				if (v11 < 2u)
				{
					if (v11 == 1)
						v3 = 28;
				}
				else if (v11 <= 2u)
				{
					v3 = (a1x->id_0x1A_26 != v4x->id_0x1A_26) + 24;
				}
				else if (v11 == 3)
				{
					if (a1x->id_0x1A_26 == v4x->id_0x1A_26)
						v3 = 23;
					else
						v3 = 26;
				}
			}
		}
		goto LABEL_48;
	}
	if (v5 > 5u)
	{
		if (v5 >= 0xAu)
		{
			if (v5 <= 0xAu)
			{
				if (a1x->model_0x40_64 == 39 && a1x->word_0x94_148 != v4x->id_0x1A_26)
					v3 = 18;
			}
			else if (v5 == 15 && !(a1x->struct_byte_0xc_12_15.byte[0] & 1))
			{
				v3 = 20;
			}
		}
		goto LABEL_48;
	}
	if (a1x->id_0x1A_26 != v4x->id_0x1A_26)
	{
		v7 = a1x->model_0x40_64;
		if (v7 < 0xCu)
			goto LABEL_30;
		if (v7 > 0xEu)
		{
			if (v7 == 22)
			{
				if (((int8_t)a1x->state_0x45_69 != -76) && a1x->word_0x94_148 != v4x->id_0x1A_26)
					v3 = 18;
				goto LABEL_48;
			}
		LABEL_30:
			v8 = a1x->state_0x45_69;
			if (v8 < 0xE8u || v8 > 0xEAu)
			{
				v10 = 1;
				if ((a1x->StageVar2_0x49_73 == 14 || a1x->StageVar2_0x49_73 == 13) && a1x->parentId_0x28_40 == v4x->id_0x1A_26)
					v10 = 0;
				if (v10)
					v3 = 19;
			}
			goto LABEL_48;
		}
	}
LABEL_48:
	if (v3)
	{
		if (x_WORD_180660_VGA_type_resolution & 1)
		{
			posX *= 2;
			posY *= 2;
		}
		if (str_E2A74[v3].axis_2[0] & 2)
		{
			if (a1x == str_E2A74[v3].dword_12)
			{
				if (!(str_unk_1804B0ar.byte_0x9f & 0x1))
				{
					str_E2A74[v3].axis_2[3] = posX;
					str_E2A74[v3].axis_2[0] |= 8;
					str_E2A74[v3].axis_2[4] = posY;
					str_unk_1804B0ar.byte_0x9f |= 2;
				}
			}
		}
		else
		{
			v18 = 0;
			v13 = Maths::sub_58490_radix_3d_2(&v4x->axis_0x4C_76, &a1x->axis_0x4C_76);
			if (!str_E2A74[v3].dword_12 || v13 < str_E2A74[v3].dword_20 && v13 > 1024)
				v18 = 1;
			if (v18)
			{
				str_E2A74[v3].dword_20 = v13;
				str_E2A74[v3].dword_12 = a1x;
				str_E2A74[v3].axis_2[0] |= 8;
			}
		}
	}
}

void GameRenderHD::SetBillboards_3B560(int16_t roll)
{
	int v1; // edx
	int v2idx;
	signed int* v3; // esi
	int32_t v4; // eax
	int16_t v5; // bx
	signed int v6; // ecx
	int v7; // edx
	uint8_t v8; // cf
	int v9; // eax
	int v10; // esi
	uint32_t v11; // eax
	int v12idx;
	signed int* v13; // esi
	int v14; // eax
	signed int v15; // ecx
	int v16; // edx
	int v17idx;
	signed int* v18; // esi
	int v19; // eax
	int16_t v20; // bx
	signed int v21; // ecx
	int v22; // edx
	int v23idx;
	signed int* v24; // esi
	int v25; // eax
	int16_t v26; // bx
	signed int v27; // ecx
	int v28; // edx
	type_unk_F0E20x* resultx;
	signed int* v31; // esi
	int v32idx;
	int v33; // eax
	signed int v34; // ecx
	int v35; // edx
	int v36idx;
	signed int* v37; // esi
	int v38; // eax
	int16_t v39; // bx
	signed int v40; // ecx
	int v41; // edx
	uint8_t* v42x; // edx
	int v43idx;
	signed int* v44; // esi
	int v45; // eax
	int16_t v46; // bx
	signed int v47; // ecx
	int v48; // edx
	int v49; // edx
	int v50; // esi
	signed int* v52; // esi
	int v53idx;
	int v54; // eax
	signed int v55; // ecx
	int v56; // edx
	int v57idx;
	signed int* v58; // esi
	int v59; // eax
	int16_t v60; // bx
	signed int v61; // ecx
	int v62; // edx
	int v63idx;
	signed int* v64; // esi
	int v65; // eax
	int16_t v66; // bx
	signed int v67; // ecx
	int v68; // edx
	int v69; // esi
	int v70; // eax
	int v71idx;
	signed int* v72; // esi
	int v73; // eax
	signed int v74; // ecx
	int v75; // edx
	int v76idx;
	signed int* v77; // esi
	int v78; // eax
	int16_t v79; // bx
	signed int v80; // ecx
	int v81; // edx
	int v82; // edx
	int v83; // ecx
	int v84; // [esp+0h] [ebp-10h]
	int v85; // [esp+0h] [ebp-10h]
	int v86; // [esp+0h] [ebp-10h]
	int v87; // [esp+0h] [ebp-10h]
	int v88; // [esp+4h] [ebp-Ch]
	int v89; // [esp+4h] [ebp-Ch]
	int v90; // [esp+4h] [ebp-Ch]
	int v91; // [esp+4h] [ebp-Ch]
	int v92; // [esp+8h] [ebp-8h]
	int v93; // [esp+8h] [ebp-8h]
	int v94; // [esp+8h] [ebp-8h]
	int v95; // [esp+8h] [ebp-8h]
	int v96; // [esp+8h] [ebp-8h]
	int v97; // [esp+8h] [ebp-8h]
	int v98; // [esp+8h] [ebp-8h]
	int v99; // [esp+8h] [ebp-8h]
	int v100; // [esp+8h] [ebp-8h]
	int16_t v101; // [esp+Ch] [ebp-4h]
	int16_t v102; // [esp+Ch] [ebp-4h]
	int16_t v103; // [esp+Ch] [ebp-4h]
	int16_t v104; // [esp+Ch] [ebp-4h]
	int16_t v105; // [esp+Ch] [ebp-4h]
	int16_t v106; // [esp+Ch] [ebp-4h]
	int16_t v107; // [esp+Ch] [ebp-4h]
	int16_t v108; // [esp+Ch] [ebp-4h]

	v1 = roll & 0x7FF;
	str_F2C20ar.dword0x1e = v1 >> 8;
	switch (v1 >> 8)
	{
	case 0:
		str_F2C20ar.dword0x27 = Maths::sin_DB750[v1];
		str_F2C20ar.dword0x1b = Maths::sin_DB750[512 + v1];

		v88 = iScreenWidth_DE560;
		v92 = (uint16_t)viewPort.Width_DE564;
		str_F2C20ar.dword0x1f = (str_F2C20ar.dword0x27 << 8) / (str_F2C20ar.dword0x1b >> 8);
		v101 = (str_F2C20ar.dword0x27 << 8) / (str_F2C20ar.dword0x1b >> 8);
		v2idx = 0;
		v3 = (signed int*)&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_3];
		v4 = 0;
		v5 = 0;
		v6 = 1;
		v7 = 0;
		do
		{
			m_str_F0E20x[v2idx].dword_1 = v4;
			m_str_F0E20x[v2idx].dword_2 = v7;
			v8 = __CFADD__(v101, v5);
			v5 += v101;
			if (v8)
			{
				v4 += v88;
				++v7;
				*v3 = v6;
				++v3;
			}
			v2idx++;
			++v4;
			++v6;
			--v92;
		} while (v92);
		str_F2C20ar.dword0x1d = v7;
		str_F2C20ar.dword0x21 = -v7;
		str_F2C20ar.width0x25 = (uint16_t)viewPort.Height_DE568;
		str_F2C20ar.height0x26 = (uint16_t)viewPort.Width_DE564;
		str_F2C20ar.dword0x1c = (uint16_t)viewPort.Height_DE568 + (uint16_t)viewPort.Width_DE564;
		str_F2C20ar.pbyte0x1a = (4 * (v7 - 1) + &m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_3]);
		v9 = iScreenWidth_DE560;
		str_F2C20ar.Height_0x19 = viewPort.Height_DE568 - v7;
		goto LABEL_66;
	case 1:
		v10 = Maths::sin_DB750[v1];
		v11 = Maths::sin_DB750[512 + v1];

		str_F2C20ar.dword0x27 = v10;
		str_F2C20ar.dword0x1b = (int)v11;
		v84 = iScreenWidth_DE560;
		v93 = (uint16_t)viewPort.Height_DE568;
		if (v1 == 256)
		{
			str_F2C20ar.dword0x1f = 0x10000;
			v12idx = 0;
			v13 = (signed int*)&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_3];
			v14 = 0;
			v15 = 1;
			v16 = 0;
			do
			{
				m_str_F0E20x[v12idx].dword_1 = v14;
				m_str_F0E20x[v12idx].dword_2 = v16++;
				*v13 = v15;
				++v13;
				v12idx++;
				v14 += v84 + 1;
				++v15;
				--v93;
			} while (v93);
			str_F2C20ar.dword0x1d = (uint16_t)viewPort.Height_DE568;
			str_F2C20ar.dword0x21 = -(uint16_t)viewPort.Height_DE568;
		}
		else
		{
			str_F2C20ar.dword0x1f = (str_F2C20ar.dword0x1b << 8) / (v10 >> 8);
			v102 = (str_F2C20ar.dword0x1b << 8) / (v10 >> 8);
			v17idx = 0;
			v18 = (signed int*)&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_3];
			v19 = 0;
			v20 = 0;
			v21 = 1;
			v22 = 0;
			do
			{
				m_str_F0E20x[v17idx].dword_1 = v19;
				m_str_F0E20x[v17idx].dword_2 = v22;
				v8 = __CFADD__(v102, v20);
				v20 += v102;
				if (v8)
				{
					v19++;
					v22++;
					*v18 = v21;
					v18++;
				}
				v17idx++;
				v19 += v84;
				v21++;
				v93--;
			} while (v93);
			str_F2C20ar.dword0x1d = v22;
			str_F2C20ar.dword0x21 = -v22;
		}
		str_F2C20ar.width0x25 = (uint16_t)viewPort.Width_DE564;
		str_F2C20ar.height0x26 = (uint16_t)viewPort.Height_DE568;
		str_F2C20ar.dword0x1c = (uint16_t)viewPort.Height_DE568 + (uint16_t)viewPort.Width_DE564;
		str_F2C20ar.pbyte0x1a = (4 * (-1 - str_F2C20ar.dword0x21) + &m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_3]);
		v9 = -1;
		str_F2C20ar.Height_0x19 = (uint16_t)viewPort.Width_DE564 + str_F2C20ar.dword0x21;
		goto LABEL_66;
	case 2:
		str_F2C20ar.dword0x27 = Maths::sin_DB750[v1 - 512];//copy to other
		str_F2C20ar.dword0x1b = Maths::sin_DB750[v1];
		
		v85 = iScreenWidth_DE560;
		v94 = (uint16_t)viewPort.Height_DE568;
		str_F2C20ar.dword0x1f = (str_F2C20ar.dword0x27 << 8) / (str_F2C20ar.dword0x1b >> 8);
		v103 = (str_F2C20ar.dword0x27 << 8) / (str_F2C20ar.dword0x1b >> 8);
		v23idx = 0;
		v24 = (signed int*)&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_3];
		v25 = 0;
		v26 = 0;
		v27 = 1;
		v28 = 0;
		do
		{
			m_str_F0E20x[v23idx].dword_1 = v25;
			m_str_F0E20x[v23idx].dword_2 = v28;
			v8 = __CFADD__(v103, v26);
			v26 += v103;
			if (v8)
			{
				v25--;
				v28++;
				*v24 = v27;
				v24++;
			}
			v23idx++;
			v25 += v85;
			v27++;
			v94--;
		} while (v94);
		str_F2C20ar.dword0x1d = v28;
		str_F2C20ar.dword0x21 = -v28;
		str_F2C20ar.width0x25 = (uint16_t)viewPort.Width_DE564;
		str_F2C20ar.dword0x1c = (uint16_t)viewPort.Height_DE568 + (uint16_t)viewPort.Width_DE564;
		str_F2C20ar.height0x26 = (uint16_t)viewPort.Height_DE568;
		str_F2C20ar.dword0x23_stride = -1;
		str_F2C20ar.Height_0x19 = (uint16_t)viewPort.Width_DE564 - v28;
		str_F2C20ar.pbyte0x1a = (4 * (v28 - 1) + &m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_3]);
		v95 = (uint16_t)viewPort.Height_DE568 - 1;
		resultx = m_str_F0E20x;
		if (v95 < 0)
			return;
		goto LABEL_68;
	case 3:
		str_F2C20ar.dword0x27 = Maths::sin_DB750[v1 - 512];//copy to other
		str_F2C20ar.dword0x1b = Maths::sin_DB750[v1];
		
		v89 = iScreenWidth_DE560;
		v96 = (uint16_t)viewPort.Width_DE564;
		if (v1 == 768)
		{
			str_F2C20ar.dword0x1f = 0x10000;
			v31 = (signed int*)&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_3];
			v32idx = 0;
			v33 = 0;
			v34 = 1;
			v35 = 0;
			do
			{
				m_str_F0E20x[v32idx].dword_1 = v33;
				m_str_F0E20x[v32idx].dword_2 = v35++;
				*v31 = v34;
				v31++;
				v32idx++;
				v33 = v89 + v33 - 1;
				v34++;
				v96--;
			} while (v96);
			str_F2C20ar.dword0x1d = (uint16_t)viewPort.Width_DE564;
			str_F2C20ar.dword0x21 = -(uint16_t)viewPort.Width_DE564;
		}
		else
		{
			str_F2C20ar.dword0x1f = (str_F2C20ar.dword0x1b << 8) / (str_F2C20ar.dword0x27 >> 8);
			v104 = (str_F2C20ar.dword0x1b << 8) / (str_F2C20ar.dword0x27 >> 8);
			v36idx = 0;
			v37 = (signed int*)&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_3];
			v38 = 0;
			v39 = 0;
			v40 = 1;
			v41 = 0;
			do
			{
				m_str_F0E20x[v36idx].dword_1 = v38;
				m_str_F0E20x[v36idx].dword_2 = v41;
				v8 = __CFADD__(v104, v39);
				v39 += v104;
				if (v8)
				{
					v38 += v89;
					v41++;
					*v37 = v40;
					v37++;
				}
				v36idx++;
				v38--;
				v40++;
				v96--;
			} while (v96);
			str_F2C20ar.dword0x1d = v41;
			str_F2C20ar.dword0x21 = -v41;
		}
		str_F2C20ar.width0x25 = (uint16_t)viewPort.Height_DE568;
		str_F2C20ar.height0x26 = (uint16_t)viewPort.Width_DE564;
		str_F2C20ar.dword0x1c = (uint16_t)viewPort.Height_DE568 + (uint16_t)viewPort.Width_DE564;
		v42x = (4 * (-1 - str_F2C20ar.dword0x21) + &m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_3]);
		str_F2C20ar.Height_0x19 = (uint16_t)viewPort.Height_DE568 + str_F2C20ar.dword0x21;
		v9 = -iScreenWidth_DE560;
		goto LABEL_65;
	case 4:
		str_F2C20ar.dword0x27 = Maths::sin_DB750[v1 - 1024];//copy to other
		str_F2C20ar.dword0x1b = Maths::sin_DB750[v1 - 512];//copy to other

		v90 = -iScreenWidth_DE560;
		v97 = (uint16_t)viewPort.Width_DE564;
		str_F2C20ar.dword0x1f = (str_F2C20ar.dword0x27 << 8) / (str_F2C20ar.dword0x1b >> 8);
		v105 = (str_F2C20ar.dword0x27 << 8) / (str_F2C20ar.dword0x1b >> 8);
		v43idx = 0;
		v44 = (signed int*)&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_3];
		v45 = 0;
		v46 = 0;
		v47 = 1;
		v48 = 0;
		do
		{
			m_str_F0E20x[v43idx].dword_1 = v45;
			m_str_F0E20x[v43idx].dword_2 = v48;
			v8 = __CFADD__(v105, v46);
			v46 += v105;
			if (v8)
			{
				v45 += v90;
				v48++;
				*v44 = v47;
				v44++;
			}
			v43idx++;
			v45--;
			v47++;
			v97--;
		} while (v97);
		str_F2C20ar.dword0x1d = v48;
		v49 = -v48;
		str_F2C20ar.dword0x21 = v49;
		v50 = (uint16_t)viewPort.Height_DE568 + v49;
		str_F2C20ar.width0x25 = (uint16_t)viewPort.Height_DE568;
		str_F2C20ar.height0x26 = (uint16_t)viewPort.Width_DE564;
		str_F2C20ar.dword0x1c = (uint16_t)viewPort.Height_DE568 + (uint16_t)viewPort.Width_DE564;
		v42x = (4 * (-1 - v49) + &m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_3]);
		str_F2C20ar.Height_0x19 = v50;
		v9 = -iScreenWidth_DE560;
		goto LABEL_65;
	case 5:
		str_F2C20ar.dword0x1b = Maths::sin_DB750[v1 - 512];//copy to other
		str_F2C20ar.dword0x27 = Maths::sin_DB750[v1 - 1024];//copy to other

		v86 = -iScreenWidth_DE560;
		v98 = (uint16_t)viewPort.Height_DE568;
		if (v1 == 1280)
		{
			str_F2C20ar.dword0x1f = 0x10000;
			v52 = (signed int*)&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_3];
			v53idx = 0;
			v54 = 0;
			v55 = 1;
			v56 = 0;
			do
			{
				m_str_F0E20x[v53idx].dword_1 = v54;
				m_str_F0E20x[v53idx].dword_2 = v56++;
				*v52 = v55;
				++v52;
				v53idx++;
				v54 = v86 + v54 - 1;
				++v55;
				--v98;
			} while (v98);
			str_F2C20ar.dword0x1d = (uint16_t)viewPort.Height_DE568;
			str_F2C20ar.dword0x21 = -(uint16_t)viewPort.Height_DE568;
		}
		else
		{
			str_F2C20ar.dword0x1f = (str_F2C20ar.dword0x1b << 8) / (str_F2C20ar.dword0x27 >> 8);
			v106 = (str_F2C20ar.dword0x1b << 8) / (str_F2C20ar.dword0x27 >> 8);
			v57idx = 0;
			v58 = (signed int*)&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_3];
			v59 = 0;
			v60 = 0;
			v61 = 1;
			v62 = 0;
			do
			{
				m_str_F0E20x[v57idx].dword_1 = v59;
				m_str_F0E20x[v57idx].dword_2 = v62;
				v8 = __CFADD__(v106, v60);
				v60 += v106;
				if (v8)
				{
					v59--;
					v62++;
					*v58 = v61;
					v58++;
				}
				v57idx++;
				v59 += v86;
				v61++;
				v98--;
			} while (v98);
			str_F2C20ar.dword0x1d = v62;
			str_F2C20ar.dword0x21 = -v62;
		}
		str_F2C20ar.width0x25 = (uint16_t)viewPort.Width_DE564;
		str_F2C20ar.height0x26 = (uint16_t)viewPort.Height_DE568;
		str_F2C20ar.dword0x1c = (uint16_t)viewPort.Height_DE568 + (uint16_t)viewPort.Width_DE564;
		str_F2C20ar.dword0x23_stride = 1;
		str_F2C20ar.Height_0x19 = (uint16_t)viewPort.Width_DE564 + str_F2C20ar.dword0x21;
		str_F2C20ar.pbyte0x1a = (4 * (-1 - str_F2C20ar.dword0x21) + &m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_3]);
		v95 = (uint16_t)viewPort.Height_DE568 - 1;
		resultx = m_str_F0E20x;
		if (v95 < 0)
			return;
		goto LABEL_68;
	case 6:
		str_F2C20ar.dword0x27 = Maths::sin_DB750[v1 - 1536];//copy to other
		str_F2C20ar.dword0x1b = Maths::sin_DB750[v1 - 1024];//copy to other

		v87 = -iScreenWidth_DE560;
		v99 = (uint16_t)viewPort.Height_DE568;
		str_F2C20ar.dword0x1f = (str_F2C20ar.dword0x27 << 8) / (str_F2C20ar.dword0x1b >> 8);
		v107 = (str_F2C20ar.dword0x27 << 8) / (str_F2C20ar.dword0x1b >> 8);
		v63idx = 0;
		v64 = (signed int*)&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_3];
		v65 = 0;
		v66 = 0;
		v67 = 1;
		v68 = 0;
		do
		{
			m_str_F0E20x[v63idx].dword_1 = v65;
			m_str_F0E20x[v63idx].dword_2 = v68;
			v8 = __CFADD__(v107, v66);
			v66 += v107;
			if (v8)
			{
				v65++;
				v68++;
				*v64 = v67;
				v64++;
			}
			v63idx++;
			v65 += v87;
			v67++;
			v99--;
		} while (v99);
		str_F2C20ar.dword0x1d = v68;
		str_F2C20ar.dword0x21 = -v68;
		str_F2C20ar.Height_0x19 = (uint16_t)viewPort.Width_DE564 - v68;
		str_F2C20ar.width0x25 = (uint16_t)viewPort.Width_DE564;
		str_F2C20ar.dword0x1c = (uint16_t)viewPort.Height_DE568 + (uint16_t)viewPort.Width_DE564;
		str_F2C20ar.height0x26 = (uint16_t)viewPort.Height_DE568;
		str_F2C20ar.dword0x23_stride = 1;
		str_F2C20ar.pbyte0x1a = (4 * (v68 - 1) + &m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_3]);
		v95 = (uint16_t)viewPort.Height_DE568 - 1;
		resultx = m_str_F0E20x;
		if (v95 < 0)
			return;
		goto LABEL_68;
	case 7:
		v69 = Maths::sin_DB750[v1 - 1536];//copy to other
		v70 = Maths::sin_DB750[v1 - 1024];//copy to other

		str_F2C20ar.dword0x27 = v69;
		str_F2C20ar.dword0x1b = v70;
		v91 = -iScreenWidth_DE560;
		v100 = (uint16_t)viewPort.Width_DE564;
		if (v1 == 1792)
		{
			v71idx = 0;
			v72 = (signed int*)&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_3];
			str_F2C20ar.dword0x1f = 0x10000;
			v73 = 0;
			v74 = 1;
			v75 = 0;
			do
			{
				m_str_F0E20x[v71idx].dword_1 = v73;
				m_str_F0E20x[v71idx].dword_2 = v75++;
				v72[0] = v74;
				v72++;
				v71idx++;
				v73 += v91 + 1;
				v74++;
				v100--;
			} while (v100);
			str_F2C20ar.dword0x1d = (uint16_t)viewPort.Width_DE564;
			str_F2C20ar.dword0x21 = -(uint16_t)viewPort.Width_DE564;
		}
		else
		{
			str_F2C20ar.dword0x1f = (str_F2C20ar.dword0x1b << 8) / (v69 >> 8);
			v108 = (str_F2C20ar.dword0x1b << 8) / (v69 >> 8);
			v76idx = 0;
			v77 = (signed int*)&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_3];
			v78 = 0;
			v79 = 0;
			v80 = 1;
			v81 = 0;
			do
			{
				m_str_F0E20x[v76idx].dword_1 = v78;
				m_str_F0E20x[v76idx].dword_2 = v81;
				v8 = __CFADD__(v108, v79);
				v79 += v108;
				if (v8)
				{
					v78 += v91;
					v81++;
					*v77 = v80;
					v77++;
				}
				v76idx++;
				v78++;
				v80++;
				v100--;
			} while (v100);
			str_F2C20ar.dword0x1d = v81;
			str_F2C20ar.dword0x21 = -v81;
		}
		str_F2C20ar.width0x25 = (uint16_t)viewPort.Height_DE568;
		str_F2C20ar.height0x26 = (uint16_t)viewPort.Width_DE564;
		str_F2C20ar.dword0x1c = (uint16_t)viewPort.Height_DE568 + (uint16_t)viewPort.Width_DE564;
		str_F2C20ar.Height_0x19 = (uint16_t)viewPort.Height_DE568 + str_F2C20ar.dword0x21;
		v42x = (4 * (-1 - str_F2C20ar.dword0x21) + &m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_3]);
		v9 = iScreenWidth_DE560;
	LABEL_65:
		str_F2C20ar.pbyte0x1a = v42x;
	LABEL_66:
		str_F2C20ar.dword0x23_stride = v9;
		break;
	default:
		break;
	}
	v95 = str_F2C20ar.height0x26 - 1;
	for (resultx = m_str_F0E20x; v95 >= 0; --v95)
	{
	LABEL_68:
		v82 = resultx->dword_1;
		// FIXME: This is what would have happend in the original memory layout in which 
		//        the array unk_F0A20x is directly located before unk_F0E20x.
		//        But not sure if this is intended. Maybe it becomes clearer when the logic get refactored.
		/*if (resultx == m_str_F0E20x) {
			v83 = *(x_DWORD*)&unk_F0A20x[1016];
		}
		else {
			v83 = *(result - 2);
		}
		result += 3;
		*(result - 3) = v82 - v83;*/
		if (resultx == m_str_F0E20x) {
			v83 = *(x_DWORD*)&unk_F0A20x[1016];
		}
		else {
			v83 = resultx[-1].dword_1;
		}
		resultx->dword_0 = v82 - v83;
		resultx++;
	}
}

void GameRenderHD::DrawSorcererNameAndHealthBar_2CB30(type_event_0x6E8E* a1x, __int16 a2, int a3, __int16 a4)
{
	char* v5; // esi
	int v9x; // eax
	int v9y; // eax
	__int16 v10; // bx
	__int16 v11; // bx
	int v12; // edi
	int v13; // esi
	char v24[32]; // [esp+0h] [ebp-58h]
	int v25; // [esp+20h] [ebp-38h]
	int v26; // [esp+24h] [ebp-34h]
	//int v27; // [esp+28h] [ebp-30h]
	//int v28; // [esp+2Ch] [ebp-2Ch]
	int v29; // [esp+30h] [ebp-28h]
	int v30; // [esp+34h] [ebp-24h]
	int v31; // [esp+38h] [ebp-20h]
	int v32; // [esp+3Ch] [ebp-1Ch]
	uint8_t v33; // [esp+40h] [ebp-18h]
	char v34; // [esp+44h] [ebp-14h]
	char v35; // [esp+48h] [ebp-10h]
	char v36; // [esp+4Ch] [ebp-Ch]
	char v37; // [esp+50h] [ebp-8h]
	uint8_t v38; // [esp+54h] [ebp-4h]
	int v39; // [esp+74h] [ebp+1Ch]
	v31 = viewPort.PreWidth_EA3C4 + viewPort.PosX_EA3D0 - 4;
	v29 = viewPort.PreHeight_EA3C0 + viewPort.PosY_EA3CC - 22;
	v25 = a1x->dword_0xA4_164x->word_0x38_56;
	v5 = D41A0_0.array_0x2BDE[v25].WizardName_0x39f_2BFA_12157;
	strcpy(v24, v5);
	v36 = x_BYTE_E88E0x[3 * GetTrueWizardNumber_61790(v25)];//c
	v35 = m_ptrColorPalette[0];//10 //v19
	v34 = x_BYTE_E88E0x[3 * GetTrueWizardNumber_61790(v25)];	//14 //v18
	v33 = str_D94F0_bldgprmbuffer[static_cast<std::underlying_type<MapType_t>::type>(D41A0_0.terrain_2FECE.MapType)][2];//18 v14
	v38 = str_D94F0_bldgprmbuffer[static_cast<std::underlying_type<MapType_t>::type>(D41A0_0.terrain_2FECE.MapType)][3];//4 v15
	v37 = str_D94F0_bldgprmbuffer[static_cast<std::underlying_type<MapType_t>::type>(D41A0_0.terrain_2FECE.MapType)][0];//?v22
	v10 = (a4 >> 1) + a2;
	if (x_WORD_180660_VGA_type_resolution & 1)
	{
		v10 *= 2;
		a3 *= 2;
	}
	v11 = viewPort.PosX_EA3D0 + v10;
	v12 = viewPort.PosY_EA3CC + a3 - 20;
	v39 = viewPort.PosY_EA3CC + a3 - 20;
	if (v11 >= viewPort.PosX_EA3D0)
	{
		if ((int16_t)v12 >= viewPort.PosY_EA3CC && v11 < v31 && (int16_t)v12 < v29)
		{
			v9x = strlen(v24);
			v13 = 8 * v9x + 4;
			if (v11 + v13 > v31)
			{
				v13 = v31 - v11;
				v9x = ((v31 - v11 - 4) - (my_sign32(v31 - v11 - 4) << 3) + my_sign32(v31 - v11 - 4)) >> 3;
			}
			if (v9x > 0)
			{
				v24[v9x] = 0;
				v32 = v13 + 2;
				v26 = (int16_t)(v13 + 2);
				v30 = v11;
				DrawLine_2BC80(v11, v39, v13 + 2, 18, v37);//8
				//v27 = v33;//30// v16
				DrawLine_2BC80(v30, v39, v26, 2, v33);//18
				//v28 = v38;//2c//v17
				DrawLine_2BC80(v30, v39 + 16, v26, 2, v38);//4
				DrawLine_2BC80(v30, v39, 2, 16, v33);//30,tj.18
				DrawLine_2BC80(v11 + v32 - 2, v39, 2, 18, v38);//2c tj. 4
				DrawText_2BC10(v24, v11 + 4, v39, v34);//14
				DrawLine_2BC80(v11 + 2, v39 + 14, v13 - 2, 2, v35);//10
				if (a1x->maxLife_0x4)
				{
					v9y = a1x->life_0x8 * (v13 - 2) / a1x->maxLife_0x4;
					if (v30 + 2 + v9y > v31 - 2)
						v9y = v31 - 2 - (v30 + 2);
					if (v9y > 0)
						DrawLine_2BC80(v11 + 2, v39 + 14, v9y, 2, v36);
				}
			}
		}
	}
}

void GameRenderHD::StartWorkerThreads(uint8_t numOfThreads, bool assignToSpecificCores)
{
	m_multiThreadRender = true;
	if (m_renderThreads.size() < numOfThreads)
	{
		WaitForRenderFinish();

		for (int i = 0; i < numOfThreads; i++)
		{
			if (assignToSpecificCores)
			{
				StartWorkerThread(i + 1);
			}
			else
			{
				StartWorkerThread();
			}
		}
	}
}

void GameRenderHD::StartWorkerThread()
{
	RenderThread* renderThread = new RenderThread();

	if (renderThread->IsRunning())
	{
		m_renderThreads.push_back(renderThread);
	}
}

void GameRenderHD::StartWorkerThread(int core)
{
	RenderThread* renderThread = new RenderThread(core);

	if (renderThread->IsRunning())
	{
		m_renderThreads.push_back(renderThread);
	}
}

void GameRenderHD::StopWorkerThreads()
{
	if (m_renderThreads.size() > 0)
	{
		for (RenderThread* thread : m_renderThreads) {
			if (thread->IsRunning())
			{
				thread->StopWorkerThread();
				delete thread;
			}
		}
		m_renderThreads.clear();
	}
}

//Coordinates Already transformed into "Screen Space" (x & y, top left 0,0)
void GameRenderHD::DrawSquareInProjectionSpace(std::vector<int>& vertexs, int index)
{
	//Set Texture coordinates for polys
	vertexs[20] = xunk_D4350[Str_E9C38_smalltit[index].textUV_42][0];
	vertexs[21] = xunk_D4350[Str_E9C38_smalltit[index].textUV_42][1];
	vertexs[14] = xunk_D4350[Str_E9C38_smalltit[index].textUV_42][2];
	vertexs[15] = xunk_D4350[Str_E9C38_smalltit[index].textUV_42][3];
	vertexs[8] = xunk_D4350[Str_E9C38_smalltit[index].textUV_42][4];
	vertexs[9] = xunk_D4350[Str_E9C38_smalltit[index].textUV_42][5];
	vertexs[2] = xunk_D4350[Str_E9C38_smalltit[index].textUV_42][6];
	vertexs[3] = xunk_D4350[Str_E9C38_smalltit[index].textUV_42][7];

	//Get Texture
	x_DWORD_DE55C_ActTexture = x_DWORD_DDF50_texture_adresses.at(Str_E9C38_smalltit[index].textIndex_41);

	//Render
	auto vertex0 = ProjectionPolygon(&vertexs[0]);
	auto vertex6 = ProjectionPolygon(&vertexs[6]);
	auto vertex12 = ProjectionPolygon(&vertexs[12]);
	auto vertex18 = ProjectionPolygon(&vertexs[18]);

	uint8_t drawEveryNthLine = m_renderThreads.size() + 1;

	if ((uint8_t)Str_E9C38_smalltit[index].triangleFeatures_38 & 1)
	{
		if (m_renderThreads.size() > 0)
		{
			uint8_t i = 0;

			for (i = 0; i < m_renderThreads.size(); i++)
			{
				m_renderThreads[i]->Run([this, vertex0, vertex6, vertex12, vertex18, i, drawEveryNthLine] {
					this->DrawTriangleInProjectionSpace_B6253(&vertex18, &vertex12, &vertex0, i, drawEveryNthLine);
					this->DrawTriangleInProjectionSpace_B6253(&vertex0, &vertex12, &vertex6, i, drawEveryNthLine);
				});
			}

			this->DrawTriangleInProjectionSpace_B6253(&vertex18, &vertex12, &vertex0, i, drawEveryNthLine);
			this->DrawTriangleInProjectionSpace_B6253(&vertex0, &vertex12, &vertex6, i, drawEveryNthLine);

			WaitForRenderFinish();
		}
		else
		{
			DrawTriangleInProjectionSpace_B6253(&vertex18, &vertex12, &vertex0, 0, 1);
			DrawTriangleInProjectionSpace_B6253(&vertex0, &vertex12, &vertex6, 0, 1);
		}
	}
	else
	{
		if (m_renderThreads.size() > 0)
		{
			uint8_t i = 0;

			for (i = 0; i < m_renderThreads.size(); i++)
			{
				m_renderThreads[i]->Run([this, vertex0, vertex6, vertex12, vertex18, i, drawEveryNthLine] {
					this->DrawTriangleInProjectionSpace_B6253(&vertex18, &vertex12, &vertex6, i, drawEveryNthLine);
					this->DrawTriangleInProjectionSpace_B6253(&vertex18, &vertex6, &vertex0, i, drawEveryNthLine);
				});
			}

			this->DrawTriangleInProjectionSpace_B6253(&vertex18, &vertex12, &vertex6, i, drawEveryNthLine);
			this->DrawTriangleInProjectionSpace_B6253(&vertex18, &vertex6, &vertex0, i, drawEveryNthLine);

			WaitForRenderFinish();
		}
		else
		{
			DrawTriangleInProjectionSpace_B6253(&vertex18, &vertex12, &vertex6, 0, 1);
			DrawTriangleInProjectionSpace_B6253(&vertex18, &vertex6, &vertex0, 0, 1);
		}
	}
}

void GameRenderHD::DrawInverseSquareInProjectionSpace(int* vertexs, int index)
{
	DrawInverseSquareInProjectionSpace(vertexs, index, x_DWORD_DDF50_texture_adresses.at(Str_E9C38_smalltit[index].textIndex_41));
}

void GameRenderHD::DrawInverseSquareInProjectionSpace(int* vertexs, int index, uint8_t* pTexture)
{
	//Set Texture coordinates for polys
	vertexs[20] = xunk_D4350[Str_E9C38_smalltit[index].textUV_42][0];
	vertexs[21] = xunk_D4350[Str_E9C38_smalltit[index].textUV_42][1];
	vertexs[14] = xunk_D4350[Str_E9C38_smalltit[index].textUV_42][2];
	vertexs[15] = xunk_D4350[Str_E9C38_smalltit[index].textUV_42][3];
	vertexs[8] = xunk_D4350[Str_E9C38_smalltit[index].textUV_42][4];
	vertexs[9] = xunk_D4350[Str_E9C38_smalltit[index].textUV_42][5];
	vertexs[2] = xunk_D4350[Str_E9C38_smalltit[index].textUV_42][6];
	vertexs[3] = xunk_D4350[Str_E9C38_smalltit[index].textUV_42][7];
	x_BYTE_E126D = 5;

	//Get Texture
	x_DWORD_DE55C_ActTexture = pTexture;

	//Render
	auto vertex0 = ProjectionPolygon(&vertexs[0]);
	auto vertex6 = ProjectionPolygon(&vertexs[6]);
	auto vertex12 = ProjectionPolygon(&vertexs[12]);
	auto vertex18 = ProjectionPolygon(&vertexs[18]);

	//Logger->trace("--------------------------------------");
	//Logger->trace("{}", vertex0.Print());
	//Logger->trace("{}", vertex6.Print());
	//Logger->trace("{}", vertex12.Print());
	//Logger->trace("{}", vertex18.Print());
	//Logger->trace("--------------------------------------");

	uint8_t drawEveryNthLine = m_renderThreads.size() + 1;

	if (Str_E9C38_smalltit[index].triangleFeatures_38 & 1)
	{
		if (m_renderThreads.size() > 0)
		{
			uint8_t i = 0;

			for (i = 0; i < m_renderThreads.size(); i++)
			{
				m_renderThreads[i]->Run([this, vertex0, vertex6, vertex12, vertex18, i, drawEveryNthLine] {
					this->DrawTriangleInProjectionSpace_B6253(&vertex18, &vertex0, &vertex12, i, drawEveryNthLine);
					this->DrawTriangleInProjectionSpace_B6253(&vertex0, &vertex6, &vertex12, i, drawEveryNthLine);
					});
			}

			this->DrawTriangleInProjectionSpace_B6253(&vertex18, &vertex0, &vertex12, i, drawEveryNthLine);
			this->DrawTriangleInProjectionSpace_B6253(&vertex0, &vertex6, &vertex12, i, drawEveryNthLine);

			WaitForRenderFinish();
		}
		else
		{
			DrawTriangleInProjectionSpace_B6253(&vertex18, &vertex0, &vertex12, 0, 1);
			DrawTriangleInProjectionSpace_B6253(&vertex0, &vertex6, &vertex12, 0, 1);
		}

	}
	else
	{
		if (m_renderThreads.size() > 0)
		{
			uint8_t i = 0;

			for (i = 0; i < m_renderThreads.size(); i++)
			{
				m_renderThreads[i]->Run([this, vertex0, vertex6, vertex12, vertex18, i, drawEveryNthLine] {
					this->DrawTriangleInProjectionSpace_B6253(&vertex18, &vertex6, &vertex12, i, drawEveryNthLine);
					this->DrawTriangleInProjectionSpace_B6253(&vertex18, &vertex0, &vertex6, i, drawEveryNthLine);
				});
			}

			this->DrawTriangleInProjectionSpace_B6253(&vertex18, &vertex6, &vertex12, i, drawEveryNthLine);
			this->DrawTriangleInProjectionSpace_B6253(&vertex18, &vertex0, &vertex6, i, drawEveryNthLine);

			WaitForRenderFinish();
		}
		else
		{
			DrawTriangleInProjectionSpace_B6253(&vertex18, &vertex6, &vertex12, 0, 1);
			DrawTriangleInProjectionSpace_B6253(&vertex18, &vertex0, &vertex6, 0, 1);
		}
	}
}

void GameRenderHD::DrawSprites_3E360(int a2x, type_particle_str** str_DWORD_F66F0x[], uint8_t x_BYTE_E88E0x[], int32_t x_DWORD_F5730[], type_event_0x6E8E* x_DWORD_EA3E4[], type_str_unk_1804B0ar str_unk_1804B0ar, ViewPort viewPort, uint16_t screenWidth)
{
	uint16_t result; // ax
	type_event_0x6E8E* v3x; // eax
	__int16 v4; // cx
	int v5; // ecx
	int v6; // edx
	type_WORD_D951C* v7x; // edi
	int v8; // ecx
	int v9; // ST18_4
	char v10; // al
	int v17; // ebx
	int v18; // ebx
	int v19; // eax
	int v20; // eax
	int v21; // eax
	int v22; // edx
	int v23; // eax
	int v24; // eax
	int v25; // ebx
	uint16_t v27; // ax
	int v28; // eax
	uint16_t v30; // ax
	int v31; // eax
	int v32; // ebx
	int v33; // eax
	int v35; // eax
	int v36; // eax
	int v37; // ebx
	int v38; // edx
	int v39; // eax
	int v40; // eax
	int v41; // eax
	int v42; // eax
	int v43; // ebx
	uint8_t v45; // al
	int v46; // ecx
	int v47; // eax
	int v48; // eax
	int v49; // ecx
	type_D404C* v50x; // ebx
	int v51; // edx
	type_WORD_D951C* v52x; // edi
	int v53; // ecx
	int v54; // ST1C_4
	char v55; // al
	int v59; // ebx
	int v61; // ebx
	int v62; // ebx
	int v63; // eax
	uint16_t v65; // ax
	int v66; // eax
	int v67; // eax
	int v68; // eax
	int v70; // eax
	int v71; // eax
	int v72; // ebx
	int v73; // eax
	int v75; // eax
	int v76; // eax
	int v77; // eax
	int v78; // eax
	int v79; // ebx
	int v80; // eax
	int v81; // eax
	int v82; // ebx
	int v83; // edx
	int v84; // eax
	int v85; // eax
	int v86; // eax
	uint16_t v88; // ax
	int v89; // eax
	type_event_0x6E8E* v90x; // ebx
	__int16 v91; // cx
	uint8_t v92; // al
	char v93; // cl
	int v94; // eax
	int v95; // eax
	int v96; // [esp+0h] [ebp-20h]
	int v97; // [esp+8h] [ebp-18h]
	int v98; // [esp+10h] [ebp-10h]
	int v99; // [esp+18h] [ebp-8h]
	int v100; // [esp+1Ch] [ebp-4h]

	type_particle_str* a1y = NULL;
	//fix

	result = Str_E9C38_smalltit[a2x].haveBillboard_36;
	do
	{
		//adress 21f370

		v3x = x_DWORD_EA3E4[result];
		str_F2C20ar.dword0x14x = v3x;
		if (!(v3x->struct_byte_0xc_12_15.byte[0] & 0x21))
		{
			v4 = v3x->axis_0x4C_76.y;
			v96 = (int16_t)(v3x->axis_0x4C_76.x - x_WORD_F2CC4);
			v97 = (int16_t)(x_WORD_F2CC2 - v4);
			if (shadows_F2CC7)
			{
				if (!Str_E9C38_smalltit[a2x].textAtyp_43 && !(v3x->struct_byte_0xc_12_15.word[1] & 0x808))
				{
					//adress 21f40c
					v98 = sub_B5C60_getTerrainAlt2(v3x->axis_0x4C_76.x, v4) - str_F2C20ar.dword0x20;
					v5 = (str_F2C20ar.dword0x0f * v96 - str_F2C20ar.dword0x17 * v97) >> 16;
					v99 = (str_F2C20ar.dword0x17 * v96 + str_F2C20ar.dword0x0f * v97) >> 16;
					v6 = v99 * v99 + v5 * v5;
					if (v99 > 64 && v6 < str_F2C20ar.dword0x15_tileRenderCutOffDistance)
					{
						if (v6 <= str_F2C20ar.dword0x13)
							str_F2C20ar.dword0x00 = 0x2000;
						else
							str_F2C20ar.dword0x00 = v6 < str_F2C20ar.dword0x16 ? 32 * (str_F2C20ar.dword0x16 - (v99 * v99 + v5 * v5)) / str_F2C20ar.dword0x12 << 8 : 0;
						v7x = &str_WORD_D951C[str_F2C20ar.dword0x14x->word_0x5A_90];
						if (!v7x->byte_10)
						{
							v8 = v5 * str_F2C20ar.dword0x18 / v99;
							v9 = str_F2C20ar.dword0x18 * v98 / v99 + str_F2C20ar.dword0x22;
							str_F2C20ar.dword0x04_screenY = (((int64_t)v8 * str_F2C20ar.cos_0x11 - str_F2C20ar.sin_0x0d * (int64_t)v9) >> 16) + str_F2C20ar.dword0x24;
							str_F2C20ar.dword0x03_screenX = str_F2C20ar.dword0x10 - ((str_F2C20ar.sin_0x0d * (int64_t)v8 + (int64_t)v9 * str_F2C20ar.cos_0x11) >> 16);
							v10 = v7x->byte_12;
							x_BYTE_F2CC6 = 0;
							switch (v10)
							{
							case 0:
								if (str_DWORD_F66F0x[v7x->word_0])
								{
									//v12 = v7x->word_0;
									//v13 = 4 * v7x->word_0;
								}
								else
								{
									if (!MainInitTmaps_71520(v7x->word_0))
										goto LABEL_178;
									//v12 = v7x->word_0;
									//v13 = 4 * v12;
								}
								x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v7x->word_0].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
								a1y = *str_DWORD_F66F0x[v7x->word_0];
								goto LABEL_51;
							case 1:
								if (!str_DWORD_F66F0x[v7x->word_0] && !MainInitTmaps_71520(v7x->word_0))
									goto LABEL_178;
								x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v7x->word_0].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
								a1y = *str_DWORD_F66F0x[v7x->word_0];
								goto LABEL_51;
							case 2:
							case 3:
							case 4:
							case 5:
							case 6:
							case 7:
							case 8:
							case 9:
							case 10:
							case 11:
							case 12:
							case 13:
							case 14:
							case 15:
							case 16:
								goto LABEL_29;
							case 17:
								v25 = (((str_F2C20ar.dword0x14x->word_0x1C_28 - (uint16_t)yaw_F2CC0) >> 3) & 0xF0) >> 4;
								if (v25 < 8)
								{
									if (str_DWORD_F66F0x[v25 + v7x->word_0])
									{
										v27 = str_TMAPS00TAB_BEGIN_BUFFER[v25 + v7x->word_0].word_8;
									}
									else
									{
										if (!MainInitTmaps_71520(v25 + v7x->word_0))
											goto LABEL_178;
										v27 = str_TMAPS00TAB_BEGIN_BUFFER[v25 + v7x->word_0].word_8;
									}
									x_DWORD_F5730[v27] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
									a1y = *str_DWORD_F66F0x[v25 + v7x->word_0];
									goto LABEL_51;
								}
								if (str_DWORD_F66F0x[v7x->word_0 + 15 - v25])
								{
									v30 = str_TMAPS00TAB_BEGIN_BUFFER[v7x->word_0 + 15 - v25].word_8;
								}
								else
								{
									if (!MainInitTmaps_71520(v7x->word_0 + 15 - v25))
										goto LABEL_178;
									v30 = str_TMAPS00TAB_BEGIN_BUFFER[v7x->word_0 + 15 - v25].word_8;
								}
								x_DWORD_F5730[v30] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
								a1y = *str_DWORD_F66F0x[v7x->word_0 + 15 - v25];
								str_F2C20ar.dword0x08_width = a1y->width;
								str_F2C20ar.dword0x06_height = a1y->height;
								v31 = (signed __int64)(str_F2C20ar.dword0x18 * v7x->rotSpeed_8) / v99;
								str_F2C20ar.dword0x0c_realHeight = v31;
								str_F2C20ar.dword0x09_realWidth = v31 * str_F2C20ar.dword0x08_width / str_F2C20ar.dword0x06_height;
								v21 = -str_F2C20ar.dword0x08_width;
								goto LABEL_72;
							case 18:
								v32 = (((str_F2C20ar.dword0x14x->word_0x1C_28 - (uint16_t)yaw_F2CC0) >> 3) & 0xF0) >> 4;
								v33 = v32 + v7x->word_0;
								if (str_DWORD_F66F0x[v33])
								{
									v35 = str_TMAPS00TAB_BEGIN_BUFFER[v33].word_8;
								}
								else
								{
									if (!MainInitTmaps_71520(v32 + v7x->word_0))
										goto LABEL_178;
									v35 = str_TMAPS00TAB_BEGIN_BUFFER[v32 + v7x->word_0].word_8;
								}
								x_DWORD_F5730[v35] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
								a1y = *str_DWORD_F66F0x[v32 + v7x->word_0];
								str_F2C20ar.dword0x08_width = a1y->width;
								str_F2C20ar.dword0x06_height = a1y->height;
								v36 = (signed __int64)(str_F2C20ar.dword0x18 * v7x->rotSpeed_8) / v99;
								str_F2C20ar.dword0x0c_realHeight = v36;
								str_F2C20ar.dword0x09_realWidth = v36 * str_F2C20ar.dword0x08_width / str_F2C20ar.dword0x06_height;
								v21 = str_F2C20ar.dword0x08_width;
								goto LABEL_72;
							case 19:
								v18 = (((str_F2C20ar.dword0x14x->word_0x1C_28 - (uint16_t)yaw_F2CC0) >> 3) & 0xF0) >> 4;
								if (v18 >= 8)
								{
									v22 = (uint8_t)x_BYTE_D4750[12 + v18];
									v23 = v22 + v7x->word_0;
									if (str_DWORD_F66F0x[v23])
									{
										x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v23].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
									}
									else
									{
										if (!MainInitTmaps_71520(v7x->word_0 + (uint8_t)v22))
											goto LABEL_178;
										x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v7x->word_0 + (uint8_t)x_BYTE_D4750[12 + v18]].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
									}
									a1y = *str_DWORD_F66F0x[v7x->word_0 + (uint8_t)x_BYTE_D4750[12 + v18]];
									str_F2C20ar.dword0x08_width = a1y->width;
									str_F2C20ar.dword0x06_height = a1y->height;
									v24 = (signed __int64)(str_F2C20ar.dword0x18 * v7x->rotSpeed_8) / v99;
									str_F2C20ar.dword0x0c_realHeight = v24;
									str_F2C20ar.dword0x09_realWidth = v24 * str_F2C20ar.dword0x08_width / str_F2C20ar.dword0x06_height;
									v21 = -str_F2C20ar.dword0x08_width;
								}
								else
								{
									v19 = (uint8_t)x_BYTE_D4750[12 + v18] + v7x->word_0;
									if (str_DWORD_F66F0x[v19])
									{
										x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v19].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
									}
									else
									{
										if (!MainInitTmaps_71520(v7x->word_0 + (uint8_t)x_BYTE_D4750[12 + v18]))
											goto LABEL_178;
										x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v7x->word_0 + (uint8_t)x_BYTE_D4750[12 + v18]].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
									}
									a1y = *str_DWORD_F66F0x[v7x->word_0 + (uint8_t)x_BYTE_D4750[12 + v18]];
									str_F2C20ar.dword0x08_width = a1y->width;
									str_F2C20ar.dword0x06_height = a1y->height;
									v20 = (signed __int64)(str_F2C20ar.dword0x18 * v7x->rotSpeed_8) / v99;
									str_F2C20ar.dword0x0c_realHeight = v20;
									str_F2C20ar.dword0x09_realWidth = v20 * str_F2C20ar.dword0x08_width / str_F2C20ar.dword0x06_height;
									v21 = str_F2C20ar.dword0x08_width;
								}
								goto LABEL_72;
							case 20:
								v37 = (((str_F2C20ar.dword0x14x->word_0x1C_28 - (uint16_t)yaw_F2CC0) >> 3) & 0xF0) >> 4;
								if (v37 >= 8)
								{
									v41 = v7x->word_0 + (uint8_t)x_BYTE_D4750[28 + v37];
									if (!str_DWORD_F66F0x[v41])
									{
										if (!MainInitTmaps_71520(v7x->word_0 + (uint8_t)x_BYTE_D4750[28 + v37]))
											goto LABEL_178;
										v41 = (uint8_t)x_BYTE_D4750[28 + v37] + v7x->word_0;
									}
									x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v41].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
									a1y = *str_DWORD_F66F0x[v7x->word_0 + (uint8_t)x_BYTE_D4750[28 + v37]];
									str_F2C20ar.dword0x08_width = a1y->width;
									str_F2C20ar.dword0x06_height = a1y->height;
									v42 = (signed __int64)(str_F2C20ar.dword0x18 * v7x->rotSpeed_8) / v99;
									str_F2C20ar.dword0x0c_realHeight = v42;
									str_F2C20ar.dword0x09_realWidth = v42 * str_F2C20ar.dword0x08_width / str_F2C20ar.dword0x06_height;
									v21 = -str_F2C20ar.dword0x08_width;
								}
								else
								{
									v38 = (uint8_t)x_BYTE_D4750[28 + v37];
									v39 = v38 + v7x->word_0;
									if (str_DWORD_F66F0x[v39])
									{
										x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v39].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
									}
									else
									{
										if (!MainInitTmaps_71520(v7x->word_0 + (uint8_t)v38))
											goto LABEL_178;
										x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v7x->word_0 + (uint8_t)x_BYTE_D4750[28 + v37]].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
									}
									a1y = *str_DWORD_F66F0x[v7x->word_0 + (uint8_t)x_BYTE_D4750[28 + v37]];
									str_F2C20ar.dword0x08_width = a1y->width;
									str_F2C20ar.dword0x06_height = a1y->height;
									v40 = (signed __int64)(str_F2C20ar.dword0x18 * v7x->rotSpeed_8) / v99;
									str_F2C20ar.dword0x0c_realHeight = v40;
									str_F2C20ar.dword0x09_realWidth = v40 * str_F2C20ar.dword0x08_width / str_F2C20ar.dword0x06_height;
									v21 = str_F2C20ar.dword0x08_width;
								}
								goto LABEL_72;
							case 21:
								if (str_DWORD_F66F0x[v7x->word_0])
								{
									//v15 = v7x->word_0;
									//v16 = 4 * v7x->word_0;
								}
								else
								{
									if (!MainInitTmaps_71520(v7x->word_0))
										goto LABEL_178;
									//v15 = v7x->word_0;
									//v16 = 4 * v15;
								}
								x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v7x->word_0].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
								a1y = *str_DWORD_F66F0x[v7x->word_0];
								goto LABEL_51;
							case 22:
							case 23:
							case 24:
							case 25:
							case 26:
							case 27:
							case 28:
							case 29:
							case 30:
							case 31:
							case 32:
							case 33:
							case 34:
							case 35:
							case 36:
								x_BYTE_F2CC6 = 1;
							LABEL_29:
								v17 = v7x->word_0 + str_F2C20ar.dword0x14x->byte_0x5C_92;
								if (str_DWORD_F66F0x[v17])
								{
									x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v17].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
								}
								else
								{
									if (!MainInitTmaps_71520(v7x->word_0 + str_F2C20ar.dword0x14x->byte_0x5C_92))
										goto LABEL_178;
									x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v7x->word_0 + str_F2C20ar.dword0x14x->byte_0x5C_92].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
								}
								a1y = *str_DWORD_F66F0x[v7x->word_0 + str_F2C20ar.dword0x14x->byte_0x5C_92];
							LABEL_51:
								str_F2C20ar.dword0x08_width = a1y->width;
								str_F2C20ar.dword0x06_height = a1y->height;
								v28 = (signed __int64)(str_F2C20ar.dword0x18 * v7x->rotSpeed_8) / v99;
								str_F2C20ar.dword0x0c_realHeight = v28;
								str_F2C20ar.dword0x09_realWidth = v28 * str_F2C20ar.dword0x08_width / str_F2C20ar.dword0x06_height;
								v21 = str_F2C20ar.dword0x08_width;
							LABEL_72:
								str_F2C20ar.dword0x05 = v21;
							LABEL_73:
								v43 = str_F2C20ar.dword0x00;
								str_F2C20ar.dword0x02_data = a1y->textureBuffer;
								a1y->word_0 |= 8;
								if (v43 == 0x2000)
									v45 = x_BYTE_D4750[v7x->byte_10];
								else
									v45 = x_BYTE_D4750[6 + v7x->byte_10];
								str_F2C20ar.dword0x01_rotIdx = v45;
								v46 = str_F2C20ar.dword0x0c_realHeight >> 2;
								str_F2C20ar.dword0x0c_realHeight >>= 2;
								if (str_F2C20ar.dword0x09_realWidth > 0 && v46 > 0)
								{
									v47 = str_F2C20ar.dword0x00 >> 2;
									if (notDay_D4320)
										str_F2C20ar.dword0x00 = 0x2000 - v47;
									else
										str_F2C20ar.dword0x00 = v47 + 0x2000;
									str_F2C20ar.dword0x01_rotIdx = 8;
									DrawSprite_41BD3(0);
								}
								break;
							default:
								goto LABEL_73;
							}
						}
					}
				}
			}
			if (str_F2C20ar.dword0x14x->struct_byte_0xc_12_15.byte[3] >= 0)
				v48 = str_F2C20ar.dword0x14x->axis_0x4C_76.z;
			else
				v48 = str_F2C20ar.dword0x14x->axis_0x4C_76.z - 160;
			v100 = (str_F2C20ar.dword0x17 * v96 + str_F2C20ar.dword0x0f * v97) >> 16;
			v49 = (str_F2C20ar.dword0x0f * v96 - str_F2C20ar.dword0x17 * v97) >> 16;
			if (str_F2C20ar.dword0x14x->struct_byte_0xc_12_15.byte[3] & 0x20)
			{
				v50x = &str_D404C[str_F2C20ar.dword0x14x->byte_0x3B_59];
				switch ((((x_DWORD_EA3E4[str_F2C20ar.dword0x14x->word_0x32_50]->word_0x1C_28
					- (uint16_t)yaw_F2CC0) >> 3) & 0xF0) >> 4)
				{
				case 0:
				case 15:
					v100 -= v50x->word_16;
					break;
				case 1:
				case 14:
					v100 -= v50x->word_18;
					break;
				case 2:
				case 13:
					v100 -= v50x->word_20;
					break;
				case 5:
				case 10:
					v100 += v50x->word_20;
					break;
				case 6:
				case 9:
					v100 += v50x->word_18;
					break;
				case 7:
				case 8:
					v100 += v50x->word_16;
					break;
				default:
					break;
				}
			}
			v51 = v100 * v100 + v49 * v49;
			if (v100 > 64 && v51 < str_F2C20ar.dword0x15_tileRenderCutOffDistance)
			{
				if (v51 <= str_F2C20ar.dword0x13)
				{
					str_F2C20ar.dword0x00 = 0x2000;
				}
				else if (v51 < str_F2C20ar.dword0x16)
				{
					str_F2C20ar.dword0x00 = 32 * (str_F2C20ar.dword0x16 - (v100 * v100 + v49 * v49)) / str_F2C20ar.dword0x12 << 8;
				}
				else
				{
					str_F2C20ar.dword0x00 = 0;
				}
				v52x = &str_WORD_D951C[str_F2C20ar.dword0x14x->word_0x5A_90];
				v53 = v49 * str_F2C20ar.dword0x18 / v100;
				v54 = str_F2C20ar.dword0x18 * (v48 - str_F2C20ar.dword0x20) / v100 + str_F2C20ar.dword0x22;
				str_F2C20ar.dword0x04_screenY = (((int64_t)v53 * str_F2C20ar.cos_0x11 - str_F2C20ar.sin_0x0d * (int64_t)v54) >> 16) + str_F2C20ar.dword0x24;
				str_F2C20ar.dword0x03_screenX = str_F2C20ar.dword0x10 - ((str_F2C20ar.sin_0x0d * (int64_t)v53 + (int64_t)v54 * str_F2C20ar.cos_0x11) >> 16);
				v55 = v52x->byte_12;
				x_BYTE_F2CC6 = 0;
				switch (v55)
				{
				case 0:
					if (str_DWORD_F66F0x[v52x->word_0])
					{
						goto LABEL_105;
					}
					if (MainInitTmaps_71520(v52x->word_0))
					{
					LABEL_105:
						x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v52x->word_0].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
						a1y = *str_DWORD_F66F0x[v52x->word_0];
						goto LABEL_141;
					}
					break;
				case 1:
					if (str_DWORD_F66F0x[v52x->word_0])
					{
						x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v52x->word_0].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
					}
					else
					{
						if (!MainInitTmaps_71520(v52x->word_0))
							break;
						x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v52x->word_0].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
					}
					a1y = *str_DWORD_F66F0x[v52x->word_0];
					goto LABEL_141;
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
				case 16:
					goto LABEL_117;
				case 17:
					v72 = (((str_F2C20ar.dword0x14x->word_0x1C_28 - (uint16_t)yaw_F2CC0) >> 3) & 0xF0) >> 4;
					if (str_F2C20ar.dword0x14x->struct_byte_0xc_12_15.byte[3] & 0x40)
						v72 = (uint8_t)x_BYTE_D4750[44 + v72];
					if (v72 < 8)
					{
						v73 = v72 + v52x->word_0;
						if (str_DWORD_F66F0x[v73])
						{
							v75 = str_TMAPS00TAB_BEGIN_BUFFER[v73].word_8;
						}
						else
						{
							if (!MainInitTmaps_71520(v72 + v52x->word_0))
								break;
							v75 = str_TMAPS00TAB_BEGIN_BUFFER[v72 + v52x->word_0].word_8;
						}
						x_DWORD_F5730[v75] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
						a1y = *str_DWORD_F66F0x[v52x->word_0 + v72];
						goto LABEL_141;
					}
					v77 = v52x->word_0 + 15 - v72;
					if (str_DWORD_F66F0x[v77])
					{
						x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v77].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
					}
					else
					{
						if (!MainInitTmaps_71520(v52x->word_0 + 15 - v72))
							break;
						x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v52x->word_0 + 15 - v72].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
					}
					a1y = *str_DWORD_F66F0x[v52x->word_0 + 15 - v72];
					str_F2C20ar.dword0x08_width = a1y->width;
					str_F2C20ar.dword0x06_height = a1y->height;
					v78 = (signed __int64)(str_F2C20ar.dword0x18 * v52x->rotSpeed_8) / v100;
					str_F2C20ar.dword0x0c_realHeight = v78;
					str_F2C20ar.dword0x09_realWidth = v78 * str_F2C20ar.dword0x08_width / str_F2C20ar.dword0x06_height;
					v67 = -str_F2C20ar.dword0x08_width;
					goto LABEL_163;
				case 18:
					v79 = (((str_F2C20ar.dword0x14x->word_0x1C_28 - (uint16_t)yaw_F2CC0) >> 3) & 0xF0) >> 4;
					v80 = v79 + v52x->word_0;
					if (str_DWORD_F66F0x[v80])
					{
						x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v80].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
					}
					else
					{
						if (!MainInitTmaps_71520(v79 + v52x->word_0))
							break;
						x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v79 + v52x->word_0].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
					}
					a1y = *str_DWORD_F66F0x[v79 + v52x->word_0];
					str_F2C20ar.dword0x08_width = a1y->width;
					str_F2C20ar.dword0x06_height = a1y->height;
					v81 = (signed __int64)(str_F2C20ar.dword0x18 * v52x->rotSpeed_8) / v100;
					str_F2C20ar.dword0x0c_realHeight = v81;
					str_F2C20ar.dword0x09_realWidth = v81 * str_F2C20ar.dword0x08_width / str_F2C20ar.dword0x06_height;
					v67 = str_F2C20ar.dword0x08_width;
					goto LABEL_163;
				case 19:
					v62 = (((str_F2C20ar.dword0x14x->word_0x1C_28 - (uint16_t)yaw_F2CC0) >> 3) & 0xF0) >> 4;
					if (v62 >= 8)
					{
						v68 = v52x->word_0 + (uint8_t)x_BYTE_D4750[12 + v62];
						if (str_DWORD_F66F0x[v68])
						{
							v70 = str_TMAPS00TAB_BEGIN_BUFFER[v68].word_8;
						}
						else
						{
							if (!MainInitTmaps_71520(v52x->word_0 + (uint8_t)x_BYTE_D4750[12 + v62]))
								break;
							v70 = str_TMAPS00TAB_BEGIN_BUFFER[v52x->word_0 + (uint8_t)x_BYTE_D4750[12 + v62]].word_8;
						}
						x_DWORD_F5730[v70] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
						a1y = *str_DWORD_F66F0x[v52x->word_0 + (uint8_t)x_BYTE_D4750[12 + v62]];
						str_F2C20ar.dword0x08_width = a1y->width;
						str_F2C20ar.dword0x06_height = a1y->height;
						v71 = (signed __int64)(str_F2C20ar.dword0x18 * v52x->rotSpeed_8) / v100;
						str_F2C20ar.dword0x0c_realHeight = v71;
						str_F2C20ar.dword0x09_realWidth = v71 * str_F2C20ar.dword0x08_width / str_F2C20ar.dword0x06_height;
						v67 = -str_F2C20ar.dword0x08_width;
					}
					else
					{
						v63 = v52x->word_0 + (uint8_t)x_BYTE_D4750[12 + v62];
						if (str_DWORD_F66F0x[v63])
						{
							v65 = str_TMAPS00TAB_BEGIN_BUFFER[v63].word_8;
						}
						else
						{
							if (!MainInitTmaps_71520(v52x->word_0 + (uint8_t)x_BYTE_D4750[12 + v62]))
								break;
							v65 = str_TMAPS00TAB_BEGIN_BUFFER[(uint8_t)x_BYTE_D4750[12 + v62] + v52x->word_0].word_8;
						}
						x_DWORD_F5730[v65] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
						a1y = *str_DWORD_F66F0x[v52x->word_0 + (uint8_t)x_BYTE_D4750[12 + v62]];
						str_F2C20ar.dword0x08_width = a1y->width;
						str_F2C20ar.dword0x06_height = a1y->height;
						v66 = (signed __int64)(str_F2C20ar.dword0x18 * v52x->rotSpeed_8) / v100;
						str_F2C20ar.dword0x0c_realHeight = v66;
						str_F2C20ar.dword0x09_realWidth = v66 * str_F2C20ar.dword0x08_width / str_F2C20ar.dword0x06_height;
						v67 = str_F2C20ar.dword0x08_width;
					}
					goto LABEL_163;
				case 20:
					v82 = (((str_F2C20ar.dword0x14x->word_0x1C_28 - (uint16_t)yaw_F2CC0) >> 3) & 0xF0) >> 4;
					if (v82 >= 8)
					{
						v86 = (uint8_t)x_BYTE_D4750[28 + v82] + v52x->word_0;
						if (str_DWORD_F66F0x[v86])
						{
							v88 = str_TMAPS00TAB_BEGIN_BUFFER[v86].word_8;
						}
						else
						{
							if (!MainInitTmaps_71520(v52x->word_0 + (uint8_t)x_BYTE_D4750[28 + v82]))
								break;
							v88 = str_TMAPS00TAB_BEGIN_BUFFER[v52x->word_0 + (uint8_t)x_BYTE_D4750[28 + v82]].word_8;
						}
						x_DWORD_F5730[v88] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
						a1y = *str_DWORD_F66F0x[v52x->word_0 + (uint8_t)x_BYTE_D4750[28 + v82]];
						str_F2C20ar.dword0x08_width = a1y->width;
						str_F2C20ar.dword0x06_height = a1y->height;
						v89 = (signed __int64)(str_F2C20ar.dword0x18 * v52x->rotSpeed_8) / v100;
						str_F2C20ar.dword0x0c_realHeight = v89;
						str_F2C20ar.dword0x09_realWidth = v89 * str_F2C20ar.dword0x08_width / str_F2C20ar.dword0x06_height;
						v67 = -str_F2C20ar.dword0x08_width;
					}
					else
					{
						v83 = (uint8_t)x_BYTE_D4750[28 + v82];
						v84 = v83 + v52x->word_0;
						if (str_DWORD_F66F0x[v84])
						{
							x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v84].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
						}
						else
						{
							if (!MainInitTmaps_71520(v52x->word_0 + (uint8_t)v83))
								break;
							x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v52x->word_0 + (uint8_t)x_BYTE_D4750[28 + v82]].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
						}
						a1y = *str_DWORD_F66F0x[v52x->word_0 + (uint8_t)x_BYTE_D4750[28 + v82]];
						str_F2C20ar.dword0x08_width = a1y->width;
						str_F2C20ar.dword0x06_height = a1y->height;
						v85 = (signed __int64)(str_F2C20ar.dword0x18 * v52x->rotSpeed_8) / v100;
						str_F2C20ar.dword0x0c_realHeight = v85;
						str_F2C20ar.dword0x09_realWidth = v85 * str_F2C20ar.dword0x08_width / str_F2C20ar.dword0x06_height;
						v67 = str_F2C20ar.dword0x08_width;
					}
					goto LABEL_163;
				case 21:
					v59 = v52x->word_0;
					if (str_DWORD_F66F0x[v59])
					{
						x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v59].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
					}
					else
					{
						if (!MainInitTmaps_71520(v59))
							break;
						x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v52x->word_0].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
					}
					a1y = *str_DWORD_F66F0x[v52x->word_0];
					x_BYTE_F2CC6 = 1;
					goto LABEL_141;
				case 22:
				case 23:
				case 24:
				case 25:
				case 26:
				case 27:
				case 28:
				case 29:
				case 30:
				case 31:
				case 32:
				case 33:
				case 34:
				case 35:
				case 36:
					x_BYTE_F2CC6 = 1;
				LABEL_117:
					v61 = v52x->word_0 + str_F2C20ar.dword0x14x->byte_0x5C_92;
					if (str_DWORD_F66F0x[v61])
					{
						x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v61].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
					}
					else
					{
						if (!MainInitTmaps_71520(v52x->word_0 + str_F2C20ar.dword0x14x->byte_0x5C_92))
							break;
						x_DWORD_F5730[str_TMAPS00TAB_BEGIN_BUFFER[v52x->word_0 + str_F2C20ar.dword0x14x->byte_0x5C_92].word_8] = x_D41A0_BYTEARRAY_4_struct.byteindex_26;
					}
					a1y = *str_DWORD_F66F0x[v52x->word_0 + str_F2C20ar.dword0x14x->byte_0x5C_92];
				LABEL_141:
					str_F2C20ar.dword0x08_width = a1y->width;
					str_F2C20ar.dword0x06_height = a1y->height;
					v76 = (signed __int64)(str_F2C20ar.dword0x18 * v52x->rotSpeed_8) / v100;
					str_F2C20ar.dword0x0c_realHeight = v76;
					str_F2C20ar.dword0x09_realWidth = v76 * str_F2C20ar.dword0x08_width / str_F2C20ar.dword0x06_height;
					v67 = str_F2C20ar.dword0x08_width;
				LABEL_163:
					str_F2C20ar.dword0x05 = v67;
				LABEL_164:
					v90x = str_F2C20ar.dword0x14x;
					a1y->word_0 |= 8u;
					v91 = v90x->struct_byte_0xc_12_15.byte[2];
					str_F2C20ar.dword0x02_data = a1y->textureBuffer;
					if (v91 & 0x380)
					{
						v93 = v90x->struct_byte_0xc_12_15.byte[3];
						if (v93 & 2)
						{
							v94 = (uint8_t)x_BYTE_E88E0x[2 + 3
								* x_DWORD_EA3E4[v90x->parentId_0x28_40]->dword_0xA4_164x->word_0x38_56];
							str_F2C20ar.dword0x01_rotIdx = 4;
							str_F2C20ar.dword0x07 = v94;
						}
						else if (v93 & 4)
						{
							v95 = (uint8_t)x_BYTE_E88E0x[2 + 3
								* x_DWORD_EA3E4[v90x->parentId_0x28_40]->dword_0xA4_164x->word_0x38_56];
							str_F2C20ar.dword0x01_rotIdx = 5;
							str_F2C20ar.dword0x07 = v95;
						}
						else if (v90x->struct_byte_0xc_12_15.byte[2] >= 0)
						{
							if (v93 & 1)
								str_F2C20ar.dword0x01_rotIdx = 3;
						}
						else
						{
							str_F2C20ar.dword0x01_rotIdx = 2;
						}
					}
					else
					{
						if (str_F2C20ar.dword0x00 == 0x2000)
							v92 = x_BYTE_D4750[v52x->byte_10];
						else
							v92 = x_BYTE_D4750[6 + v52x->byte_10];
						str_F2C20ar.dword0x01_rotIdx = v92;
					}
					str_F2C20ar.dword0x09_realWidth++;
					str_F2C20ar.dword0x0c_realHeight++;
					DrawSprite_41BD3(1u);
					break;
				default:
					goto LABEL_164;
				}
			}
		}
	LABEL_178:
		result = str_F2C20ar.dword0x14x->oldMapEntity_0x16_22;
	} while (result);
}

void GameRenderHD::DrawSprite_41BD3(uint32 a1)
{
	int8_t* ptrSpriteRenderSrc_v2x; // ebx
	x_DWORD* v3; // esi
	uint8_t* v4; // edi
	int v5; // ecx
	char v6; // cf
	int v7; // ecx
	int v8; // ecx
	char v9; // al
	char v10; // al
	char v11; // al
	char v12; // al
	int v13; // eax
	x_BYTE* ptrSpriteRenderSrc_v14; // ebx
	x_DWORD* v15; // esi
	uint8_t* v16; // edi
	int v17; // ecx
	x_BYTE* ptrSpriteRenderSrc_v18; // ebx
	x_DWORD* v19; // esi
	uint8_t* v20; // edi
	int v21; // eax
	int v22; // ecx
	x_BYTE* ptrSpriteRenderSrc_v23; // ebx
	x_DWORD* v24; // esi
	uint8_t* v25; // edi
	int v26; // eax
	int v27; // ecx
	x_BYTE* ptrSpriteRenderSrc_v28; // ebx
	x_DWORD* v29; // esi
	uint8_t* v30; // edi
	int v31; // eax
	int v32; // ecx
	int v33; // eax
	x_BYTE* ptrSpriteRenderSrc_v34; // ebx
	x_DWORD* v35; // esi
	uint8_t* v36; // edi
	int v37; // ecx
	int v38; // edx
	x_BYTE* ptrSpriteRenderSrc_v39; // ebx
	x_DWORD* v40; // esi
	uint8_t* v41; // edi
	int v42; // eax
	int v43; // ecx
	int v44; // edx
	x_BYTE* ptrSpriteRenderSrc_v45; // ebx
	x_DWORD* v46; // esi
	uint8_t* v47; // edi
	int v48; // eax
	int v49; // ecx
	int v50; // ST4C_4
	int8_t* ptrSpriteRenderSrc_v51x; // ebx
	uint8_t* ptrSpriteRenderDest_v52; // edx
	x_DWORD* v53; // esi
	type_unk_F0E20x* v54x; // edi
	int v55; // ecx
	int v56; // ecx
	int v57; // ecx
	char v58; // al
	char v59; // al
	char v60; // al
	char v61; // al
	int8_t* ptrSpriteRenderSrc_v62x; // ebx
	int v63; // eax
	uint8_t* ptrSpriteRenderDest_v64; // edx
	x_DWORD* v65; // esi
	type_unk_F0E20x* v66x; // edi
	int v67; // ecx
	int8_t* ptrSpriteRenderSrc_v68x; // ebx
	uint8_t* ptrSpriteRenderDest_v69; // edx
	x_DWORD* v70; // esi
	type_unk_F0E20x* v71x; // edi
	int v72; // eax
	int v73; // ecx
	int8_t* ptrSpriteRenderSrc_v74x; // ebx
	uint8_t* ptrSpriteRenderDest_v75; // edx
	x_DWORD* v76; // esi
	type_unk_F0E20x* v77x; // edi
	int v78; // eax
	int v79; // ecx
	int8_t* ptrSpriteRenderSrc_v80x; // ebx
	uint8_t* ptrSpriteRenderDest_v81; // edx
	x_DWORD* v82; // esi
	type_unk_F0E20x* v83x; // edi
	int v84; // eax
	int v85; // ecx
	int8_t* ptrSpriteRenderSrc_v86x; // ebx
	int v87; // eax
	uint8_t* ptrSpriteRenderDest_v88; // edx
	x_DWORD* v89; // esi
	type_unk_F0E20x* v90x; // edi
	int v91; // ecx
	int v92; // ecx
	int8_t* ptrSpriteRenderSrc_v93x; // ebx
	uint8_t* ptrSpriteRenderDest_v94; // edx
	int v95; // eax
	x_DWORD* v96; // esi
	type_unk_F0E20x* v97x; // edi
	int v98; // ecx
	int8_t* ptrSpriteRenderSrc_v99x; // ebx
	uint8_t* ptrSpriteRenderDest_v100; // edx
	int v101; // eax
	x_DWORD* v102; // esi
	type_unk_F0E20x* v103x; // edi
	int8_t* ptrSpriteRenderSrc_v104x; // ebx
	int v105; // eax
	uint8_t* ptrSpriteRenderDest_v106; // edx
	x_DWORD* v107; // esi
	type_unk_F0E20x* v108x; // edi
	int v109; // ecx
	char v110; // al
	char v111; // al
	int v112; // eax
	int v113; // edx
	int v114; // edx
	signed int v116; // [esp+10h] [ebp-54h]
	x_DWORD* v117; // [esp+14h] [ebp-50h]
	x_DWORD* v118; // [esp+14h] [ebp-50h]
	int32_t* v119; // [esp+14h] [ebp-50h]
	x_DWORD* v120; // [esp+14h] [ebp-50h]
	int8_t* v121x; // [esp+1Ch] [ebp-48h]
	uint8_t* v122x; // [esp+20h] [ebp-44h]
	uint8_t* ptrRenderBuffer; // [esp+20h] [ebp-44h]
	int v124; // [esp+24h] [ebp-40h]
	int v125; // [esp+24h] [ebp-40h]
	int i; // [esp+24h] [ebp-40h]
	int k; // [esp+24h] [ebp-40h]
	int v129; // [esp+24h] [ebp-40h]
	int v130; // [esp+24h] [ebp-40h]
	int l; // [esp+28h] [ebp-3Ch]
	int v132; // [esp+28h] [ebp-3Ch]
	int v133; // [esp+28h] [ebp-3Ch]
	int v134; // [esp+34h] [ebp-30h]
	int v135; // [esp+34h] [ebp-30h]
	int v136; // [esp+38h] [ebp-2Ch]
	int v137; // [esp+38h] [ebp-2Ch]
	int v138; // [esp+3Ch] [ebp-28h]
	int v139; // [esp+3Ch] [ebp-28h]
	int v140; // [esp+3Ch] [ebp-28h]
	int v141; // [esp+3Ch] [ebp-28h]
	int v142; // [esp+3Ch] [ebp-28h]
	int v143; // [esp+3Ch] [ebp-28h]
	int v144; // [esp+3Ch] [ebp-28h]
	int v145; // [esp+3Ch] [ebp-28h]
	int v146; // [esp+3Ch] [ebp-28h]
	int v147; // [esp+3Ch] [ebp-28h]
	int v148; // [esp+3Ch] [ebp-28h]
	int v149; // [esp+3Ch] [ebp-28h]
	int v150; // [esp+3Ch] [ebp-28h]
	int v151; // [esp+3Ch] [ebp-28h]
	int v152; // [esp+3Ch] [ebp-28h]
	x_DWORD* v153; // [esp+40h] [ebp-24h]
	x_DWORD* v154; // [esp+40h] [ebp-24h]
	uint8_t* v155; // [esp+40h] [ebp-24h]
	int v156; // [esp+44h] [ebp-20h]
	int v157; // [esp+48h] [ebp-1Ch]
	int v158; // [esp+48h] [ebp-1Ch]
	int scaledHeight; // [esp+4Ch] [ebp-18h]
	int v160; // [esp+50h] [ebp-14h]
	int v161; // [esp+50h] [ebp-14h]
	int* v162; // [esp+54h] [ebp-10h]
	x_DWORD* v163; // [esp+54h] [ebp-10h]
	x_DWORD* v164; // [esp+54h] [ebp-10h]
	x_DWORD* v165; // [esp+54h] [ebp-10h]
	int* v166; // [esp+54h] [ebp-10h]
	x_DWORD* v167; // [esp+54h] [ebp-10h]
	x_DWORD* v168; // [esp+54h] [ebp-10h]
	type_unk_F0E20x* v169x; // [esp+58h] [ebp-Ch]
	int screenPosX; // [esp+5Ch] [ebp-8h]
	int v171; // [esp+5Ch] [ebp-8h]
	int v172; // [esp+60h] [ebp-4h]
	int v173; // [esp+60h] [ebp-4h]
	int v174; // [esp+60h] [ebp-4h]
	int v175; // [esp+60h] [ebp-4h]

	int jy;

	/*uint8_t origbyte2y[100];
	uint8_t remakebyte2y[100];
	int remakepos2y[100];
	int comp22a = compare_with_sequence_array_222BD3((char*)"00222BD3", (uint8_t*)x_DWORD_F2C20ar, 0x222bd3, debugcounter_sub_41BD3_subDrawSprite, 0x28 * 4, origbyte2y, remakebyte2y, remakepos2y);

	if (comp22a< 0x28 * 4)
		comp22a = comp22a;
		*/
		/*if (CommandLineParams.DoDebugafterload())
			VGA_Debug_Blit(640, 480, m_ptrScreenBuffer_351628);*/

	if (!x_BYTE_F2CC6)
	{
		if (a1 < 1)
		{
			if (a1)//a1==0
				goto LABEL_126;
		}
		else
		{
			if (a1 <= 1)//a1==1
			{
				str_F2C20ar.dword0x04_screenY -= ((str_F2C20ar.cos_0x11 * str_F2C20ar.dword0x09_realWidth >> 1) + str_F2C20ar.sin_0x0d * str_F2C20ar.dword0x0c_realHeight) >> 16;
				str_F2C20ar.dword0x03_screenX -= (str_F2C20ar.cos_0x11 * str_F2C20ar.dword0x0c_realHeight - (str_F2C20ar.sin_0x0d * str_F2C20ar.dword0x09_realWidth >> 1)) >> 16;
				goto LABEL_126;
			}
			if (a1 != 2)//a1 == 0,1
			{
			LABEL_126:
				if ((unsigned int)str_F2C20ar.dword0x1e <= 7)
				{
					switch (str_F2C20ar.dword0x1e)//mirroring
					{
					case 0:
						v156 = str_F2C20ar.dword0x1b * str_F2C20ar.dword0x09_realWidth >> 16;
						if (v156 <= 0)
							break;
						scaledHeight = (str_F2C20ar.dword0x0c_realHeight << 16) / str_F2C20ar.dword0x1b;
						if (scaledHeight <= 0)
							break;
						v135 = (str_F2C20ar.dword0x06_height << 16) / scaledHeight;
						if (a1 == 1)
						{
							str_F2C20ar.dword0x0a_actIdx = 0;
						}
						else
						{
							str_F2C20ar.dword0x0a_actIdx = (scaledHeight - 1) * v135;
							v135 = -v135;
						}
						v160 = str_F2C20ar.dword0x27 * str_F2C20ar.dword0x0c_realHeight / scaledHeight;
						v162 = (int*)(&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_2]);
						v157 = str_F2C20ar.dword0x04_screenY << 16;
						screenPosX = str_F2C20ar.dword0x03_screenX - (str_F2C20ar.dword0x1f * str_F2C20ar.dword0x04_screenY >> 16);
						if (str_F2C20ar.dword0x03_screenX - (str_F2C20ar.dword0x1f * str_F2C20ar.dword0x04_screenY >> 16) >= str_F2C20ar.dword0x21)
							goto LABEL_136;
						v139 = str_F2C20ar.dword0x21 - screenPosX;
						scaledHeight -= str_F2C20ar.dword0x21 - screenPosX;
						if (scaledHeight > 0)
						{
							str_F2C20ar.dword0x0a_actIdx += v139 * v135;
							v157 -= v139 * v160;
							screenPosX = str_F2C20ar.dword0x21;
						LABEL_136:
							ptrRenderBuffer = iScreenWidth_DE560 * screenPosX + ViewPortRenderBufferStart_DE558;
							goto LABEL_137;
						}
						break;
					case 1:
						v156 = str_F2C20ar.dword0x27 * str_F2C20ar.dword0x09_realWidth >> 16;
						if (v156 <= 0)
							break;
						scaledHeight = (str_F2C20ar.dword0x0c_realHeight << 16) / str_F2C20ar.dword0x27;
						if (scaledHeight <= 0)
							break;
						v135 = (str_F2C20ar.dword0x06_height << 16) / scaledHeight;
						if (a1 == 1)
						{
							str_F2C20ar.dword0x0a_actIdx = 0;
						}
						else
						{
							str_F2C20ar.dword0x0a_actIdx = (scaledHeight - 1) * v135;
							v135 = -v135;
						}
						v161 = str_F2C20ar.dword0x1b * str_F2C20ar.dword0x0c_realHeight / scaledHeight;
						v166 = (int*)(&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_2]);
						v158 = str_F2C20ar.dword0x03_screenX << 16;
						v171 = str_F2C20ar.dword0x04_screenY - (str_F2C20ar.dword0x1f * str_F2C20ar.dword0x03_screenX >> 16);
						if (str_F2C20ar.dword0x04_screenY - (str_F2C20ar.dword0x1f * str_F2C20ar.dword0x03_screenX >> 16) < str_F2C20ar.width0x25)
						{
							if (v171 >= str_F2C20ar.dword0x21)
								goto LABEL_284;
						}
						else
						{
							v141 = v171 - str_F2C20ar.width0x25;
							scaledHeight -= v171 - str_F2C20ar.width0x25;
							if (scaledHeight > 0)
							{
								str_F2C20ar.dword0x0a_actIdx += v141 * v135;
								v158 += v141 * v161;
								v171 = str_F2C20ar.width0x25;
							LABEL_284:
								ptrRenderBuffer = v171 + ViewPortRenderBufferStart_DE558;
								goto LABEL_285;
							}
						}
						break;
					case 2:
						v156 = str_F2C20ar.dword0x1b * str_F2C20ar.dword0x09_realWidth >> 16;
						if (v156 <= 0)
							break;
						scaledHeight = (str_F2C20ar.dword0x0c_realHeight << 16) / str_F2C20ar.dword0x1b;
						if (scaledHeight <= 0)
							break;
						v135 = (str_F2C20ar.dword0x06_height << 16) / scaledHeight;
						if (a1 == 1)
						{
							str_F2C20ar.dword0x0a_actIdx = 0;
						}
						else
						{
							str_F2C20ar.dword0x0a_actIdx = (scaledHeight - 1) * v135;
							v135 = -v135;
						}
						v160 = str_F2C20ar.dword0x27 * str_F2C20ar.dword0x0c_realHeight / scaledHeight;
						v162 = (int*)(&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_2]);
						v157 = str_F2C20ar.dword0x03_screenX << 16;
						screenPosX = str_F2C20ar.width0x25 - str_F2C20ar.dword0x04_screenY - (str_F2C20ar.dword0x1f * str_F2C20ar.dword0x03_screenX >> 16);
						if (screenPosX >= str_F2C20ar.dword0x21)
							goto LABEL_329;
						v147 = str_F2C20ar.dword0x21 - screenPosX;
						scaledHeight -= str_F2C20ar.dword0x21 - screenPosX;
						if (scaledHeight <= 0)
							break;
						str_F2C20ar.dword0x0a_actIdx += v147 * v135;
						v157 -= v147 * v160;
						screenPosX = str_F2C20ar.dword0x21;
					LABEL_329:
						ptrRenderBuffer = str_F2C20ar.width0x25 + ViewPortRenderBufferStart_DE558 - 1 - screenPosX;
						goto LABEL_137;
					case 3:
						v156 = str_F2C20ar.dword0x27 * str_F2C20ar.dword0x09_realWidth >> 16;
						if (v156 <= 0)
							break;
						scaledHeight = (str_F2C20ar.dword0x0c_realHeight << 16) / str_F2C20ar.dword0x27;
						if (scaledHeight <= 0)
							break;
						v135 = (str_F2C20ar.dword0x06_height << 16) / scaledHeight;
						if (a1 == 1)
						{
							str_F2C20ar.dword0x0a_actIdx = 0;
						}
						else
						{
							str_F2C20ar.dword0x0a_actIdx = (scaledHeight - 1) * v135;
							v135 = -v135;
						}
						v161 = str_F2C20ar.dword0x1b * str_F2C20ar.dword0x0c_realHeight / scaledHeight;
						v166 = (int*)(&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_2]);
						v158 = (str_F2C20ar.height0x26 - str_F2C20ar.dword0x04_screenY) << 16;
						v171 = str_F2C20ar.dword0x03_screenX - (str_F2C20ar.dword0x1f * (str_F2C20ar.height0x26 - str_F2C20ar.dword0x04_screenY) >> 16);
						if (str_F2C20ar.dword0x03_screenX - (str_F2C20ar.dword0x1f * (str_F2C20ar.height0x26 - str_F2C20ar.dword0x04_screenY) >> 16) < str_F2C20ar.width0x25)
						{
							if (v171 < str_F2C20ar.dword0x21)
								break;
						}
						else
						{
							v148 = v171 - str_F2C20ar.width0x25;
							scaledHeight -= v171 - str_F2C20ar.width0x25;
							if (scaledHeight <= 0)
								break;
							str_F2C20ar.dword0x0a_actIdx += v148 * v135;
							v158 += v148 * v161;
							v171 = str_F2C20ar.width0x25;
						}
						ptrRenderBuffer = str_F2C20ar.height0x26 + iScreenWidth_DE560 * v171 - 1 + ViewPortRenderBufferStart_DE558;
						goto LABEL_285;
					case 4:
						v156 = str_F2C20ar.dword0x1b * str_F2C20ar.dword0x09_realWidth >> 16;
						if (v156 <= 0)
							break;
						scaledHeight = (str_F2C20ar.dword0x0c_realHeight << 16) / str_F2C20ar.dword0x1b;
						if (scaledHeight <= 0)
							break;
						v135 = (str_F2C20ar.dword0x06_height << 16) / scaledHeight;
						if (a1 == 1)
						{
							str_F2C20ar.dword0x0a_actIdx = 0;
						}
						else
						{
							str_F2C20ar.dword0x0a_actIdx = (scaledHeight - 1) * v135;
							v135 = -v135;
						}
						v160 = str_F2C20ar.dword0x27 * str_F2C20ar.dword0x0c_realHeight / scaledHeight;
						v162 = (int*)(&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_2]);
						v157 = (str_F2C20ar.height0x26 - str_F2C20ar.dword0x04_screenY) << 16;
						screenPosX = str_F2C20ar.width0x25 - str_F2C20ar.dword0x03_screenX - (str_F2C20ar.dword0x1f * (str_F2C20ar.height0x26 - str_F2C20ar.dword0x04_screenY) >> 16);
						if (screenPosX >= str_F2C20ar.dword0x21)
							goto LABEL_348;
						v149 = str_F2C20ar.dword0x21 - screenPosX;
						scaledHeight -= str_F2C20ar.dword0x21 - screenPosX;
						if (scaledHeight <= 0)
							break;
						str_F2C20ar.dword0x0a_actIdx += v149 * v135;
						v157 -= v149 * v160;
						screenPosX = str_F2C20ar.dword0x21;
					LABEL_348:
						ptrRenderBuffer = str_F2C20ar.height0x26 + ViewPortRenderBufferStart_DE558 + iScreenWidth_DE560 * (str_F2C20ar.width0x25 - screenPosX - 1) - 1;
						goto LABEL_137;
					case 5:
						v156 = str_F2C20ar.dword0x27 * str_F2C20ar.dword0x09_realWidth >> 16;
						if (v156 <= 0)
							break;
						scaledHeight = (str_F2C20ar.dword0x0c_realHeight << 16) / str_F2C20ar.dword0x27;
						if (scaledHeight <= 0)
							break;
						v135 = (str_F2C20ar.dword0x06_height << 16) / scaledHeight;
						if (a1 == 1)
						{
							str_F2C20ar.dword0x0a_actIdx = 0;
						}
						else
						{
							str_F2C20ar.dword0x0a_actIdx = (scaledHeight - 1) * v135;
							v135 = -v135;
						}
						v161 = str_F2C20ar.dword0x1b * str_F2C20ar.dword0x0c_realHeight / scaledHeight;
						v166 = (int*)(&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_2]);
						v158 = (str_F2C20ar.height0x26 - str_F2C20ar.dword0x03_screenX) << 16;
						v113 = str_F2C20ar.dword0x1f * (str_F2C20ar.height0x26 - str_F2C20ar.dword0x03_screenX) >> 16;
						v171 = str_F2C20ar.width0x25 - str_F2C20ar.dword0x04_screenY - v113;
						if (v171 < str_F2C20ar.width0x25)
						{
							if (v171 < str_F2C20ar.dword0x21)
								break;
						}
						else
						{
							v150 = v171 - str_F2C20ar.width0x25;
							scaledHeight -= v171 - str_F2C20ar.width0x25;
							if (scaledHeight <= 0)
								break;
							str_F2C20ar.dword0x0a_actIdx += v150 * v135;
							v158 += v150 * v161;
							v171 = str_F2C20ar.width0x25;
						}
						ptrRenderBuffer = (str_F2C20ar.height0x26 - 1) * iScreenWidth_DE560 + str_F2C20ar.width0x25 + ViewPortRenderBufferStart_DE558 - 1 - v171;
						goto LABEL_285;
					case 6:
						v156 = str_F2C20ar.dword0x1b * str_F2C20ar.dword0x09_realWidth >> 16;
						if (v156 <= 0)
							break;
						scaledHeight = (str_F2C20ar.dword0x0c_realHeight << 16) / str_F2C20ar.dword0x1b;
						if (scaledHeight <= 0)
							break;
						v135 = (str_F2C20ar.dword0x06_height << 16) / scaledHeight;
						if (a1 == 1)
						{
							str_F2C20ar.dword0x0a_actIdx = 0;
						}
						else
						{
							str_F2C20ar.dword0x0a_actIdx = (scaledHeight - 1) * v135;
							v135 = -v135;
						}
						v160 = str_F2C20ar.dword0x27 * str_F2C20ar.dword0x0c_realHeight / scaledHeight;
						v162 = (int*)(&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_2]);
						v157 = (str_F2C20ar.height0x26 - str_F2C20ar.dword0x03_screenX) << 16;
						screenPosX = str_F2C20ar.dword0x04_screenY - (str_F2C20ar.dword0x1f * (str_F2C20ar.height0x26 - str_F2C20ar.dword0x03_screenX) >> 16);
						if (str_F2C20ar.dword0x04_screenY - (str_F2C20ar.dword0x1f * (str_F2C20ar.height0x26 - str_F2C20ar.dword0x03_screenX) >> 16) >= str_F2C20ar.dword0x21)
							goto LABEL_367;
						v151 = str_F2C20ar.dword0x21 - screenPosX;
						scaledHeight -= str_F2C20ar.dword0x21 - screenPosX;
						if (scaledHeight <= 0)
							break;
						str_F2C20ar.dword0x0a_actIdx += v151 * v135;
						v157 -= v151 * v160;
						screenPosX = str_F2C20ar.dword0x21;
					LABEL_367:
						ptrRenderBuffer = ViewPortRenderBufferStart_DE558 + iScreenWidth_DE560 * (str_F2C20ar.height0x26 - 1) + screenPosX;
					LABEL_137:
						v140 = str_F2C20ar.width0x25 - str_F2C20ar.dword0x21;
						if (screenPosX <= 0)
						{
							if (scaledHeight > v140)
								scaledHeight = str_F2C20ar.width0x25 - str_F2C20ar.dword0x21;
						}
						else if (screenPosX + scaledHeight > v140)
						{
							scaledHeight = v140 - screenPosX;
							if (v140 - screenPosX <= 0)
								break;
						}
						v116 = 9999999;
						v124 = scaledHeight;
						while (2)
						{
							if (!v124)
								goto LABEL_154;
							v172 = v157 >> 16;
							if (v157 >> 16 >= 0)
							{
								v162[0] = v172;
								v162[1] = v156;
								v162[2] = 0;
								v116 = 0;
							LABEL_151:
								if (v162[1] + *v162 > str_F2C20ar.height0x26)
									v162[1] = str_F2C20ar.height0x26 - *v162;
								v157 -= v160;
								v162 += 3;
								v124--;
								continue;
							}
							break;
						}
						v173 = -v172;
						v162[0] = 0;
						v162[1] = v156 - v173;
						if (v162[1] > 0)
						{
							v162[2] = v173;
							if (v173 < v116)
								v116 = v173;
							goto LABEL_151;
						}
						scaledHeight -= v124;
					LABEL_154:
						if (screenPosX + scaledHeight > str_F2C20ar.Height_0x19)
						{
							if (str_F2C20ar.Height_0x19 - screenPosX <= 0)
							{
								v163 = (x_DWORD*)(&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_2]);
								v117 = (x_DWORD*)(4 * (str_F2C20ar.Height_0x19 - screenPosX) + str_F2C20ar.pbyte0x1a);
								v125 = scaledHeight;
							}
							else
							{
								v163 = (x_DWORD*)(12 * (str_F2C20ar.Height_0x19 - screenPosX) + &m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_2]);
								v117 = (x_DWORD*)str_F2C20ar.pbyte0x1a;
								v125 = screenPosX + scaledHeight - str_F2C20ar.Height_0x19;
							}
							while (v125)
							{
								if (*v163 + v163[1] > *v117)
								{
									if (*v163 >= *v117)
									{
										scaledHeight -= v125;
										break;
									}
									v163[1] = *v117 - *v163;
								}
								v163 += 3;
								v117--;
								v125--;
							}
						}
						if (screenPosX < 0)
						{
							v118 = (x_DWORD*)(4 * (-1 - screenPosX) + m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_3);
							v164 = (x_DWORD*)(&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_2]);
							while (screenPosX)
							{
								if (*v118 > *v164)
								{
									if (v164[1] + *v164 >= *v118)
									{
										v50 = *v118 - *v164;
										v164[0] = *v118;
										v164[2] += v50;
										v164[1] -= v50;
									}
									else
									{
										v164[1] = 0;
									}
								}
								v118--;
								v164 += 3;
								screenPosX++;
							}
						}
					LABEL_172:
						v137 = (str_F2C20ar.dword0x05 << 16) / v156;
						str_F2C20ar.dword0x0b = 0;
						if (str_F2C20ar.dword0x05 < 0)
							str_F2C20ar.dword0x0b -= v137 * (v156 - 1);
						v132 = v156 - v116;
						if (v156 - v116 > 0)
						{
							if (v132 > str_F2C20ar.dword0x1c)
								v132 = str_F2C20ar.dword0x1c;
							str_F2C20ar.dword0x0b += v137 * v116;
							v154 = (x_DWORD*)(&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_1]);
							while (v132 >= 0)
							{
								v154[1] = str_F2C20ar.dword0x0b >> 16;
								*v154 = v154[1] - *(v154 - 1);
								str_F2C20ar.dword0x0b += v137;
								v154 += 2;
								v132--;
							}
							v165 = (x_DWORD*)(&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_2]);

							//Draw Sprite to Render Buffer (rotated and scaled)
							for (i = scaledHeight; i; i--)
							{
								v133 = v165[1];
								if (v133 > 0)
								{
									//adress 2237d3
									v169x = &m_str_F0E20x[*v165];
									v155 = 8 * (v165[2] - v116) + &m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_1];
									v121x = &str_F2C20ar.dword0x02_data[str_F2C20ar.dword0x08_width * (str_F2C20ar.dword0x0a_actIdx >> 16)];

									if ((unsigned int)str_F2C20ar.dword0x01_rotIdx <= 8)
									{
										switch (str_F2C20ar.dword0x01_rotIdx)
										{
										case 0:
											ptrSpriteRenderSrc_v51x = &v121x[*(x_DWORD*)(v155 + 4)];
											ptrSpriteRenderDest_v52 = (uint8_t*)(v169x->dword_1 + ptrRenderBuffer);
											v53 = (x_DWORD*)(8 * (v165[2] - v116) + &m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_1]);
											v54x = &m_str_F0E20x[*v165];
											v55 = v133 >> 1;
											if (!(v133 & 1))
											{
												v6 = v55 & 1;
												v56 = v133 >> 2;
												if (v6)
												{
													v56++;
													v53 = (x_DWORD*)(v155 - 8);
													v54x = &v169x[-1];
													goto LABEL_197;
												}
												v53 = (x_DWORD*)(v155 + 8);
												v54x = &v169x[1];
												goto LABEL_191;
											}
											v57 = v55 + 2;
											v6 = v57 & 1;
											v56 = v57 >> 1;
											if (!v6)
											{
												v53 = (x_DWORD*)(v155 - 16);
												v54x = &v169x[-2];
												goto LABEL_200;
											}
											while (1)
											{
												v59 = ptrSpriteRenderSrc_v51x[0];
												ptrSpriteRenderSrc_v51x += v53[2];

												if (v59)
													*ptrSpriteRenderDest_v52 = v59;
												ptrSpriteRenderDest_v52 += v54x[1].dword_0;

											LABEL_197:

												v60 = ptrSpriteRenderSrc_v51x[0];
												ptrSpriteRenderSrc_v51x += v53[4];

												if (v60)
													*ptrSpriteRenderDest_v52 = v60;
												ptrSpriteRenderDest_v52 += v54x[2].dword_0;

											LABEL_200:

												v61 = ptrSpriteRenderSrc_v51x[0];
												ptrSpriteRenderSrc_v51x += v53[6];

												if (v61)
													*ptrSpriteRenderDest_v52 = v61;
												ptrSpriteRenderDest_v52 += v54x[3].dword_0;

												v53 += 8;
												v54x += 4;
												if (!--v56)
													break;
											LABEL_191:

												v58 = ptrSpriteRenderSrc_v51x[0];
												ptrSpriteRenderSrc_v51x += *v53;

												if (v58)
													*ptrSpriteRenderDest_v52 = v58;
												ptrSpriteRenderDest_v52 += v54x[0].dword_0;
											}
											break;
										case 1:
											ptrSpriteRenderSrc_v62x = &v121x[*(x_DWORD*)(v155 + 4)];

											v63 = str_F2C20ar.dword0x00;
											ptrSpriteRenderDest_v64 = (uint8_t*)(v169x->dword_1 + ptrRenderBuffer);

											v65 = (x_DWORD*)(8 * (v165[2] - v116) + &m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_1]);
											v66x = &m_str_F0E20x[*v165];
											v67 = v133 >> 1;
											if (!(v133 & 1))
											{
												v65 = (x_DWORD*)(v155 + 8);
												v66x = &v169x[1];
												goto LABEL_207;
											}
											v67++;
											while (1)
											{
												LOBYTE(v63) = ptrSpriteRenderSrc_v62x[0];
												ptrSpriteRenderSrc_v62x += v65[2];

												if ((x_BYTE)v63)
													*ptrSpriteRenderDest_v64 = x_BYTE_F6EE0_tablesx[v63];
												ptrSpriteRenderDest_v64 += v66x[1].dword_0;

												v65 += 4;
												v66x += 2;
												if (!--v67)
													break;
											LABEL_207:
												LOBYTE(v63) = ptrSpriteRenderSrc_v62x[0];
												ptrSpriteRenderSrc_v62x += *v65;

												if ((x_BYTE)v63)
													*ptrSpriteRenderDest_v64 = x_BYTE_F6EE0_tablesx[v63];
												ptrSpriteRenderDest_v64 += v66x->dword_0;
											}
											break;
										case 2:
											ptrSpriteRenderSrc_v68x = &v121x[*(x_DWORD*)(v155 + 4)];
											ptrSpriteRenderDest_v69 = (uint8_t*)(v169x->dword_1 + ptrRenderBuffer);
											v70 = (x_DWORD*)(8 * (v165[2] - v116) + &m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_1]);
											v71x = &m_str_F0E20x[*v165];
											HIWORD(v72) = 0;
											v73 = v133 >> 1;
											if (!(v133 & 1))
											{
												v70 = (x_DWORD*)(v155 + 8);
												v71x = &v169x[1];
												goto LABEL_217;
											}
											v73++;
											while (1)
											{
												HIBYTE(v72) = ptrSpriteRenderSrc_v68x[0];
												ptrSpriteRenderSrc_v68x += v70[2];

												if (HIBYTE(v72))
												{
													LOBYTE(v72) = *ptrSpriteRenderDest_v69;
													*ptrSpriteRenderDest_v69 = x_BYTE_F6EE0_tablesx[16384 + v72];
												}
												ptrSpriteRenderDest_v69 += v71x[1].dword_0;

												v70 += 4;
												v71x += 2;
												if (!--v73)
													break;
											LABEL_217:
												HIBYTE(v72) = ptrSpriteRenderSrc_v68x[0];
												ptrSpriteRenderSrc_v68x += *v70;

												if (HIBYTE(v72))
												{
													LOBYTE(v72) = *ptrSpriteRenderDest_v69;
													*ptrSpriteRenderDest_v69 = x_BYTE_F6EE0_tablesx[16384 + v72];
												}
												ptrSpriteRenderDest_v69 += v71x->dword_0;
											}
											break;
										case 3:
											ptrSpriteRenderSrc_v74x = &v121x[*(x_DWORD*)(v155 + 4)];
											ptrSpriteRenderDest_v75 = (uint8_t*)(v169x->dword_1 + ptrRenderBuffer);
											v76 = (x_DWORD*)(8 * (v165[2] - v116) + &m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_1]);//to position
											v77x = &m_str_F0E20x[v165[0]];//from position
											HIWORD(v78) = 0;
											v79 = v133 >> 1;
											if (!(v133 & 1))
											{
												v76 = (x_DWORD*)(v155 + 8);
												v77x = &v169x[1];
												goto LABEL_227;
											}
											v79++;
											while (1)
											{
												LOBYTE(v78) = ptrSpriteRenderSrc_v74x[0];
												ptrSpriteRenderSrc_v74x += v76[2];

												if ((x_BYTE)v78)
												{
													HIBYTE(v78) = *ptrSpriteRenderDest_v75;
													*ptrSpriteRenderDest_v75 = x_BYTE_F6EE0_tablesx[16384 + v78];
												}
												ptrSpriteRenderDest_v75 += v77x[1].dword_0;

												v76 += 4;
												v77x += 2;
												if (!--v79)
													break;
											LABEL_227:
												LOBYTE(v78) = ptrSpriteRenderSrc_v74x[0];
												ptrSpriteRenderSrc_v74x += *v76;

												if ((x_BYTE)v78)
												{
													HIBYTE(v78) = *ptrSpriteRenderDest_v75;
													*ptrSpriteRenderDest_v75 = x_BYTE_F6EE0_tablesx[16384 + v78];
												}
												ptrSpriteRenderDest_v75 += v77x->dword_0;
											}
											break;
										case 4:
											ptrSpriteRenderSrc_v80x = &v121x[*(x_DWORD*)(v155 + 4)];
											HIWORD(v84) = HIWORD(str_F2C20ar.dword0x07);
											ptrSpriteRenderDest_v81 = (uint8_t*)(v169x->dword_1 + ptrRenderBuffer);

											v82 = (x_DWORD*)(8 * (v165[2] - v116) + &m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_1]);
											v83x = &m_str_F0E20x[*v165];
											BYTE1(v84) = str_F2C20ar.dword0x07;
											v85 = v133 >> 1;
											if (!(v133 & 1))
											{
												v82 = (x_DWORD*)(v155 + 8);
												v83x = &v169x[1];
												goto LABEL_237;
											}
											v85++;
											while (1)
											{
												LOBYTE(v84) = ptrSpriteRenderSrc_v80x[0];
												ptrSpriteRenderSrc_v80x += v82[2];

												if ((x_BYTE)v84)
													*ptrSpriteRenderDest_v81 = x_BYTE_F6EE0_tablesx[16384 + v84];
												ptrSpriteRenderDest_v81 += v83x[1].dword_0;

												v82 += 4;
												v83x += 2;
												if (!--v85)
													break;
											LABEL_237:
												LOBYTE(v84) = ptrSpriteRenderSrc_v80x[0];
												ptrSpriteRenderSrc_v80x += *v82;

												if ((x_BYTE)v84)
													*ptrSpriteRenderDest_v81 = x_BYTE_F6EE0_tablesx[16384 + v84];
												ptrSpriteRenderDest_v81 += v83x->dword_0;
											}
											break;
										case 5:
											ptrSpriteRenderSrc_v86x = &v121x[*(x_DWORD*)(v155 + 4)];
											v87 = str_F2C20ar.dword0x07;
											ptrSpriteRenderDest_v88 = (uint8_t*)(v169x->dword_1 + ptrRenderBuffer);
											v89 = (x_DWORD*)(8 * (v165[2] - v116) + &m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_1]);
											v90x = &m_str_F0E20x[*v165];
											v91 = v133 >> 1;
											if (!(v133 & 1))
											{
												v89 = (x_DWORD*)(v155 + 8);
												v90x = &m_str_F0E20x[1];
												goto LABEL_247;
											}
											v91++;
											while (1)
											{
												BYTE1(v87) = ptrSpriteRenderSrc_v86x[0];
												ptrSpriteRenderSrc_v86x += v89[2];

												if (BYTE1(v87))
													*ptrSpriteRenderDest_v88 = x_BYTE_F6EE0_tablesx[16384 + v87];
												ptrSpriteRenderDest_v88 += v90x[1].dword_0;

												v89 += 4;
												v90x += 2;
												if (!--v91)
													break;
											LABEL_247:
												BYTE1(v87) = ptrSpriteRenderSrc_v86x[0];
												ptrSpriteRenderSrc_v86x += *v89;

												if (BYTE1(v87))
													*ptrSpriteRenderDest_v88 = x_BYTE_F6EE0_tablesx[16384 + v87];
												ptrSpriteRenderDest_v88 += v90x->dword_0;
											}
											break;
										case 6:
											v92 = str_F2C20ar.dword0x00;
											ptrSpriteRenderSrc_v93x = &v121x[*(x_DWORD*)(v155 + 4)];
											ptrSpriteRenderDest_v94 = (uint8_t*)(v169x->dword_1 + ptrRenderBuffer);

											HIWORD(v95) = 0;
											v96 = (x_DWORD*)(v155 + 8);
											v97x = &v169x[1];
											do
											{
												BYTE1(v95) = ptrSpriteRenderSrc_v93x[0];
												ptrSpriteRenderSrc_v93x += *v96;

												if (BYTE1(v95))
												{
													LOBYTE(v95) = *ptrSpriteRenderDest_v94;
													LOBYTE(v92) = x_BYTE_F6EE0_tablesx[16384 + v95];
													*ptrSpriteRenderDest_v94 = x_BYTE_F6EE0_tablesx[v92];
												}
												ptrSpriteRenderDest_v94 += v97x->dword_0;
												v96 += 2;
												v97x++;
												v133--;
											} while (v133);
											break;
										case 7:
											v98 = str_F2C20ar.dword0x00;
											ptrSpriteRenderSrc_v99x = &v121x[*(x_DWORD*)(v155 + 4)];
											ptrSpriteRenderDest_v100 = (uint8_t*)(v169x->dword_1 + ptrRenderBuffer);
											HIWORD(v101) = 0;
											v102 = (x_DWORD*)(v155 + 8);
											v103x = &v169x[1];
											do
											{
												LOBYTE(v101) = ptrSpriteRenderSrc_v99x[0];
												ptrSpriteRenderSrc_v99x += *v102;

												if ((x_BYTE)v101)
												{
													BYTE1(v101) = *ptrSpriteRenderDest_v100;
													LOBYTE(v98) = x_BYTE_F6EE0_tablesx[16384 + v101];
													*ptrSpriteRenderDest_v100 = x_BYTE_F6EE0_tablesx[v98];
												}
												ptrSpriteRenderDest_v100 += v103x->dword_0;
												v102 += 2;
												v103x++;
												v133--;
											} while (v133);
											break;
										case 8:
											ptrSpriteRenderSrc_v104x = &v121x[*(x_DWORD*)(v155 + 4)];
											v105 = str_F2C20ar.dword0x00;
											ptrSpriteRenderDest_v106 = (uint8_t*)(v169x->dword_1 + ptrRenderBuffer);
											v107 = (x_DWORD*)(8 * (v165[2] - v116) + &m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_1]);
											v108x = &m_str_F0E20x[*v165];
											v109 = v133 >> 1;
											if (!(v133 & 1))
											{
												v107 = (x_DWORD*)(v155 + 8);
												v108x = &v169x[1];
												goto LABEL_267;
											}
											v109++;
											while (1)
											{
												v111 = ptrSpriteRenderSrc_v104x[0];
												ptrSpriteRenderSrc_v104x += v107[2];

												if (v111)
												{
													LOBYTE(v105) = *ptrSpriteRenderDest_v106;
													*ptrSpriteRenderDest_v106 = x_BYTE_F6EE0_tablesx[v105];
												}
												ptrSpriteRenderDest_v106 += v108x[3].dword_0;
												v107 += 4;
												v108x += 2;
												if (!--v109)
													break;
											LABEL_267:
												v110 = ptrSpriteRenderSrc_v104x[0];
												ptrSpriteRenderSrc_v104x += *v107;
												if (v110)
												{
													LOBYTE(v105) = *ptrSpriteRenderDest_v106;
													*ptrSpriteRenderDest_v106 = x_BYTE_F6EE0_tablesx[v105];
												}
												ptrSpriteRenderDest_v106 += v108x->dword_0;
											}
											break;
										}
									}
								}
								str_F2C20ar.dword0x0a_actIdx += v135;
								ptrRenderBuffer += str_F2C20ar.dword0x23_stride;
								v165 += 3;
							}
						}
						break;
					case 7:
						v156 = str_F2C20ar.dword0x27 * str_F2C20ar.dword0x09_realWidth >> 16;
						if (v156 <= 0)
							break;
						scaledHeight = (str_F2C20ar.dword0x0c_realHeight << 16) / str_F2C20ar.dword0x27;
						if (scaledHeight <= 0)
							break;
						if (str_F2C20ar.dword0x04_screenY >= str_F2C20ar.height0x26)
							break;
						v135 = (str_F2C20ar.dword0x06_height << 16) / scaledHeight;
						if (a1 == 1)
						{
							str_F2C20ar.dword0x0a_actIdx = 0;
						}
						else
						{
							str_F2C20ar.dword0x0a_actIdx = (scaledHeight - 1) * v135;
							v135 = -v135;
						}
						v161 = str_F2C20ar.dword0x1b * str_F2C20ar.dword0x0c_realHeight / scaledHeight;
						v166 = (int*)(&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_2]);
						v158 = str_F2C20ar.dword0x04_screenY << 16;
						v114 = str_F2C20ar.dword0x1f * str_F2C20ar.dword0x04_screenY >> 16;
						v171 = str_F2C20ar.width0x25 - str_F2C20ar.dword0x03_screenX - v114;
						if (v171 < str_F2C20ar.width0x25)
						{
							if (v171 < str_F2C20ar.dword0x21)
								break;
						}
						else
						{
							v152 = v171 - str_F2C20ar.width0x25;
							scaledHeight -= v171 - str_F2C20ar.width0x25;
							if (scaledHeight <= 0)
								break;
							str_F2C20ar.dword0x0a_actIdx += v152 * v135;
							v158 += v152 * v161;
							v171 = str_F2C20ar.width0x25;
						}
						ptrRenderBuffer = iScreenWidth_DE560 * (str_F2C20ar.width0x25 - 1 - v171) + ViewPortRenderBufferStart_DE558;

					LABEL_285:
						v142 = str_F2C20ar.width0x25 - str_F2C20ar.dword0x21;
						if (v171 < str_F2C20ar.width0x25)
						{
							if (scaledHeight > v142)
								scaledHeight = str_F2C20ar.width0x25 - str_F2C20ar.dword0x21;
						}
						else if (v171 + scaledHeight - str_F2C20ar.width0x25 > v142)
						{
							scaledHeight = v142 - v171 + str_F2C20ar.width0x25;
							if (scaledHeight <= 0)
								break;
						}
						//adress 223d83
						v116 = 9999999;
						for (jy = scaledHeight; jy; jy--)
						{
							v174 = v158 >> 16;
							if (v158 >> 16 < 0)
							{
								v175 = -v174;
								v166[0] = 0;
								v166[1] = v156 - v175;
								v166[2] = v175;
								if (v175 < v116)
									v116 = v175;
							}
							else
							{
								v166[0] = v174;
								v166[1] = v156;
								v166[2] = 0;
								v116 = 0;
							}
							if (v166[1] + v166[0] > str_F2C20ar.height0x26)
								v166[1] = str_F2C20ar.height0x26 - v166[0];
							v158 += v161;
							v166 += 3;
						}
						v143 = v171 + str_F2C20ar.dword0x1d - str_F2C20ar.width0x25 + 1;
						if (v143 > 0)
						{
							int addressOffset = 12 * v143;
							v167 = (x_DWORD*)(addressOffset + &m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_2]);
							v119 = (int32_t*)str_F2C20ar.pbyte0x1a;
							for (k = v171 + str_F2C20ar.dword0x1d - str_F2C20ar.width0x25 + 2; k; k--)
							{
								v167 -= 3;
								if (&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_2] > (uint8_t*)v167)
									break;
								v144 = v167[1] + v167[0] - v119[0];
								if (v144 > 0)
								{
									v167[1] -= v144;
									if (v167[1] < 0)
										v167[1] = 0;
								}
								v119--;
							}
						}
						if (v171 - scaledHeight < 0)
						{
							v129 = scaledHeight - v171;
							v168 = (x_DWORD*)(12 * v171 + &m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_2]);
							v120 = (x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_3);
							v145 = v171 - scaledHeight - str_F2C20ar.dword0x21;
							if (v145 < 0)
							{
								scaledHeight = v171 - str_F2C20ar.dword0x21;
								if (v171 - str_F2C20ar.dword0x21 <= 0)
									break;
								v129 += v145;
							}
							v112 = v129;
							v130 = v129 - 1;
							if (v112 > 0)
							{
								while (v130)
								{
									v168 += 3;
									if (&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_2] <= (uint8_t*)v168)
									{
										v146 = *v120 - *v168;
										if (v146 > 0)
										{
											v168[0] = *v120;
											v168[1] -= v146;
											if (v168[1] < 0)
												v168[1] = 0;
											v168[2] += v146;
										}
									}
									v120++;
									v130--;
								}
							}
						}
						goto LABEL_172;
					}
				}
				if (a1 == 1)
				{
					if (!x_D41A0_BYTEARRAY_4_struct.byteindex_207
						&& str_F2C20ar.dword0x14x->class_0x3F_63 == 3
						&& (!str_F2C20ar.dword0x14x->model_0x40_64 || str_F2C20ar.dword0x14x->model_0x40_64 == 1))
					{
						DrawSorcererNameAndHealthBar_2CB30(str_F2C20ar.dword0x14x, str_F2C20ar.dword0x04_screenY, (int16_t)str_F2C20ar.dword0x03_screenX, str_F2C20ar.dword0x09_realWidth);
					}
					if (x_D41A0_BYTEARRAY_4_struct.showHelp_10)
						sub_88740(
							str_F2C20ar.dword0x14x,
							(int16_t)(str_F2C20ar.dword0x04_screenY + (str_F2C20ar.dword0x09_realWidth >> 1)),
							(int16_t)(str_F2C20ar.dword0x03_screenX + (str_F2C20ar.dword0x0c_realHeight >> 1)));
					if (str_F2C20ar.dword0x14x->struct_byte_0xc_12_15.byte[3] & 0x40)
					{
						str_F2C20ar.dword0x14x->word_0x2A_42 |= 0x40u;
					}
				}
				return;
			}
		}
		str_F2C20ar.dword0x04_screenY -= str_F2C20ar.cos_0x11 * str_F2C20ar.dword0x09_realWidth >> 17;
		str_F2C20ar.dword0x03_screenX -= -(str_F2C20ar.sin_0x0d * str_F2C20ar.dword0x09_realWidth) >> 17;
		goto LABEL_126;
	}

	//Draw Sprite to Render buffer
	v138 = (str_F2C20ar.dword0x0c_realHeight + str_F2C20ar.dword0x09_realWidth) >> 2;
	if (a1 >= 1)
	{
		if (a1 <= 1)
		{
			str_F2C20ar.dword0x04_screenY += -(str_F2C20ar.sin_0x0d * v138 >> 16) - v138;
			str_F2C20ar.dword0x03_screenX += -(str_F2C20ar.cos_0x11 * v138 >> 16) - v138;
		}
		else if (a1 == 2)
		{
			str_F2C20ar.dword0x04_screenY += (str_F2C20ar.sin_0x0d * v138 >> 16) - v138;
			str_F2C20ar.dword0x03_screenX += (str_F2C20ar.cos_0x11 * v138 >> 16) - v138;
		}
	}
	if ((uint16_t)viewPort.Width_DE564 > str_F2C20ar.dword0x04_screenY)
	{
		v136 = (str_F2C20ar.dword0x05 << 16) / str_F2C20ar.dword0x09_realWidth;
		if (-str_F2C20ar.dword0x04_screenY < 0 || str_F2C20ar.dword0x04_screenY == 0)
		{
			str_F2C20ar.dword0x0b = 0;
			if (str_F2C20ar.dword0x09_realWidth + str_F2C20ar.dword0x04_screenY - (uint16_t)viewPort.Width_DE564 > 0)
				str_F2C20ar.dword0x09_realWidth -= str_F2C20ar.dword0x09_realWidth + str_F2C20ar.dword0x04_screenY - (uint16_t)viewPort.Width_DE564;
		}
		else
		{
			str_F2C20ar.dword0x09_realWidth += str_F2C20ar.dword0x04_screenY;
			if (str_F2C20ar.dword0x09_realWidth <= 0)
				return;
			str_F2C20ar.dword0x0b = v136 * -str_F2C20ar.dword0x04_screenY;
			str_F2C20ar.dword0x04_screenY = 0;
			if ((uint16_t)viewPort.Width_DE564 <= str_F2C20ar.dword0x09_realWidth)
				str_F2C20ar.dword0x09_realWidth = (uint16_t)viewPort.Width_DE564;
		}
		if ((uint16_t)viewPort.Height_DE568 > str_F2C20ar.dword0x03_screenX)
		{
			v134 = (str_F2C20ar.dword0x06_height << 16) / str_F2C20ar.dword0x0c_realHeight;
			if (-str_F2C20ar.dword0x03_screenX < 0 || str_F2C20ar.dword0x03_screenX == 0)
			{
				str_F2C20ar.dword0x0a_actIdx = 0;
				if (str_F2C20ar.dword0x0c_realHeight + str_F2C20ar.dword0x03_screenX - (uint16_t)viewPort.Height_DE568 > 0)
					str_F2C20ar.dword0x0c_realHeight -= str_F2C20ar.dword0x0c_realHeight + str_F2C20ar.dword0x03_screenX - (uint16_t)viewPort.Height_DE568;
			}
			else
			{
				str_F2C20ar.dword0x0c_realHeight += str_F2C20ar.dword0x03_screenX;
				if (str_F2C20ar.dword0x0c_realHeight <= 0)
					return;
				str_F2C20ar.dword0x0a_actIdx = v134 * -str_F2C20ar.dword0x03_screenX;
				str_F2C20ar.dword0x03_screenX = 0;
				if ((uint16_t)viewPort.Height_DE568 <= str_F2C20ar.dword0x0c_realHeight)
					str_F2C20ar.dword0x0c_realHeight = (uint16_t)viewPort.Height_DE568;
			}
			v153 = (x_DWORD*)(&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_1]);
			for (l = str_F2C20ar.dword0x09_realWidth; l; l--)
			{
				v153[1] = str_F2C20ar.dword0x0b >> 16;
				if ((x_DWORD*)(&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_1]) == v153)
					*v153 = 22;
				else
					*v153 = v153[1] - *(v153 - 1);
				str_F2C20ar.dword0x0b += v136;
				v153 += 2;
			}
			if (a1 == 1 && x_D41A0_BYTEARRAY_4_struct.showHelp_10)
				sub_88740(
					str_F2C20ar.dword0x14x,
					(int16_t)(str_F2C20ar.dword0x04_screenY + (str_F2C20ar.dword0x09_realWidth >> 1)),
					(int16_t)(str_F2C20ar.dword0x03_screenX + (str_F2C20ar.dword0x0c_realHeight >> 1)));
			//v1 = (int)(x_DWORD_F2C2C * iScreenWidth_DE560 + x_DWORD_F2C30 + x_DWORD_DE558);

			//               screen-Y                                            screen-X
			v122x = &ViewPortRenderBufferStart_DE558[str_F2C20ar.dword0x03_screenX * iScreenWidth_DE560 + str_F2C20ar.dword0x04_screenY];
			//height
			while (str_F2C20ar.dword0x0c_realHeight)
			{
				if ((unsigned int)str_F2C20ar.dword0x01_rotIdx <= 7)
				{
					switch (str_F2C20ar.dword0x01_rotIdx)//mirroring
					{
					case 0:
						//               width                  actual line                   base adress                 add index
						//v2 = (char*)(str_F2C20ar.dword0x08_width * (str_F2C20ar.dword0x0a_actIdx >> 16) + (int)str_F2C20ar.dword0x02_data + *(x_DWORD*)(m_ptrDWORD_E9C38_smalltit + 36964));//sprite
						ptrSpriteRenderSrc_v2x = &str_F2C20ar.dword0x02_data[str_F2C20ar.dword0x08_width * (str_F2C20ar.dword0x0a_actIdx >> 16) + *(x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 + 4)];//sprite
						v3 = (x_DWORD*)&(m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_1]);
						v4 = v122x;//screen
						//   parametres
						v5 = str_F2C20ar.dword0x09_realWidth >> 1;
						if (!(str_F2C20ar.dword0x09_realWidth & 1))//bit 1
						{
							v6 = v5 & 1;//bit 2
							v7 = str_F2C20ar.dword0x09_realWidth >> 2;//bits 3-8 -> 1-6
							if (v6)
							{
								v7++;
								v3 = (x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 - 8);
								v4 = (uint8_t*)(v122x - 2);
								goto LABEL_49;
							}
							v3 = (x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 + 8);
							goto LABEL_45;
						}
						v8 = v5 + 2;
						v6 = v8 & 1;
						v7 = v8 >> 1;
						if (!v6)
						{
							v3 = (x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 - 16);
							v4 = (uint8_t*)(v122x - 3);
							goto LABEL_51;
						}
						v4 = (uint8_t*)(v122x - 1);
						while (1)
						{
							v10 = ptrSpriteRenderSrc_v2x[0];
							ptrSpriteRenderSrc_v2x += v3[2];

							if (v10)
								v4[1] = v10;
						LABEL_49:
							v11 = ptrSpriteRenderSrc_v2x[0];
							ptrSpriteRenderSrc_v2x += v3[4];

							if (v11)
								v4[2] = v11;
						LABEL_51:
							v12 = ptrSpriteRenderSrc_v2x[0];
							ptrSpriteRenderSrc_v2x += v3[6];

							if (v12)
								v4[3] = v12;
							v4 += 4;
							v3 += 8;
							if (!--v7)
								break;
						LABEL_45:
							v9 = ptrSpriteRenderSrc_v2x[0];
							ptrSpriteRenderSrc_v2x += v3[0];

							if (v9)//if not transparent pixel
								*v4 = v9;
						}
						break;
					case 1:
						v13 = str_F2C20ar.dword0x00;
						ptrSpriteRenderSrc_v14 = (x_BYTE*)(str_F2C20ar.dword0x08_width * (str_F2C20ar.dword0x0a_actIdx >> 16) + str_F2C20ar.dword0x02_data + *(x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 + 4));
						v15 = (x_DWORD*)(&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_1]);
						v16 = (uint8_t*)v122x;
						v17 = str_F2C20ar.dword0x09_realWidth >> 1;
						if (!(str_F2C20ar.dword0x09_realWidth & 1))
						{
							v15 = (x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 + 8);
							goto LABEL_58;
						}
						v17++;
						v16 = (uint8_t*)(v122x - 1);
						while (1)
						{
							LOBYTE(v13) = *ptrSpriteRenderSrc_v14;
							ptrSpriteRenderSrc_v14 += v15[2];

							if ((x_BYTE)v13)
								v16[1] = x_BYTE_F6EE0_tablesx[v13];
							v16 += 2;
							v15 += 4;
							if (!--v17)
								break;
						LABEL_58:
							LOBYTE(v13) = *ptrSpriteRenderSrc_v14;
							ptrSpriteRenderSrc_v14 += *v15;

							if ((x_BYTE)v13)
								*v16 = x_BYTE_F6EE0_tablesx[v13];
						}
						break;
					case 2:
						ptrSpriteRenderSrc_v18 = (x_BYTE*)(str_F2C20ar.dword0x08_width * (str_F2C20ar.dword0x0a_actIdx >> 16) + str_F2C20ar.dword0x02_data + *(x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 + 4));
						v19 = (x_DWORD*)(&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_1]);
						v20 = (uint8_t*)v122x;
						HIWORD(v21) = 0;
						v22 = str_F2C20ar.dword0x09_realWidth >> 1;
						if (!(str_F2C20ar.dword0x09_realWidth & 1))
						{
							v19 = (x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 + 8);
							goto LABEL_67;
						}
						v22++;
						v20 = (uint8_t*)(v122x - 1);
						while (1)
						{
							HIBYTE(v21) = *ptrSpriteRenderSrc_v18;
							ptrSpriteRenderSrc_v18 += v19[2];

							if (HIBYTE(v21))
							{
								LOBYTE(v21) = v20[1];
								v20[1] = x_BYTE_F6EE0_tablesx[16384 + v21];
							}
							v20 += 2;
							v19 += 4;
							if (!--v22)
								break;
						LABEL_67:
							HIBYTE(v21) = *ptrSpriteRenderSrc_v18;//zde
							ptrSpriteRenderSrc_v18 += *v19;

							if (HIBYTE(v21))
							{
								LOBYTE(v21) = *v20;
								v20[0] = x_BYTE_F6EE0_tablesx[16384 + v21];
							}
						}
						break;
					case 3:
						ptrSpriteRenderSrc_v23 = (x_BYTE*)(str_F2C20ar.dword0x08_width * (str_F2C20ar.dword0x0a_actIdx >> 16) + str_F2C20ar.dword0x02_data + *(x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 + 4));//from mask
						v24 = (x_DWORD*)(&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_1]);//from image
						v25 = v122x;//to adress
						HIWORD(v26) = 0;
						v27 = str_F2C20ar.dword0x09_realWidth >> 1;
						if (!(str_F2C20ar.dword0x09_realWidth & 1))
						{
							v24 = (x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 + 8);
							goto LABEL_76;
						}
						v27++;
						v25 = (uint8_t*)(v122x - 1);
						while (1)
						{
							LOBYTE(v26) = *ptrSpriteRenderSrc_v23;
							ptrSpriteRenderSrc_v23 += v24[2];

							if ((x_BYTE)v26)
							{
								HIBYTE(v26) = v25[1];
								v25[1] = x_BYTE_F6EE0_tablesx[16384 + v26];
							}
							v25 += 2;
							v24 += 4;
							if (!--v27)
								break;
						LABEL_76:
							LOBYTE(v26) = *ptrSpriteRenderSrc_v23;
							ptrSpriteRenderSrc_v23 += *v24;

							if ((x_BYTE)v26)
							{
								HIBYTE(v26) = v25[0];
								v25[0] = x_BYTE_F6EE0_tablesx[16384 + v26];
							}
						}
						break;
					case 4:
						HIWORD(v31) = HIWORD(str_F2C20ar.dword0x07);
						ptrSpriteRenderSrc_v28 = (x_BYTE*)(str_F2C20ar.dword0x08_width * (str_F2C20ar.dword0x0a_actIdx >> 16) + str_F2C20ar.dword0x02_data + *(x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 + 4));
						v29 = (x_DWORD*)(&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_1]);
						v30 = v122x;
						HIBYTE(v31) = str_F2C20ar.dword0x07;
						v32 = str_F2C20ar.dword0x09_realWidth >> 1;
						if (!(str_F2C20ar.dword0x09_realWidth & 1))
						{
							v29 = (x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 + 8);
							goto LABEL_85;
						}
						v32++;
						v30 = (uint8_t*)(v122x - 1);
						while (1)
						{
							LOBYTE(v31) = *ptrSpriteRenderSrc_v28;
							ptrSpriteRenderSrc_v28 += v29[2];

							if ((x_BYTE)v31)
								v30[1] = x_BYTE_F6EE0_tablesx[16384 + v31];
							v30 += 2;
							v29 += 4;
							if (!--v32)
								break;
						LABEL_85:
							LOBYTE(v31) = *ptrSpriteRenderSrc_v28;
							ptrSpriteRenderSrc_v28 += *v29;

							if ((x_BYTE)v31)
								*v30 = x_BYTE_F6EE0_tablesx[16384 + v31];
						}
						break;
					case 5:
						v33 = str_F2C20ar.dword0x07;
						ptrSpriteRenderSrc_v34 = (x_BYTE*)(str_F2C20ar.dword0x08_width * (str_F2C20ar.dword0x0a_actIdx >> 16) + str_F2C20ar.dword0x02_data + *(x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 + 4));
						v35 = (x_DWORD*)(&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_1]);
						v36 = v122x;
						v37 = str_F2C20ar.dword0x09_realWidth >> 1;
						if (!(str_F2C20ar.dword0x09_realWidth & 1))
						{
							v35 = (x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 + 8);
							goto LABEL_94;
						}
						v37++;
						v36 = (uint8_t*)(v122x - 1);
						while (1)
						{
							BYTE1(v33) = *ptrSpriteRenderSrc_v34;
							ptrSpriteRenderSrc_v34 += v35[2];

							if (BYTE1(v33))
								v36[1] = x_BYTE_F6EE0_tablesx[16384 + v33];
							v36 += 2;
							v35 += 4;
							if (!--v37)
								break;
						LABEL_94:
							BYTE1(v33) = *ptrSpriteRenderSrc_v34;
							ptrSpriteRenderSrc_v34 += *v35;

							if (BYTE1(v33))
								*v36 = x_BYTE_F6EE0_tablesx[16384 + v33];
						}
						break;
					case 6:
						v38 = str_F2C20ar.dword0x00;
						ptrSpriteRenderSrc_v39 = (x_BYTE*)(str_F2C20ar.dword0x08_width * (str_F2C20ar.dword0x0a_actIdx >> 16) + str_F2C20ar.dword0x02_data + *(x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 + 4));
						v40 = (x_DWORD*)(&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_1]);
						v41 = v122x;
						HIWORD(v42) = 0;
						v43 = str_F2C20ar.dword0x09_realWidth >> 1;
						if (!(str_F2C20ar.dword0x09_realWidth & 1))
						{
							v40 = (x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 + 8);
							goto LABEL_103;
						}
						v43++;
						v41 = (uint8_t*)(v122x - 1);
						while (1)
						{
							BYTE1(v42) = *ptrSpriteRenderSrc_v39;
							ptrSpriteRenderSrc_v39 += v40[2];

							if (BYTE1(v42))
							{
								LOBYTE(v42) = v41[1];
								LOBYTE(v38) = x_BYTE_F6EE0_tablesx[16384 + v42];
								v41[1] = x_BYTE_F6EE0_tablesx[v38];
							}
							v41 += 2;
							v40 += 4;
							if (!--v43)
								break;
						LABEL_103:
							BYTE1(v42) = *ptrSpriteRenderSrc_v39;
							ptrSpriteRenderSrc_v39 += *v40;

							if (BYTE1(v42))
							{
								LOBYTE(v42) = *v41;
								LOBYTE(v38) = x_BYTE_F6EE0_tablesx[16384 + v42];
								*v41 = x_BYTE_F6EE0_tablesx[v38];
							}
						}
						break;
					case 7:
						v44 = str_F2C20ar.dword0x00;
						ptrSpriteRenderSrc_v45 = (x_BYTE*)(str_F2C20ar.dword0x08_width * (str_F2C20ar.dword0x0a_actIdx >> 16) + str_F2C20ar.dword0x02_data + *(x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 + 4));
						v46 = (x_DWORD*)(&m_ptrDWORD_E9C38_smalltit[m_bufferOffset_E9C38_1]);
						v47 = v122x;
						HIWORD(v48) = 0;
						v49 = str_F2C20ar.dword0x09_realWidth >> 1;
						if (!(str_F2C20ar.dword0x09_realWidth & 1))
						{
							v46 = (x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 + 8);
							goto LABEL_112;
						}
						v49++;
						v47 = (uint8_t*)(v122x - 1);
						while (1)
						{
							LOBYTE(v48) = *ptrSpriteRenderSrc_v45;
							ptrSpriteRenderSrc_v45 += v46[2];

							if ((x_BYTE)v48)
							{
								BYTE1(v48) = v47[1];
								LOBYTE(v44) = x_BYTE_F6EE0_tablesx[16384 + v48];
								v47[1] = x_BYTE_F6EE0_tablesx[v44];
							}
							v47 += 2;
							v46 += 4;
							if (!--v49)
								break;
						LABEL_112:
							LOBYTE(v48) = *ptrSpriteRenderSrc_v45;
							ptrSpriteRenderSrc_v45 += *v46;

							if ((x_BYTE)v48)
							{
								BYTE1(v48) = *v47;
								LOBYTE(v44) = x_BYTE_F6EE0_tablesx[16384 + v48];
								*v47 = x_BYTE_F6EE0_tablesx[v44];
							}
						}
						break;
					}
				}
				str_F2C20ar.dword0x0a_actIdx += v134;
				v122x += iScreenWidth_DE560;
				str_F2C20ar.dword0x0c_realHeight--;
			}
		}
	}
}


void DrawPolygonRasterLine_single_color_subB6253(
	Rasterline_t *pRasterLines,
	uint8_t startLine, uint8_t drawEveryNthLine, int linesToDraw, 
	uint8_t **ptrRenderBufferStartOfCurrentLine_v1102, char local_x_BYTE_E126C)
{
	Rasterline_t* next_raster_line = pRasterLines;

	uint8_t* v170 = *ptrRenderBufferStartOfCurrentLine_v1102;

	char v171 = local_x_BYTE_E126C;

	uint8_t v18;
	int v172;
	signed int v173;

	uint8_t* v174;

	uint8_t line1 = startLine;

	HIWORD(v172) = 0;
	if (CommandLineParams.DoTestRenderers()) { renderer_tests_register_hit(RendererTestsHitCheckpoint::RendTest_HD_Draw_Rasterline_SingleColor); }
	while (1)
	{
		LOWORD(v172) = HIWORD(next_raster_line->startX);
		v173 = HIWORD(next_raster_line->endX);
		v170 += iScreenWidth_DE560;
		line1++;
		if (line1 >= drawEveryNthLine)
		{
			line1 = startLine;
			if ((v172 & 0x8000u) == 0)
				break;
			if ((signed __int16)v173 > 0)
			{
				if (v173 > viewPort.Width_DE564)
					v173 = viewPort.Width_DE564;
				v174 = v170;
			LABEL_328:
				memset(v174, v171, v173);
			}
		}
		LABEL_329:
			next_raster_line++;
		if (!--linesToDraw)
			return;
	}
	if (v173 > viewPort.Width_DE564)
		v173 = viewPort.Width_DE564;
	v18 = __OFSUB__((x_WORD)v173, (x_WORD)v172);
	LOWORD(v173) = v173 - v172;
	if ((uint8_t)(((v173 & 0x8000u) != 0) ^ v18) | ((x_WORD)v173 == 0))
		goto LABEL_329;
	v174 = &v170[v172];
	goto LABEL_328;
}


void DrawPolygonRasterLine_subB6253(
	Rasterline_t *pRasterLines,
	uint8_t startLine, uint8_t drawEveryNthLine, int linesToDraw, 
	uint8_t **ptrViewPortRenderLineStart_v1102,
	uint32_t Vincrement, int Uincrement, uint32_t BrightnessIncrement_v1146,
	const uint8_t *pTexture) 
{
	Rasterline_t* next_raster_line = pRasterLines;
	Rasterline_t* current_raster_line;

	uint8_t line6 = startLine;

	uint32_t fixedpointVincrement = Vincrement << 16;

	uint8_t v18;
	uint8_t v180;
	uint16_t startX_v375;
	uint16_t paletteMapping;
	int16_t textureIndexU = 0;
	int16_t textureIndexV = 0;
	uint16_t endX_v378;
	uint8_t* ptrViewPortRenderLine_v379; // pixel position in screen buffer
	uint16_t v380;
	unsigned int v382;
	int v383;
	int32_t v384tmp;
	int16_t pixelCount_v384lo;
	int16_t BrightnessFractionalPart_v384hi;
	int16_t v385;
	uint8_t* currentPixel;

	const int16_t MAX_TEXTURE_INDEX = x_BYTE_D41B5_texture_size-1;

	if (CommandLineParams.DoTestRenderers()) { renderer_tests_register_hit(RendererTestsHitCheckpoint::RendTest_HD_Draw_Rasterline_Standard); }
	do
	{
		current_raster_line = next_raster_line;
		next_raster_line++;
		startX_v375 = HIWORD(current_raster_line->startX);
		endX_v378 = HIWORD(current_raster_line->endX);
		ptrViewPortRenderLine_v379 = iScreenWidth_DE560 + *ptrViewPortRenderLineStart_v1102;
		*ptrViewPortRenderLineStart_v1102 += iScreenWidth_DE560;
		line6++;

		if (line6 >= drawEveryNthLine)
		{
			line6 = 0;
			if ((startX_v375 & 0x8000u) == 0) {
				// startX >= 0
				if (endX_v378 > viewPort.Width_DE564)
					endX_v378 = viewPort.Width_DE564;

				v18 = __OFSUB__(endX_v378, startX_v375);
				v385 = endX_v378 - startX_v375;
				if ((uint8_t)((v385 < 0) ^ v18) | (v385 == 0)) {
					continue;
				}
				ptrViewPortRenderLine_v379 += startX_v375;
				textureIndexU = BYTE2(current_raster_line->U);
				v383 = __SWAP_HILOWORD__(current_raster_line->V);
				textureIndexV = (uint8_t)v383;
				LOWORD(v383) = LOWORD(current_raster_line->U);

				v384tmp = __SWAP_HILOWORD__(current_raster_line->brightness);
				BrightnessFractionalPart_v384hi = HIWORD(v384tmp);
				BYTE1(paletteMapping) = LOWORD(v384tmp);
				pixelCount_v384lo = v385;
			}
			else if (endX_v378 > 0)
			{
				// startX_v375 is negative here, but endX is positive -> skip pixels by updating v,u,brightness
				v380 = -startX_v375;
				v383 = __SWAP_HILOWORD__(current_raster_line->V + Vincrement * v380);
				textureIndexV = (uint8_t)v383;
				v382 = current_raster_line->U + Uincrement * v380;
				LOWORD(v383) = v382;
				startX_v375 = v382 >> 8;
				textureIndexU = BYTE2(v382);

				v384tmp = __SWAP_HILOWORD__(current_raster_line->brightness + BrightnessIncrement_v1146 * v380);
				BrightnessFractionalPart_v384hi = HIWORD(v384tmp);
				BYTE1(paletteMapping) = LOWORD(v384tmp);
				pixelCount_v384lo = HIWORD(current_raster_line->endX);

				if (pixelCount_v384lo > (int16_t)viewPort.Width_DE564) {
					pixelCount_v384lo = viewPort.Width_DE564;
				}
			}
			else {
				// endX is <= 0, skip the whole raster line
				continue;
			}

			currentPixel = &ptrViewPortRenderLine_v379[0];
			do {
				if (textureIndexV > MAX_TEXTURE_INDEX)
					break;

				uint16_t textureIndex = (uint16_t)(textureIndexV << 8) | textureIndexU;
				LOBYTE(paletteMapping) = pTexture[textureIndex];

				v180 = __CFADD__((x_WORD)Uincrement, (x_WORD)v383);
				LOWORD(v383) = Uincrement + v383;
				textureIndexU = (int8_t)BYTE2(Uincrement) + textureIndexU + v180;

				v180 = __CFADD__(fixedpointVincrement, v383);
				v383 += fixedpointVincrement;
				textureIndexV = (int8_t)BYTE2(Vincrement) + textureIndexV + v180;

				currentPixel[0] = x_BYTE_F6EE0_tablesx[paletteMapping];

				v180 = __CFADD__(LOWORD(BrightnessIncrement_v1146), BrightnessFractionalPart_v384hi);
				BrightnessFractionalPart_v384hi += BrightnessIncrement_v1146;
				paletteMapping = GameRenderHD::SumByte1WithByte2(paletteMapping, BrightnessIncrement_v1146, v180);

				currentPixel += 1;
			} while (--pixelCount_v384lo > 0);
		}
	} while(--linesToDraw);
}


void DrawPolygonRasterLine_flat_shading_subB6253(
	Rasterline_t *pRasterLines,
	uint8_t startLine, uint8_t drawEveryNthLine, int linesToDraw,
	uint8_t **pv1102,
	uint32_t Vincrement, int Uincrement,
	uint8_t *pTexture, char local_x_BYTE_E126C) 
{
	// flat shading and reflections enabled
	Rasterline_t *next_raster_line = pRasterLines;
	Rasterline_t *current_raster_line;

	uint8_t line8 = startLine;

	uint32_t v1169 = Vincrement << 16;

	uint8_t v18;
	uint8_t v180;
	int16_t startX_v406;
	uint16_t paletteMapping;
	uint16_t textureIndexU = 0;
	uint16_t textureIndexV = 0;
	int16_t endX_v408;
	uint8_t* currentPixel;
	int v410;
	unsigned int v411;
	int v412;
	unsigned int v413;

	Rasterline_t *v1278;

	const int16_t MAX_TEXTURE_INDEX = x_BYTE_D41B5_texture_size-1;

	if (CommandLineParams.DoTestRenderers()) { renderer_tests_register_hit(RendererTestsHitCheckpoint::RendTest_HD_Draw_Rasterline_Flatshading); }
	do {
		current_raster_line = next_raster_line;
		next_raster_line++;

		startX_v406 = HIWORD(current_raster_line->startX);
		endX_v408 = HIWORD(current_raster_line->endX);
		currentPixel = iScreenWidth_DE560 + *pv1102;
		*pv1102 += iScreenWidth_DE560;
		line8++;

		if (line8 >= drawEveryNthLine)
		{
			line8 = 0;
			if ((startX_v406 & 0x8000u) == 0) {
				if (endX_v408 > viewPort.Width_DE564)
					endX_v408 = viewPort.Width_DE564;

				v18 = __OFSUB__((x_WORD)endX_v408, (x_WORD)startX_v406);
				endX_v408 = endX_v408 - startX_v406;
				if ((uint8_t)(((endX_v408 & 0x8000u) != 0) ^ v18) | (endX_v408 == 0)) {
					continue;
				}
				currentPixel += startX_v406;

				v412 = __SWAP_HILOWORD__(current_raster_line->V);
				textureIndexV = (uint8_t)v412;

				LOWORD(v412) = LOWORD(current_raster_line->U);
				textureIndexU = BYTE2(current_raster_line->U);
			}
			else if (endX_v408 > 0)
			{
				v410 = -startX_v406;

				v412 = __SWAP_HILOWORD__(current_raster_line->V + Vincrement * v410);
				textureIndexV = (uint8_t)v412;

				v411 = current_raster_line->U + Uincrement * v410;
				LOWORD(v412) = v411;
				v413 = v411 >> 8;
				textureIndexU = BYTE1(v413);

				if (endX_v408 > viewPort.Width_DE564)
					endX_v408 = viewPort.Width_DE564;
				startX_v406 = (uint16_t)v413;
			}
			else
			{
				continue;
			}

			v1278 = current_raster_line;
			BYTE1(paletteMapping) = local_x_BYTE_E126C;
			do {
				if (textureIndexV > MAX_TEXTURE_INDEX)
					break;

				uint16_t textureIndex = (uint16_t)(textureIndexV << 8) | textureIndexU;
				LOBYTE(paletteMapping) = pTexture[textureIndex];

				v180 = __CFADD__((x_WORD)Uincrement, (x_WORD)v412);
				LOWORD(v412) = Uincrement + v412;
				textureIndexU = (int8_t)BYTE2(Uincrement) + textureIndexU + v180;

				v180 = __CFADD__(v1169, v412);
				v412 = v1169 + v412;
				textureIndexV = (int8_t)BYTE2(Vincrement) + textureIndexV + v180;

				*currentPixel = x_BYTE_F6EE0_tablesx[paletteMapping];
				currentPixel += 1;
			} while(--endX_v408);
			current_raster_line = v1278;
		}
	} while(--linesToDraw);
}


void DrawPolygonRasterLine_reflections_subB6253(
	Rasterline_t *pRasterLines,
	uint8_t startLine, uint8_t drawEveryNthLine, int linesToDraw,
	uint8_t **pv1102,
	uint32_t Vincrement, int Uincrement,
	uint32_t BrightnessIncrement,
	uint8_t *pTexture 
	)
{
	Rasterline_t *next_raster_line = pRasterLines;
	Rasterline_t *current_raster_line;
	uint8_t line25 = startLine;

	int VincrementFixedPoint = Vincrement << 16;
	int v1189 = BrightnessIncrement << 16;

	uint8_t v18;
	uint8_t v180;
	int16_t startX;
	uint16_t paletteMapping;
	int16_t textureIndexU = 0;
	int16_t textureIndexV = 0;
	int16_t endX;
	uint8_t* currentPixel;
	uint32_t v1053;
	int v1054;
	char v1056;
	int pixelCount;

	const int16_t MAX_TEXTURE_INDEX = x_BYTE_D41B5_texture_size-1;

	if (CommandLineParams.DoTestRenderers()) { renderer_tests_register_hit(RendererTestsHitCheckpoint::RendTest_HD_Draw_Rasterline_Reflections); }
	do {
		current_raster_line = next_raster_line;
		next_raster_line++;

		startX = HIWORD(current_raster_line->startX);
		endX = HIWORD(current_raster_line->endX);
		currentPixel = (uint8_t*)(iScreenWidth_DE560 + *pv1102);
		*pv1102 += iScreenWidth_DE560;

		line25++;
		if (line25 >= drawEveryNthLine)
		{
			line25 = 0;
			if (startX >= 0) {
				if (endX > viewPort.Width_DE564)
					endX = viewPort.Width_DE564;
				v18 = __OFSUB__(endX, startX);
				endX = endX - startX;
				if ((uint8_t)(((endX & 0x8000u) != 0) ^ v18) | (endX == 0)) {
					continue;
				}
				currentPixel += startX;

				v1053 = __SWAP_HILOWORD__(current_raster_line->V);
				textureIndexV = BYTE2(current_raster_line->V);

				LOWORD(v1053) = LOWORD(current_raster_line->U);
				textureIndexU = BYTE2(current_raster_line->U);

				pixelCount = endX;
				v1054 = __SWAP_HILOWORD__(current_raster_line->brightness);
			}
			else if (endX > 0)
			{
				if (endX > viewPort.Width_DE564)
					endX = viewPort.Width_DE564;
				pixelCount = endX;
				const int v1050 = (uint16_t)-startX;

				v1053 = __SWAP_HILOWORD__(current_raster_line->V + Vincrement * v1050);
				textureIndexV = (uint8_t)v1053;

				const unsigned int v1052 = current_raster_line->U + Uincrement * v1050;
				LOWORD(v1053) = v1052;
				textureIndexU = BYTE2(v1052);

				v1054 = __SWAP_HILOWORD__(current_raster_line->brightness + BrightnessIncrement * v1050);
			}
			else
			{
				// nothing to draw
				continue;
			}

			do {
				if (textureIndexV > MAX_TEXTURE_INDEX)
					break;

				uint16_t textureIndex = (uint16_t)(textureIndexV << 8) | textureIndexU;
				LOBYTE(paletteMapping) = pTexture[textureIndex];

				v180 = __CFADD__((x_WORD)Uincrement, (x_WORD)v1053);
				LOWORD(v1053) = Uincrement + v1053;
				BYTE1(paletteMapping) = v1054;
				textureIndexU = (int8_t)BYTE2(Uincrement) + v180 + textureIndexU;

				if ((uint8_t)paletteMapping >= 0xCu)
				{
					v1056 = x_BYTE_F6EE0_tablesx[paletteMapping]; // Fixme: x_BYTE_F6EE0_tablesx should be passed as a parameter
				}
				else
				{
					LOBYTE(paletteMapping) = x_BYTE_F6EE0_tablesx[paletteMapping];
					BYTE1(paletteMapping) = *currentPixel;
					v1056 = x_BYTE_F6EE0_tablesx[16384 + paletteMapping];
				}

				v180 = __CFADD__(VincrementFixedPoint, v1053);
				v1053 = VincrementFixedPoint + v1053;
				textureIndexV = (int8_t)BYTE2(Vincrement) + v180 + textureIndexV;

				v180 = __CFADD__(v1189, v1054);
				v1054 = v1189 + v1054;
				*currentPixel = v1056;
				LOBYTE(v1054) = BYTE2(BrightnessIncrement) + v180 + v1054;

				currentPixel += 1;
			} while (--pixelCount > 0);
		}
	} while(--linesToDraw > 0);
}


void GameRenderHD::DrawTriangleInProjectionSpace_B6253(const ProjectionPolygon* vertex1, const ProjectionPolygon* vertex2, const ProjectionPolygon* vertex3, uint8_t startLine, uint8_t drawEveryNthLine)
{
	const ProjectionPolygon* vert_y_low_v3; // esi
	const ProjectionPolygon* vert_y_middle_v4; // edi
	const ProjectionPolygon* vert_y_high_v5; // ecx
	int y1; // eax
	int y2; // ebx
	int y3; // edx
	int v14; // eax
	int v15; // ebx
	bool v16; // zf
	bool v17; // sf
	uint8_t v18; // of
	uint8_t* renderBufferStartOfCurrentLine; // [esp+0h] [ebp-88h]
	int linesToDraw_v1123; // [esp+20h] [ebp-68h]
	int Uincrement; // [esp+24h] [ebp-64h]
	uint32_t Vincrement; // [esp+30h] [ebp-58h]
	uint32_t BrightnessIncrement = 0xAAAAAAAA; // [esp+3Ch] [ebp-4Ch]

	Uincrement = 0;
	Vincrement = 0;

	// NOTE: vert_y_high does not neccessarily mean that it is the vertex with the highest y value.
	//       It means that the raster lines are drawn from vert_y_low_v3 to vert_y_high.
	//       vert_y_middle_v4 has a y value >= very_y_low
	//       The conditions in the code below sort the triangles and re-assign vert_y_xxx.
	//       Triangles whith counter-clock-wise vertices are culled and not drawn.
	vert_y_low_v3 = vertex1;
	vert_y_middle_v4 = vertex2;
	vert_y_high_v5 = vertex3;
	y1 = vertex1->Y;
	y2 = vertex2->Y;
	y3 = vertex3->Y;

#ifdef _MSC_VER
	int maxx = max(vertex1->X, max(vertex2->X, vertex3->X));
	int minx = min(vertex1->X, min(vertex2->X, vertex3->X));
	int maxy = max(y1, __max(y2, y3));
	int miny = min(y1, __min(y2, y3));
#else
	int maxx = std::max(vertex1->X, std::max(vertex2->X, vertex3->X));
	int minx = std::min(vertex1->X, std::min(vertex2->X, vertex3->X));
	int maxy = std::max(y1, std::max(y2, y3));
	int miny = std::min(y1, std::min(y2, y3));
#endif

	if (maxx < 0 || minx >= viewPort.Width_DE564 || maxy < 0 || miny >= viewPort.Height_DE568) {
		// triangle is outside of the viewport
		return;
	}

	if (y1 == y2)
	{
		if (y1 == y3) {
			// all vertices are on the same line -> nothing to draw
			return;
		}
		if (y1 >= y3)
		{
			// y3 <= y1 == y2
			if (vertex1->X <= vertex2->X) {
				//       vertex3
				//       |     |
				// vertex1 --- vertex2 
				return; // face culling
			}
			vert_y_low_v3 = vertex3;
			vert_y_middle_v4 = vertex1;
			vert_y_high_v5 = vertex2;
			//       vertex3
			//       |     |
			// vertex2 --- vertex1 
			goto LABEL_277_PrepareRasterlineForTriangleWithHorizontalBottom;
		}
		if (vertex2->X <= vertex1->X) {
			// vertex2 --- vertex1 
			//       |     |
			//       vertex3
			return; // face culling
		}
		// vertex1 --- vertex2 
		//       |     |
		//       vertex3
		goto LABEL_277_PrepareRasterlineForTriangleWithHorizontalTop;
	}

	if (y1 <= y2)
	{
		if (y1 != y3)
		{
			if (y1 >= y3)
			{
				// y3 < y1 <= y2

				vert_y_low_v3 = vertex3;
				vert_y_middle_v4 = vertex1;
				vert_y_high_v5 = vertex2;

				goto LABEL_24_DrawTriangle;

				return;
			}
			if (y2 != y3)
			{
				if (y2 <= y3) {
					// y1 <= y2 < y3
					goto LABEL_24_DrawTriangle;
				}
				// y1 < y3 < y2
				goto LABEL_129_DrawTriangle;
			}
			if (vertex2->X <= vertex3->X) {
				//       vertex1
				//       |     |
				// vertex2 --- vertex3
				return; // face culling
			}

			goto LABEL_277_PrepareRasterlineForTriangleWithHorizontalBottom;

		}

		// y3 == y1 <= y2

		if (vertex1->X <= vertex3->X) {
			// vertex1 --- vertex3 
			//       |     |
			//       vertex2
			return; // face culling
		}

		// vertex3 --- vertex1
		//       |     |
		//       vertex2
		vert_y_low_v3 = vertex3;
		vert_y_middle_v4 = vertex1;
		vert_y_high_v5 = vertex2;

		goto LABEL_277_PrepareRasterlineForTriangleWithHorizontalTop;
	}

	// here y1 > y2, because all the switch cases above leave with a goto

	if (y1 == y3)
	{
		// y2 < y1 == y3
		if (vertex3->X <= vertex1->X) {
			//       vertex2
			//       |     |
			// vertex3 --- vertex1 
			return; // face culling
		}
		vert_y_low_v3 = vertex2;
		vert_y_middle_v4 = vertex3;
		vert_y_high_v5 = vertex1;
		goto LABEL_277_PrepareRasterlineForTriangleWithHorizontalBottom;
	}
	if (y1 < y3)
	{
		// y2 < y1 < y3
		vert_y_low_v3 = vertex2;
		vert_y_middle_v4 = vertex3; //?
		vert_y_high_v5 = vertex1; //?
		goto LABEL_129_DrawTriangle;
	}
	if (y2 == y3)
	{
		// y2 == y3 < y1
		if (vertex3->X <= vertex2->X) {
			// vertex3 --- vertex2 
			//       |     |
			//       vertex1
			return; // face culling
		}
		vert_y_low_v3 = vertex2;
		vert_y_middle_v4 = vertex3;
		vert_y_high_v5 = vertex1;
		goto LABEL_277_PrepareRasterlineForTriangleWithHorizontalTop;
	}
	if (y2 < y3)
	{
		// y2 < y1 and y2 < y3 (and not y1 < y3 - checked above)
		// => y2 < y3 < y1
		vert_y_low_v3 = vertex2;
		vert_y_middle_v4 = vertex3;
		vert_y_high_v5 = vertex1;
		goto LABEL_24_DrawTriangle;
	}

	// y3 < y2 < y1
	vert_y_low_v3 = vertex3;
	vert_y_middle_v4 = vertex1;
	vert_y_high_v5 = vertex2;

LABEL_129_DrawTriangle:
	// general case where
	//
	// vertex_low                          vertex_low
	//  |       |                          |        |
	//  |       vertex_high  or  vertex_high        |     
	//  |       |                          |        |
	// vertex_middle                       vertex_middle
	// 
	// Only the _right_ triangle is clock-wise, the left case will be culled by
	// making sure that the slope low-middle is greater than the slope low-high.

	{
		int v69;
		int v70;
		int v72;
		Rasterline_t *v81;
		int v84;
		int v85;
		bool v86;
		int v87;
		int v89;
		int v90;
		int v91;
		int v92;
		int v93;
		int v94;
		Rasterline_t *v95;
		int v110;
		int v111;
		int v113;
		Rasterline_t *v114;
		int v1128;
		int v1134;
		int v1139;
		int v1145;
		int v1162;
		int v1164;
		bool vertYlowIsNegative;

		const int vertLowY_v1190 = vert_y_low_v3->Y;
		if (vertLowY_v1190 >= 0)
		{
			if (vertLowY_v1190 >= viewPort.Height_DE568) {
				// even lowest y coordinate is outside of the viewport -> cull triangle
				return;
			}
			renderBufferStartOfCurrentLine = ViewPortRenderBufferAltStart_DE554 + iScreenWidth_DE560 * vertLowY_v1190;
			vertYlowIsNegative = false;
		}
		else
		{
			renderBufferStartOfCurrentLine = ViewPortRenderBufferAltStart_DE554;
			vertYlowIsNegative = true;
		}
		const int vertHighY_v66 = vert_y_high_v5->Y;
		bool v1297 = vertHighY_v66 > viewPort.Height_DE568;
		int v1114 = vertHighY_v66 - vertLowY_v1190;
		const int v67 = vert_y_middle_v4->Y;
		const bool v1301 = v67 > viewPort.Height_DE568;
		const int v68 = v67 - vertLowY_v1190;
		const int v1118 = v68;
		linesToDraw_v1123 = v68;
		const int fp_slope_HighLowVert = ((vert_y_high_v5->X - vert_y_low_v3->X) << 16) / v1114;

		// only draw triangle with clock-wise vertices by comparing the slopes
		if (((vert_y_middle_v4->X - vert_y_low_v3->X) << 16) / v68 > fp_slope_HighLowVert)
		{
			//           vertex_low
			//           |        |
			// vertex_high        |     
			//           |        |
			//           vertex_middle
			const int v1108 = ((vert_y_middle_v4->X - vert_y_low_v3->X) << 16) / v68;
			const int v1112 = ((vert_y_middle_v4->X - vert_y_high_v5->X) << 16) / (vert_y_middle_v4->Y - vert_y_high_v5->Y);
			int v1120 = vert_y_middle_v4->Y - vert_y_high_v5->Y;
			const int fp_vertHighX = vert_y_high_v5->X << 16;
			switch (x_BYTE_E126D)
			{
			case 0:
			case 0xE:
			case 0xF:
				// debug shading - single color
				v110 = vert_y_low_v3->X << 16;
				v111 = vert_y_low_v3->X << 16;
				if (vertYlowIsNegative)
				{
					// clip triangle for negative y, top of the viewport
					v18 = __OFSUB__(linesToDraw_v1123, -vertLowY_v1190);
					v16 = linesToDraw_v1123 == -vertLowY_v1190;
					v17 = linesToDraw_v1123 + vertLowY_v1190 < 0;
					linesToDraw_v1123 += vertLowY_v1190;
					if ((uint8_t)(v17 ^ v18) | (uint8_t)v16)
						return;
					v1164 = -vertLowY_v1190;
					if (-vertLowY_v1190 - v1114 >= 0)
					{
						const int v112 = v1164 - v1114;
						v1120 -= v112;
						v113 = v1112 * v112 + fp_vertHighX;
						v111 += v112 * v1108 + v1114 * v1108;
						if (v1301)
						{
							// middle vertex y > viewport
							v1120 = viewPort.Height_DE568;
							linesToDraw_v1123 = viewPort.Height_DE568;
						}
						v114 = &rasterlines_DE56Cx[startLine][0];
						goto LABEL_228_DrawTriangle;
					}
					v1114 += vertLowY_v1190; // subtract -y from number of lines
					v110 += fp_slope_HighLowVert * v1164; // compute start x when entering viewport
					v111 += v1164 * v1108; // compute end x when entering viewport
					if (v1301)
					{
						// high vertex y > viewport
						linesToDraw_v1123 = viewPort.Height_DE568;
						if (v1297)
						{
							v1114 = viewPort.Height_DE568;
						}
						else
						{
							v1297 = viewPort.Height_DE568 <= v1114;
							v1120 = viewPort.Height_DE568 - v1114;
						}
					}
				}
				else if (v1301)
				{
					const int v115 = viewPort.Height_DE568 - vertLowY_v1190;
					linesToDraw_v1123 = viewPort.Height_DE568 - vertLowY_v1190;
					if (v1297)
					{
						v1114 = viewPort.Height_DE568 - vertLowY_v1190;
					}
					else
					{
						v18 = __OFSUB__(v115, v1114);
						const int v116 = v115 - v1114;
						v1297 = (v116 < 0) ^ v18 | (v116 == 0);
						v1120 = v116;
					}
				}
				v114 = RasterizePolygon(&rasterlines_DE56Cx[startLine][0], &v110, &v111, fp_slope_HighLowVert, v1108, &v1114);
				v113 = fp_vertHighX;
			LABEL_228_DrawTriangle:
				if (v1297)
				{
				}
				else
				{
					v114 = RasterizePolygon(v114, &v113, &v111, v1112, v1108, &v1120);
				}
				goto LABEL_DrawRasterLines;
			case 1:
			case 4:
			case 0x10:
			case 0x11:
				Logger->error("!!! Unsupported polygon mode !!!");
			case 2:
			case 3:
			case 7:
			case 8:
			case 9:
			case 0xA:
			case 0xB:
			case 0xC:
			case 0xD:
			case 0x12:
			case 0x13:
			case 0x16:
			case 0x17:
				// flat shading
				v84 = v1114 * (signed __int64)(vert_y_middle_v4->X - vert_y_low_v3->X) / v68;
				v85 = vert_y_low_v3->X - vert_y_high_v5->X;
				v18 = __OFADD__(v84, v85);
				v86 = v84 + v85 == 0;
				v17 = v84 + v85 < 0;
				v87 = v84 + v85;
				if ((uint8_t)v17 ^ v18)
					return;
				if (!v86)
				{
					const int v88 = v87 + 1;
					Uincrement = (signed int)(vert_y_low_v3->U + (unsigned __int64)(v1114 * (signed __int64)(vert_y_middle_v4->U - vert_y_low_v3->U) / v1118) - vert_y_high_v5->U)
						/ v88;
					Vincrement = (signed int)(vert_y_low_v3->V + (unsigned __int64)(v1114 * (signed __int64)(vert_y_middle_v4->V - vert_y_low_v3->V) / v1118) - vert_y_high_v5->V)
						/ v88;
				}
				v1128 = (vert_y_high_v5->U - vert_y_low_v3->U) / v1114;
				v1139 = (vert_y_high_v5->V - vert_y_low_v3->V) / v1114;
				v1134 = (vert_y_middle_v4->U - vert_y_high_v5->U) / v1120;
				v1145 = (vert_y_middle_v4->V - vert_y_high_v5->V) / v1120;
				v89 = vert_y_low_v3->X << 16;
				v90 = vert_y_low_v3->X << 16;
				v91 = vert_y_low_v3->U;
				v92 = vert_y_low_v3->V;
				if (vertYlowIsNegative)
				{
					v18 = __OFSUB__(linesToDraw_v1123, -vertLowY_v1190);
					v16 = linesToDraw_v1123 == -vertLowY_v1190;
					v17 = linesToDraw_v1123 + vertLowY_v1190 < 0;
					linesToDraw_v1123 += vertLowY_v1190;
					if ((uint8_t)(v17 ^ v18) | (uint8_t)v16)
						return;
					v1162 = -vertLowY_v1190;
					if (-vertLowY_v1190 - v1114 >= 0)
					{
						v93 = v1162 - v1114;
						v1120 -= v93;
						v94 = v1112 * v93 + fp_vertHighX;
						v90 += v93 * v1108 + v1114 * v1108;
						v91 += v93 * v1134 + v1114 * v1128;
						v92 += v93 * v1145 + v1114 * v1139;
						if (v1301)
						{
							v1120 = viewPort.Height_DE568;
							linesToDraw_v1123 = viewPort.Height_DE568;
						}
						v95 = &rasterlines_DE56Cx[startLine][0];
					LABEL_181_DrawTriangle:
						if (v1297)
						{
						}
						else
						{
							v95 = RasterizePolygon(v95, &v94, &v90, &v91, &v92, v1112, v1108, v1134, v1145, &v1120);
						}
						goto LABEL_DrawRasterLines;
					}
					v1114 += vertLowY_v1190;
					v89 += fp_slope_HighLowVert * v1162;
					v90 += v1162 * v1108;
					v91 += v1162 * v1128;
					v92 += v1162 * v1139;
					if (v1301)
					{
						linesToDraw_v1123 = viewPort.Height_DE568;
						if (v1297)
						{
							v1114 = viewPort.Height_DE568;
						}
						else
						{
							v1297 = viewPort.Height_DE568 <= v1114;
							v1120 = viewPort.Height_DE568 - v1114;
						}
					}
				}
				else if (v1301)
				{
					const int v96 = viewPort.Height_DE568 - vertLowY_v1190;
					linesToDraw_v1123 = viewPort.Height_DE568 - vertLowY_v1190;
					if (v1297)
					{
						v1114 = viewPort.Height_DE568 - vertLowY_v1190;
					}
					else
					{
						v18 = __OFSUB__(v96, v1114);
						const int v97 = v96 - v1114;
						v1297 = (v97 < 0) ^ v18 | (v97 == 0);
						v1120 = v97;
					}
				}
				v95 = RasterizePolygon(&rasterlines_DE56Cx[startLine][0], &v89, &v90, &v91, &v92, fp_slope_HighLowVert, v1108, v1128, v1139, &v1114);
				v94 = fp_vertHighX;
				goto LABEL_181_DrawTriangle;
			case 5:
			case 6:
			case 0x14:
			case 0x15:
			case 0x18:
			case 0x19:
			case 0x1A:
				// normal shading and reflections
				v69 = v1114 * (int64_t)(vert_y_middle_v4->X - vert_y_low_v3->X) / v68;
				v70 = vert_y_low_v3->X - vert_y_high_v5->X;
				v18 = __OFADD__(v69, v70);
				v17 = v69 + v70 < 0;
				v72 = v69 + v70;
				if ((uint8_t)v17 ^ v18)
					return;
				if (!(v69 + v70 == 0))
				{
					const int v73 = v72 + 1;
					Uincrement = (signed int)(vert_y_low_v3->U + (uint64_t)(v1114 * (int64_t)(vert_y_middle_v4->U - vert_y_low_v3->U) / v1118) - vert_y_high_v5->U)
						/ v73;
					Vincrement = (signed int)(vert_y_low_v3->V + (uint64_t)(v1114 * (int64_t)(vert_y_middle_v4->V - vert_y_low_v3->V) / v1118) - vert_y_high_v5->V)
						/ v73;
					v69 = (signed int)(vert_y_low_v3->Brightness + (uint64_t)(v1114 * (int64_t)(vert_y_middle_v4->Brightness - vert_y_low_v3->Brightness) / v1118) - vert_y_high_v5->Brightness) / v73;
				}
				BrightnessIncrement = v69;
				int v1127 = (vert_y_high_v5->U - vert_y_low_v3->U) / v1114;
				int v1138 = (vert_y_high_v5->V - vert_y_low_v3->V) / v1114;
				int v1149 = (vert_y_high_v5->Brightness - vert_y_low_v3->Brightness) / v1114;
				int v1133 = (vert_y_middle_v4->U - vert_y_high_v5->U) / v1120;
				int v1144 = (vert_y_middle_v4->V - vert_y_high_v5->V) / v1120;
				int v1155 = (vert_y_middle_v4->Brightness - vert_y_high_v5->Brightness) / v1120;
				int fpRasterStartX = vert_y_low_v3->X << 16;
				int fpRasterEndX = vert_y_low_v3->X << 16;
				int rasterU = vert_y_low_v3->U;
				int rasterV = vert_y_low_v3->V;
				int rasterBrighness = vert_y_low_v3->Brightness;
				int v1161;
				int v79;
				int v80;

				if (vertYlowIsNegative)
				{
					v18 = __OFSUB__(linesToDraw_v1123, -vertLowY_v1190);
					v16 = linesToDraw_v1123 == -vertLowY_v1190;
					v17 = linesToDraw_v1123 + vertLowY_v1190 < 0;
					linesToDraw_v1123 += vertLowY_v1190;
					if ((uint8_t)(v17 ^ v18) | (uint8_t)v16)
						return;
					v1161 = -vertLowY_v1190;
					if (-vertLowY_v1190 - v1114 >= 0)
					{
						v79 = v1161 - v1114;
						v1120 -= v79;
						v80 = v1112 * v79 + fp_vertHighX;                  // FIXME: overflow here
						fpRasterEndX += v79 * v1108 + v1114 * v1108;         // FIXME: overflow here
						rasterU += v79 * v1133 + v1114 * v1127;
						rasterV += v79 * v1144 + v1114 * v1138;
						rasterBrighness += v79 * v1155 + v1114 * v1149;
						if (v1301)
						{
							v1120 = viewPort.Height_DE568;
							linesToDraw_v1123 = viewPort.Height_DE568;
						}
						v81 = &rasterlines_DE56Cx[startLine][0];
					LABEL_156_DrawTriangle:
						if (v1297)
						{
						}
						else
						{
							v81 = RasterizePolygon(v81, &v80, &fpRasterEndX, &rasterU, &rasterV, &rasterBrighness, v1112, v1108, v1133, v1144, v1155, &v1120);
						}
						goto LABEL_DrawRasterLines; // draw raster lines
					}
					v1114 += vertLowY_v1190;
					fpRasterStartX += fp_slope_HighLowVert * v1161;                  // FIXME: overflow here
					fpRasterEndX += v1161 * v1108;                  // FIXME: overflow here
					rasterU += v1161 * v1127;
					rasterV += v1161 * v1138;
					rasterBrighness += v1161 * v1149;
					if (v1301)
					{
						linesToDraw_v1123 = viewPort.Height_DE568;
						if (v1297)
						{
							v1114 = viewPort.Height_DE568;
						}
						else
						{
							v1297 = viewPort.Height_DE568 <= v1114;
							v1120 = viewPort.Height_DE568 - v1114;
						}
					}
				}
				else if (v1301)
				{
					const int v82 = viewPort.Height_DE568 - vertLowY_v1190;
					linesToDraw_v1123 = viewPort.Height_DE568 - vertLowY_v1190;
					if (v1297)
					{
						v1114 = viewPort.Height_DE568 - vertLowY_v1190;
					}
					else
					{
						v18 = __OFSUB__(v82, v1114);
						const int v83 = v82 - v1114;
						v1297 = (v83 < 0) ^ v18 | (v83 == 0);
						v1120 = v83;
					}
				}
				v81 = RasterizePolygon(&rasterlines_DE56Cx[startLine][0], &fpRasterStartX, &fpRasterEndX, &rasterU, &rasterV, &rasterBrighness, fp_slope_HighLowVert, v1108, v1127, v1138, v1149, &v1114);
				v80 = fp_vertHighX;
				goto LABEL_156_DrawTriangle;
			}
		}
	}
	return;


LABEL_24_DrawTriangle:
	// general case where
	// 
	// vertex_low                             vertex_low
	//  |       |                             |       |
	//  |       vertex_middle  or  vertex_middle      |     
	//  |       |                             |       |
	// vertex_high                           vertex_high
	// 
	// Only the _left_ triangle is clock-wise, the right case will be culled by
	// making sure that the slope low-middle is greater than the slope low-high.
	{
		int v26;
		int v27;
		Rasterline_t *v28;
		int v32;
		int v33;
		bool v34;
		int v35;
		int v37;
		int v38;
		int v39;
		int v40;
		int v41;
		int v42;
		Rasterline_t *v43;
		int v58;
		int v59;
		int v60;
		int v61;
		Rasterline_t *v62;
		int v63;
		int v1126;
		int v1137;
		int v1157;
		int v1158;
		int v1160;
		bool v1292;

		const int v1190 = vert_y_low_v3->Y;

		if (vert_y_low_v3->Y >= 0)
		{
			if (vert_y_low_v3->Y >= viewPort.Height_DE568)
				return;
			renderBufferStartOfCurrentLine = ViewPortRenderBufferAltStart_DE554 + iScreenWidth_DE560 * vert_y_low_v3->Y;
			v1292 = false;
		}
		else
		{
			renderBufferStartOfCurrentLine = ViewPortRenderBufferAltStart_DE554;
			v1292 = true;
		}
		const bool vertYHigh_above_viewport = vert_y_high_v5->Y > viewPort.Height_DE568;
		const int dY_HighLowVert = vert_y_high_v5->Y - vert_y_low_v3->Y;
		linesToDraw_v1123 = dY_HighLowVert;
		bool vertYMiddle_above_viewport = vert_y_middle_v4->Y > viewPort.Height_DE568;
		const int dY_MiddleLowVert = vert_y_middle_v4->Y - vert_y_low_v3->Y;
		int v1117 = dY_MiddleLowVert;
		const int fp_slope_HighLowVert = ((vert_y_high_v5->X - vert_y_low_v3->X) << 16) / dY_HighLowVert;
		const int fp_slope_MiddleLowVert = ((vert_y_middle_v4->X - vert_y_low_v3->X) << 16) / dY_MiddleLowVert;

		// only draw triangle with clock-wise vertices by comparing the slopes
		if (fp_slope_MiddleLowVert > fp_slope_HighLowVert)
		{
			// vertex_low
			//  |       |
			//  |       vertex_middle
			//  |       |
			// vertex_high
			const int fp_slope_HighMiddleVert = ((vert_y_high_v5->X - vert_y_middle_v4->X) << 16) / (vert_y_high_v5->Y - vert_y_middle_v4->Y);
			int v1119 = vert_y_high_v5->Y - vert_y_middle_v4->Y;
			const int fp_vertMiddleX = vert_y_middle_v4->X << 16;
			switch (x_BYTE_E126D)
			{
			case 0:
			case 0xE:
			case 0xF:
				v58 = vert_y_low_v3->X << 16;
				v59 = vert_y_low_v3->X << 16;
				if (!v1292)
				{
					if (vertYHigh_above_viewport)
					{
						v63 = viewPort.Height_DE568 - v1190;
						linesToDraw_v1123 = viewPort.Height_DE568 - v1190;
						if (vertYMiddle_above_viewport)
						{
							v1117 = viewPort.Height_DE568 - v1190;
						}
						else
						{
							v18 = __OFSUB__(v63, v1117);
							const int v64 = v63 - v1117;
							vertYMiddle_above_viewport = (v64 < 0) ^ v18 | (v64 == 0);
							v1119 = v64;
						}
					}
					goto LABEL_121_DrawTriangle;
				}
				v18 = __OFSUB__(linesToDraw_v1123, -v1190);
				v16 = linesToDraw_v1123 == -v1190;
				v17 = linesToDraw_v1123 + v1190 < 0;
				linesToDraw_v1123 += v1190;
				if (!((uint8_t)(v17 ^ v18) | (uint8_t)v16))
				{
					v1160 = -v1190;
					if (-v1190 - v1117 >= 0)
					{
						v1119 -= v1160 - v1117;
						v60 = v1160 - v1117;
						v58 += fp_slope_HighLowVert * v60 + v1117 * fp_slope_HighLowVert;
						v61 = fp_slope_HighMiddleVert * v60 + fp_vertMiddleX;
						if (vertYHigh_above_viewport)
						{
							v1119 = viewPort.Height_DE568;
							linesToDraw_v1123 = viewPort.Height_DE568;
						}
						v62 = &rasterlines_DE56Cx[startLine][0];
						goto LABEL_124_DrawTriangle;
					}
					v1117 += v1190;
					v58 += fp_slope_HighLowVert * v1160;
					v59 += v1160 * fp_slope_MiddleLowVert;
					if (vertYHigh_above_viewport)
					{
						linesToDraw_v1123 = viewPort.Height_DE568;
						if (vertYMiddle_above_viewport)
						{
							v1117 = viewPort.Height_DE568;
						}
						else
						{
							vertYMiddle_above_viewport = viewPort.Height_DE568 <= v1117;
							v1119 = viewPort.Height_DE568 - v1117;
						}
					}
				LABEL_121_DrawTriangle:
					v62 = RasterizePolygon(&rasterlines_DE56Cx[startLine][0], &v58, &v59, fp_slope_HighLowVert, fp_slope_MiddleLowVert, &v1117);
					v61 = fp_vertMiddleX;
				LABEL_124_DrawTriangle:
					if (vertYMiddle_above_viewport)
					{
					}
					else
					{
						v62 = RasterizePolygon(v62, &v58, &v61, fp_slope_HighLowVert, fp_slope_HighMiddleVert, &v1119);
					}
					goto LABEL_DrawRasterLines;
				}
				return;
			case 1:
			case 4:
			case 0x10:
			case 0x11:
				Logger->error("!!! Unsupported polygon mode !!!");
			case 2:
			case 3:
			case 7:
			case 8:
			case 9:
			case 0xA:
			case 0xB:
			case 0xC:
			case 0xD:
			case 0x12:
			case 0x13:
			case 0x16:
			case 0x17:
				// flat shading
				v32 = dY_MiddleLowVert * (signed __int64)(vert_y_low_v3->X - vert_y_high_v5->X) / dY_HighLowVert;
				v33 = vert_y_middle_v4->X - vert_y_low_v3->X;
				v18 = __OFADD__(v32, v33);
				v34 = v32 + v33 == 0;
				v17 = v32 + v33 < 0;
				v35 = v32 + v33;
				if ((uint8_t)v17 ^ v18)
					return;
				if (!v34)
				{
					const int v36 = v35 + 1;
					Uincrement = (signed int)(vert_y_middle_v4->U + (unsigned __int64)(v1117 * (signed __int64)(vert_y_low_v3->U - vert_y_high_v5->U) / dY_HighLowVert) - vert_y_low_v3->U)
						/ v36;
					Vincrement = (signed int)(vert_y_middle_v4->V + (unsigned __int64)(v1117 * (signed __int64)(vert_y_low_v3->V - vert_y_high_v5->V) / dY_HighLowVert) - vert_y_low_v3->V)
						/ v36;
				}
				v1126 = (vert_y_high_v5->U - vert_y_low_v3->U) / dY_HighLowVert;
				v1137 = (vert_y_high_v5->V - vert_y_low_v3->V) / dY_HighLowVert;
				v37 = vert_y_low_v3->X << 16;
				v38 = vert_y_low_v3->X << 16;
				v39 = vert_y_low_v3->U;
				v40 = vert_y_low_v3->V;
				if (v1292)
				{
					v18 = __OFSUB__(linesToDraw_v1123, -v1190);
					v16 = linesToDraw_v1123 == -v1190;
					v17 = linesToDraw_v1123 + v1190 < 0;
					linesToDraw_v1123 += v1190;
					if ((uint8_t)(v17 ^ v18) | (uint8_t)v16)
						return;
					v1158 = -v1190;
					if (-v1190 - v1117 >= 0)
					{
						v1119 -= v1158 - v1117;
						v41 = v1158 - v1117;
						v37 += fp_slope_HighLowVert * v41 + v1117 * fp_slope_HighLowVert;
						v42 = fp_slope_HighMiddleVert * v41 + fp_vertMiddleX;
						v39 += v41 * v1126 + v1117 * v1126;
						v40 += v41 * v1137 + v1117 * v1137;
						if (vertYHigh_above_viewport)
						{
							v1119 = viewPort.Height_DE568;
							linesToDraw_v1123 = viewPort.Height_DE568;
						}
						v43 = &rasterlines_DE56Cx[startLine][0];
					LABEL_77_DrawTriangle:
						if (vertYMiddle_above_viewport)
						{
						}
						else
						{
							v43 = RasterizePolygon(v43, &v37, &v42, &v39, &v40, fp_slope_HighLowVert, fp_slope_HighMiddleVert, v1126, v1137, &v1119);
						}
						goto LABEL_DrawRasterLines;
					}
					v1117 += v1190;
					v37 += fp_slope_HighLowVert * v1158;
					v38 += v1158 * fp_slope_MiddleLowVert;
					v39 += v1158 * v1126;
					v40 += v1158 * v1137;
					if (vertYHigh_above_viewport)
					{
						linesToDraw_v1123 = viewPort.Height_DE568;
						if (vertYMiddle_above_viewport)
						{
							v1117 = viewPort.Height_DE568;
						}
						else
						{
							vertYMiddle_above_viewport = viewPort.Height_DE568 <= v1117;
							v1119 = viewPort.Height_DE568 - v1117;
						}
					}
				}
				else if (vertYHigh_above_viewport)
				{
					const int v44 = viewPort.Height_DE568 - v1190;
					linesToDraw_v1123 = viewPort.Height_DE568 - v1190;
					if (vertYMiddle_above_viewport)
					{
						v1117 = viewPort.Height_DE568 - v1190;
					}
					else
					{
						v18 = __OFSUB__(v44, v1117);
						const int v45 = v44 - v1117;
						vertYMiddle_above_viewport = (v45 < 0) ^ v18 | (v45 == 0);
						v1119 = v45;
					}
				}
				v43 = RasterizePolygon(&rasterlines_DE56Cx[startLine][0], &v37, &v38, &v39, &v40, fp_slope_HighLowVert, fp_slope_MiddleLowVert, v1126, v1137, &v1117);
				v42 = fp_vertMiddleX;
				goto LABEL_77_DrawTriangle;
			case 5:
			case 6:
			case 0x14:
			case 0x15:
			case 0x18:
			case 0x19:
			case 0x1A:
				v14 = dY_MiddleLowVert * (signed __int64)(vert_y_low_v3->X - vert_y_high_v5->X) / dY_HighLowVert;
				v15 = vert_y_middle_v4->X - vert_y_low_v3->X;
				v18 = __OFADD__(v14, v15);
				v16 = v14 + v15 == 0;
				v17 = v14 + v15 < 0;
				int v19 = v14 + v15;
				if ((uint8_t)v17 ^ v18)
					return;
				const int v20 = v19 + 1;
				Uincrement = (signed int)(vert_y_middle_v4->U + (unsigned __int64)(v1117 * (signed __int64)(vert_y_low_v3->U - vert_y_high_v5->U) / dY_HighLowVert) - vert_y_low_v3->U)
					/ v20;
				Vincrement = (signed int)(vert_y_middle_v4->V + (unsigned __int64)(v1117 * (signed __int64)(vert_y_low_v3->V - vert_y_high_v5->V) / dY_HighLowVert) - vert_y_low_v3->V)
					/ v20;
				BrightnessIncrement = (signed int)(vert_y_middle_v4->Brightness + (unsigned __int64)(v1117 * (signed __int64)(vert_y_low_v3->Brightness - vert_y_high_v5->Brightness) / dY_HighLowVert) - vert_y_low_v3->Brightness)
					/ v20;
				const int v1125 = (vert_y_high_v5->U - vert_y_low_v3->U) / dY_HighLowVert;
				const int v1136 = (vert_y_high_v5->V - vert_y_low_v3->V) / dY_HighLowVert;
				const int v1147 = (vert_y_high_v5->Brightness - vert_y_low_v3->Brightness) / dY_HighLowVert;
				int v21 = vert_y_low_v3->X << 16;
				int v22 = vert_y_low_v3->X << 16;
				int v23 = vert_y_low_v3->U;
				int v24 = vert_y_low_v3->V;
				int v25 = vert_y_low_v3->Brightness;
				if (v1292)
				{
					v18 = __OFSUB__(linesToDraw_v1123, -v1190);
					v16 = linesToDraw_v1123 == -v1190;
					v17 = linesToDraw_v1123 + v1190 < 0;
					linesToDraw_v1123 += v1190;
					if ((uint8_t)(v17 ^ v18) | (uint8_t)v16)
						return;
					v1157 = -v1190;
					if (-v1190 - v1117 >= 0)
					{
						v1119 -= v1157 - v1117;
						v26 = v1157 - v1117;
						v21 += fp_slope_HighLowVert * (int64_t)v26 + (int64_t)v1117 * fp_slope_HighLowVert;
						v27 = fp_slope_HighMiddleVert * (int64_t)v26 + fp_vertMiddleX;
						v23 += (int64_t)v26 * v1125 + v1117 * v1125;
						v24 += (int64_t)v26 * v1136 + v1117 * v1136;
						v25 += (int64_t)v26 * v1147 + v1117 * v1147;
						if (vertYHigh_above_viewport)
						{
							v1119 = viewPort.Height_DE568;
							linesToDraw_v1123 = viewPort.Height_DE568;
						}
						v28 = &rasterlines_DE56Cx[startLine][0];
					LABEL_51_DrawTriangle:
						if (vertYMiddle_above_viewport)
						{
						}
						else
						{
							v28 = RasterizePolygon(v28, &v21, &v27, &v23, &v24, &v25, fp_slope_HighLowVert, fp_slope_HighMiddleVert, v1125, v1136, v1147, &v1119);
						}
						goto LABEL_DrawRasterLines;
					}
					v1117 += v1190;
					v21 += fp_slope_HighLowVert * v1157;
					v22 += (int64_t)v1157 * fp_slope_MiddleLowVert;
					v23 += v1157 * v1125;
					v24 += v1157 * v1136;
					v25 += v1157 * v1147;
					if (vertYHigh_above_viewport)
					{
						linesToDraw_v1123 = viewPort.Height_DE568;
						if (vertYMiddle_above_viewport)
						{
							v1117 = viewPort.Height_DE568;
						}
						else
						{
							vertYMiddle_above_viewport = viewPort.Height_DE568 <= v1117;
							v1119 = viewPort.Height_DE568 - v1117;
						}
					}
				}
				else if (vertYHigh_above_viewport)
				{
					const int v29 = viewPort.Height_DE568 - v1190;
					linesToDraw_v1123 = viewPort.Height_DE568 - v1190;
					if (vertYMiddle_above_viewport)
					{
						v1117 = viewPort.Height_DE568 - v1190;
					}
					else
					{
						v18 = __OFSUB__(v29, v1117);
						const int v30 = v29 - v1117;
						vertYMiddle_above_viewport = (v30 < 0) ^ v18 | (v30 == 0);
						v1119 = v30;
					}
				}
				v28 = RasterizePolygon(&rasterlines_DE56Cx[startLine][0], &v21, &v22, &v23, &v24, &v25, fp_slope_HighLowVert, fp_slope_MiddleLowVert, v1125, v1136, v1147, &v1117);
				v27 = fp_vertMiddleX;
				goto LABEL_51_DrawTriangle;
			}
		}
	}
	return;

LABEL_277_PrepareRasterlineForTriangleWithHorizontalBottom:
	// this code covers the cases:

	//       vertex3
	//       |     |
	// vertex2 --- vertex1 
	// vert_y_low_v3 = vertex3;
	// vert_y_middle = vertex1;
	// vert_y_high_v5 = vertex2;

	//       vertex1
	//       |     |
	// vertex3 --- vertex2
	// vert_y_low_v3 = vertex1;
	// vert_y_middle = vertex2;
	// vert_y_high_v5 = vertex3;

	//       vertex2
	//       |     | 
	// vertex1 --- vertex3 
	// vert_y_low_v3 = vertex2;
	// vert_y_middle = vertex3;
	// vert_y_high_v5 = vertex1;

	{
		int v119;
		int v120;
		int v121;
		int v122;
		int v123;
		int v124;
		int v125;
		int v127;
		int v128;
		int v129;
		int v130;
		int v131;
		int v139;
		int v140;
		bool v1294;
		int v1129;
		int v1130;
		int v1140;
		int v1141;
		int v1151;

		const int v117 = vert_y_low_v3->Y;
		const int v1192 = v117;
		if (v117 >= 0)
		{
			if (v117 >= viewPort.Height_DE568)
				return;
			renderBufferStartOfCurrentLine = ViewPortRenderBufferAltStart_DE554 + iScreenWidth_DE560 * v117;
			v1294 = false;
		}
		else
		{
			renderBufferStartOfCurrentLine = ViewPortRenderBufferAltStart_DE554;
			v1294 = true;
		}
		const int v118 = vert_y_high_v5->Y;
		const bool v1298 = v118 > viewPort.Height_DE568;
		int v1115 = v118 - v117;
		linesToDraw_v1123 = v118 - v117; // NOTE: vert_y_high_v5->Y = vert_y_middle->Y
		const int v1105 = ((vert_y_high_v5->X - vert_y_low_v3->X) << 16) / linesToDraw_v1123;
		const int v1109 = ((vert_y_middle_v4->X - vert_y_low_v3->X) << 16) / linesToDraw_v1123;
		switch (x_BYTE_E126D)
		{
		case 0:
		case 0xE:
		case 0xF:
			v139 = vert_y_low_v3->X << 16;
			v140 = vert_y_low_v3->X << 16;
			if (v1294)
			{
				int v141 = -v1192;
				v1115 += v1192;
				v18 = __OFSUB__(linesToDraw_v1123, -v1192);
				v16 = linesToDraw_v1123 == -v1192;
				v17 = linesToDraw_v1123 + v1192 < 0;
				linesToDraw_v1123 += v1192;
				if ((uint8_t)(v17 ^ v18) | (uint8_t)v16)
					return;
				v139 += v1105 * v141;
				v140 += v141 * v1109;
				if (v1298)
				{
					linesToDraw_v1123 = viewPort.Height_DE568;
					v1115 = viewPort.Height_DE568;
				}
			}
			else if (v1298)
			{
				linesToDraw_v1123 = viewPort.Height_DE568 - v1192;
				v1115 = viewPort.Height_DE568 - v1192;
			}
			RasterizePolygon(&rasterlines_DE56Cx[startLine][0], &v139, &v140, v1105, v1109, &v1115);
			goto LABEL_DrawRasterLines;
		case 2:
		case 3:
		case 7:
		case 8:
		case 9:
		case 0xA:
		case 0xB:
		case 0xC:
		case 0xD:
		case 0x12:
		case 0x13:
		case 0x16:
		case 0x17:
			v127 = vert_y_middle_v4->X - vert_y_high_v5->X;
			Uincrement = (vert_y_middle_v4->U - vert_y_high_v5->U) / v127;
			Vincrement = (vert_y_middle_v4->V - vert_y_high_v5->V) / v127;
			v1130 = (vert_y_high_v5->U - vert_y_low_v3->U) / linesToDraw_v1123;
			v1141 = (vert_y_high_v5->V - vert_y_low_v3->V) / linesToDraw_v1123;
			v128 = vert_y_low_v3->X << 16;
			v129 = vert_y_low_v3->X << 16;
			v130 = vert_y_low_v3->U;
			v131 = vert_y_low_v3->V;
			if (v1294)
			{
				const int v132 = -v1192;
				v1115 += v1192;
				v18 = __OFSUB__(linesToDraw_v1123, -v1192);
				v16 = linesToDraw_v1123 == -v1192;
				v17 = linesToDraw_v1123 + v1192 < 0;
				linesToDraw_v1123 += v1192;
				if ((uint8_t)(v17 ^ v18) | (uint8_t)v16)
					return;
				v128 += v1105 * v132;
				v129 += v132 * v1109;
				v130 += v132 * v1130;
				v131 += v132 * v1141;
				if (v1298)
				{
					linesToDraw_v1123 = viewPort.Height_DE568;
					v1115 = viewPort.Height_DE568;
				}
			}
			else if (v1298)
			{
				linesToDraw_v1123 = viewPort.Height_DE568 - v1192;
				v1115 = viewPort.Height_DE568 - v1192;
			}
			RasterizePolygon(&rasterlines_DE56Cx[startLine][0], &v128, &v129, &v130, &v131, v1105, v1109, v1130, v1141, &v1115);
			goto LABEL_DrawRasterLines;
		case 1:
		case 4:
		case 0x10:
		case 0x11:
			Logger->error("!!! Unsupported polygon mode !!!");
		case 5:
		case 6:
		case 0x14:
		case 0x15:
		case 0x18:
		case 0x19:
		case 0x1A:
			v119 = vert_y_middle_v4->X - vert_y_high_v5->X;
			Uincrement = (vert_y_middle_v4->U - vert_y_high_v5->U) / v119;
			Vincrement = (vert_y_middle_v4->V - vert_y_high_v5->V) / v119;
			BrightnessIncrement = (vert_y_middle_v4->Brightness - vert_y_high_v5->Brightness) / v119;
			v1129 = (vert_y_high_v5->U - vert_y_low_v3->U) / linesToDraw_v1123;
			v1140 = (vert_y_high_v5->V - vert_y_low_v3->V) / linesToDraw_v1123;
			v1151 = (vert_y_high_v5->Brightness - vert_y_low_v3->Brightness) / linesToDraw_v1123;
			v120 = vert_y_low_v3->X << 16;
			v121 = vert_y_low_v3->X << 16;
			v122 = vert_y_low_v3->U;
			v123 = vert_y_low_v3->V;
			v124 = vert_y_low_v3->Brightness;
			if (v1294)
			{
				v125 = -v1192;
				v1115 += v1192;
				v18 = __OFSUB__(linesToDraw_v1123, -v1192);
				v16 = linesToDraw_v1123 == -v1192;
				v17 = linesToDraw_v1123 + v1192 < 0;
				linesToDraw_v1123 += v1192;
				if ((uint8_t)(v17 ^ v18) | (uint8_t)v16)
					return;
				v120 += v1105 * v125;
				v121 += v125 * v1109;
				v122 += v125 * v1129;
				v123 += v125 * v1140;
				v124 += v125 * v1151;
				if (v1298)
				{
					linesToDraw_v1123 = viewPort.Height_DE568;
					v1115 = viewPort.Height_DE568;
				}
			}
			else if (v1298)
			{
				linesToDraw_v1123 = viewPort.Height_DE568 - v1192;
				v1115 = viewPort.Height_DE568 - v1192;
			}
			RasterizePolygon(&rasterlines_DE56Cx[startLine][0], &v120, &v121, &v122, &v123, &v124, v1105, v1109, v1129, v1140, v1151, &v1115);
			goto LABEL_DrawRasterLines;
		}
	}


LABEL_277_PrepareRasterlineForTriangleWithHorizontalTop: 
	// handled cases:

	// vertex3 --- vertex1
	//       |     |
	//       vertex2
	// vert_y_low_v3 = vertex3;
	// vert_y_middle = vertex1;
	// vert_y_high_v5 = vertex2;

	// vertex2 --- vertex3 
	//       |     |
	//       vertex1
	// vert_y_low_v3 = vertex2;
	// vert_y_middle = vertex3;
	// vert_y_high_v5 = vertex1;

	// vertex1 --- vertex2 
	//       |     |
	//       vertex3
	// vert_y_low_v3 = vertex1;
	// vert_y_middle = vertex2;
	// vert_y_high_v5 = vertex3;

 	{
		// clipping in Y direction to viewport
		bool vertLowYnegative; // [esp+62h] [ebp-26h]
		const int vertLowY = vert_y_low_v3->Y;
		const int vertHighY = vert_y_high_v5->Y;

		if (vertLowY >= 0)
		{
			if (vertLowY >= viewPort.Height_DE568) {
				// nothing to be drawn - whole tringle Y is above the viewport
				return;
			}
			renderBufferStartOfCurrentLine = ViewPortRenderBufferAltStart_DE554 + iScreenWidth_DE560 * vertLowY;
			vertLowYnegative = false;
		}
		else
		{
			renderBufferStartOfCurrentLine = ViewPortRenderBufferAltStart_DE554;
			vertLowYnegative = true;
		}

		const bool vertHighYaboveViewport = vertHighY > viewPort.Height_DE568;
		int dY_HighLow_actual_rows_to_draw = vertHighY - vertLowY;

		linesToDraw_v1123 = vertHighY - vertLowY;
		const int fp_slope_HighLowVert = ((vert_y_high_v5->X - vert_y_low_v3->X) << 16) / (dY_HighLow_actual_rows_to_draw);
		const int v1110 = ((vert_y_high_v5->X - vert_y_middle_v4->X) << 16) / (dY_HighLow_actual_rows_to_draw);

		int v154;
		int v155;
		int v156;
		int v157;
		int v158;
		int v165;
		int v166;
		int v167;
		int v1132;
		int v1143;
		int dX_v4v3;

		switch (x_BYTE_E126D)
		{
		case 0:
		case 0xE:
		case 0xF:
			// debug shading - single color
			v165 = vert_y_low_v3->X << 16;
			v166 = vert_y_middle_v4->X << 16;
			if (vertLowYnegative)
			{
				v167 = -vertLowY;
				dY_HighLow_actual_rows_to_draw += vertLowY;
				v18 = __OFSUB__(linesToDraw_v1123, -vertLowY);
				v16 = linesToDraw_v1123 == -vertLowY;
				v17 = linesToDraw_v1123 + vertLowY < 0;
				linesToDraw_v1123 += vertLowY;
				if ((uint8_t)(v17 ^ v18) | (uint8_t)v16)
					return;
				v165 += fp_slope_HighLowVert * v167;
				v166 += v167 * v1110;
				if (vertHighYaboveViewport)
				{
					linesToDraw_v1123 = viewPort.Height_DE568;
					dY_HighLow_actual_rows_to_draw = viewPort.Height_DE568;
				}
			}
			else if (vertHighYaboveViewport)
			{
				linesToDraw_v1123 = viewPort.Height_DE568 - vertLowY;
				dY_HighLow_actual_rows_to_draw = viewPort.Height_DE568 - vertLowY;
			}
			RasterizePolygon(&rasterlines_DE56Cx[startLine][0], &v165, &v166, fp_slope_HighLowVert, v1110, &dY_HighLow_actual_rows_to_draw);
			goto LABEL_DrawRasterLines;
		case 2:
		case 3:
		case 7:
		case 8:
		case 9:
		case 0xA:
		case 0xB:
		case 0xC:
		case 0xD:
		case 0x12:
		case 0x13:
		case 0x16:
		case 0x17:
			// flat shading
			dX_v4v3 = vert_y_middle_v4->X - vert_y_low_v3->X;
			Uincrement = (vert_y_middle_v4->U - vert_y_low_v3->U) / dX_v4v3;
			Vincrement = (vert_y_middle_v4->V - vert_y_low_v3->V) / dX_v4v3;
			v1132 = (vert_y_high_v5->U - vert_y_low_v3->U) / linesToDraw_v1123;
			v1143 = (vert_y_high_v5->V - vert_y_low_v3->V) / linesToDraw_v1123;
			v154 = vert_y_low_v3->X << 16;
			v155 = vert_y_middle_v4->X << 16;
			v156 = vert_y_low_v3->U;
			v157 = vert_y_low_v3->V;
			if (vertLowYnegative)
			{
				v158 = -vertLowY;
				dY_HighLow_actual_rows_to_draw += vertLowY;
				v18 = __OFSUB__(linesToDraw_v1123, -vertLowY);
				v16 = linesToDraw_v1123 == -vertLowY;
				v17 = linesToDraw_v1123 + vertLowY < 0;
				linesToDraw_v1123 += vertLowY;
				if ((uint8_t)(v17 ^ v18) | (uint8_t)v16)
					return;
				v154 += fp_slope_HighLowVert * v158;
				v155 += v158 * v1110;
				v156 += v158 * v1132;
				v157 += v158 * v1143;
				if (vertHighYaboveViewport)
				{
					linesToDraw_v1123 = viewPort.Height_DE568;
					dY_HighLow_actual_rows_to_draw = viewPort.Height_DE568;
				}
			}
			else if (vertHighYaboveViewport)
			{
				linesToDraw_v1123 = viewPort.Height_DE568 - vertLowY;
				dY_HighLow_actual_rows_to_draw = viewPort.Height_DE568 - vertLowY;
			}
			RasterizePolygon(&rasterlines_DE56Cx[startLine][0], &v154, &v155, &v156, &v157, fp_slope_HighLowVert, v1110, v1132, v1143, &dY_HighLow_actual_rows_to_draw);
			goto LABEL_DrawRasterLines;
		case 1:
		case 4:
		case 0x10:
		case 0x11:
			Logger->error("!!! Unsupported polygon mode !!!");
		case 5:
		case 6:
		case 0x14:
		case 0x15:
		case 0x18:
		case 0x19:
		case 0x1A:
			// normal shading and reflections
			dX_v4v3 = vert_y_middle_v4->X - vert_y_low_v3->X;
			Uincrement = (vert_y_middle_v4->U - vert_y_low_v3->U) / dX_v4v3;
			Vincrement = (vert_y_middle_v4->V - vert_y_low_v3->V) / dX_v4v3;
			BrightnessIncrement = (vert_y_middle_v4->Brightness - vert_y_low_v3->Brightness) / dX_v4v3;
			int v1131 = (vert_y_high_v5->U - vert_y_low_v3->U) / linesToDraw_v1123;
			int v1142 = (vert_y_high_v5->V - vert_y_low_v3->V) / linesToDraw_v1123;
			int v1153 = (vert_y_high_v5->Brightness - vert_y_low_v3->Brightness) / linesToDraw_v1123;
			int v146 = vert_y_low_v3->X << 16;
			int v147 = vert_y_middle_v4->X << 16;
			int v148 = vert_y_low_v3->U;
			int v149 = vert_y_low_v3->V;
			int v150 = vert_y_low_v3->Brightness;
			if (vertLowYnegative)
			{
				const int v151 = -vertLowY;
				dY_HighLow_actual_rows_to_draw += vertLowY;
				v18 = __OFSUB__(linesToDraw_v1123, -vertLowY);
				v16 = linesToDraw_v1123 == -vertLowY;
				v17 = linesToDraw_v1123 + vertLowY < 0;
				linesToDraw_v1123 += vertLowY;
				if ((uint8_t)(v17 ^ v18) | (uint8_t)v16)
					return;
				v146 += fp_slope_HighLowVert * v151;
				v147 += v151 * v1110;
				v148 += v151 * v1131;
				v149 += v151 * v1142;
				v150 += v151 * v1153;
				if (vertHighYaboveViewport)
				{
					linesToDraw_v1123 = viewPort.Height_DE568;
					dY_HighLow_actual_rows_to_draw = viewPort.Height_DE568;
				}
			}
			else if (vertHighYaboveViewport)
			{
				linesToDraw_v1123 = viewPort.Height_DE568 - vertLowY;
				dY_HighLow_actual_rows_to_draw = viewPort.Height_DE568 - vertLowY;
			}
			RasterizePolygon(&rasterlines_DE56Cx[startLine][0], &v146, &v147, &v148, &v149, &v150, fp_slope_HighLowVert, v1110, v1131, v1142, v1153, &dY_HighLow_actual_rows_to_draw);
			goto LABEL_DrawRasterLines;
		}
 	}


LABEL_DrawRasterLines:
	switch (x_BYTE_E126D)
	{
	case 0:
		DrawPolygonRasterLine_single_color_subB6253(
			&rasterlines_DE56Cx[startLine][0],
			startLine, drawEveryNthLine, linesToDraw_v1123,
			&renderBufferStartOfCurrentLine,
			x_BYTE_E126C
		);
		return;
	case 5:
		DrawPolygonRasterLine_subB6253(
			&rasterlines_DE56Cx[startLine][0],
			startLine, drawEveryNthLine, linesToDraw_v1123,
			&renderBufferStartOfCurrentLine, 
			Vincrement, Uincrement, BrightnessIncrement,
			x_DWORD_DE55C_ActTexture
		);
		return;
	case 7:
	case 0xB:
		DrawPolygonRasterLine_flat_shading_subB6253(
			&rasterlines_DE56Cx[startLine][0],
			startLine, drawEveryNthLine, linesToDraw_v1123,
			&renderBufferStartOfCurrentLine, 
			Vincrement, Uincrement,
			x_DWORD_DE55C_ActTexture, x_BYTE_E126C
		);
		return;
	case 0x1A:
		DrawPolygonRasterLine_reflections_subB6253(
			&rasterlines_DE56Cx[startLine][0],
			startLine, drawEveryNthLine, linesToDraw_v1123,
 			&renderBufferStartOfCurrentLine,
			Vincrement, Uincrement, BrightnessIncrement,
			x_DWORD_DE55C_ActTexture
		);
		return;
	}
}

Rasterline_t* GameRenderHD::RasterizePolygon(Rasterline_t* ptrPolys, int* v0, int* v1, int s0, int s1, int* line)
{
	do
	{
		ptrPolys->startX = *v0;
		*v0 += s0;
		ptrPolys->endX = *v1;
		*v1 += s1;
		ptrPolys++;
		*line = *line - 1;
	} while (*line);

	return ptrPolys;
}

Rasterline_t* GameRenderHD::RasterizePolygon(Rasterline_t* ptrPolys, int* v0, int* v1, int* v4, int s0, int s1, int s4, int* line)
{
	do
	{
		ptrPolys->startX = *v0;
		*v0 += s0;
		ptrPolys->endX = *v1;
		*v1 += s1;
		ptrPolys->brightness = *v4;
		*v4 += s4;
		ptrPolys++;
		*line = *line - 1;
	} while (*line);

	return ptrPolys;
}

Rasterline_t* GameRenderHD::RasterizePolygon(Rasterline_t* ptrPolys, int* v0, int* v1, int* v2, int* v3, int s0, int s1, int s2, int s3, int* line)
{
	do
	{
		ptrPolys->startX = *v0;
		*v0 += s0;
		ptrPolys->endX = *v1;
		*v1 += s1;
		ptrPolys->U = *v2;
		*v2 += s2;
		ptrPolys->V = *v3;
		*v3 += s3;
		ptrPolys++;
		*line = *line - 1;
	} while (*line);

	return ptrPolys;
}

Rasterline_t* GameRenderHD::RasterizePolygon(Rasterline_t* ptrPolys, 
	int *startX, int *endX, int *U, int *V, int *brightness, 
	int startX_inc, int endX_inc, int U_inc, int V_inc, int brightness_inc, int* numLines)
{
	do
	{
		ptrPolys->startX = *startX;
		*startX += startX_inc;
		ptrPolys->endX = *endX;
		*endX += endX_inc;
		ptrPolys->U = *U;
		*U += U_inc;
		ptrPolys->V = *V;
		*V += V_inc;
		ptrPolys->brightness = *brightness;
		*brightness += brightness_inc;
		ptrPolys++;
		*numLines = *numLines - 1;
	} while (*numLines);

	return ptrPolys;
}

void GameRenderHD::SetRenderThreads(uint8_t renderThreads)
{
	StopWorkerThreads();

	if (renderThreads < 0)
	{
		renderThreads = 0;
	}

	if (renderThreads > 0)
	{
		StartWorkerThreads(renderThreads, m_assignToSpecificCores);
	}
}

uint8_t GameRenderHD::GetRenderThreads()
{
	return m_renderThreads.size();
}

void GameRenderHD::WaitForRenderFinish()
{
	int taskCount = 0;

	do
	{
		uint8_t i = 0;
		taskCount = 0;
		for (i = 0; i < m_renderThreads.size(); i++)
		{
			taskCount += m_renderThreads[i]->GetIsTaskRunning()? 1 : 0;
		}
		//std::this_thread::sleep_for(std::chrono::milliseconds(50));
	} while (taskCount > 0);
}

int GameRenderHD::SumByte1WithByte2(int byte1, int byte2, uint8_t v180)
{
	BYTE1(byte1) += BYTE2(byte2) + v180;
	return byte1;
}
