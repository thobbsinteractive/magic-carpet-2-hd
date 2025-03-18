#include "GameRenderHDSprites.h"

#include "../portability/bitmap_pos_struct.h"
#include "../utilities/RendererTests.h"
#include "Basic.h"
#include "CommandLineParser.h"
#include "ViewPort.h"
#include "defs.h"
#include "GameUI.h"

void DrawSprite_41BD3(
    uint32_t a1, 
    const int32_t spriteWidth,
    const char x_BYTE_F2CC6,
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
    )
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
				if ((unsigned int)str_F2C20ar.dword0x1e_spriteSymmetry <= 7)
				{
					// probably only two cases are handled, because roll is between +45deg and -45deg
					switch (str_F2C20ar.dword0x1e_spriteSymmetry)//mirroring
					{
					case 0:
						// left rotation 0-45deg roll
						if (CommandLineParams.DoTestRenderers()) { renderer_tests_register_hit(RendererTestsHitCheckpoint::RendTest_HD_Draw_Sprite_0); }

						v156 = str_F2C20ar.dword0x1b_cosRoll * str_F2C20ar.dword0x09_realWidth >> 16;
						if (v156 <= 0)
							break;
						scaledHeight = (str_F2C20ar.dword0x0c_realHeight << 16) / str_F2C20ar.dword0x1b_cosRoll;
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
						v160 = str_F2C20ar.dword0x27_sinRoll * str_F2C20ar.dword0x0c_realHeight / scaledHeight;
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
						// ?deg roll
						if (CommandLineParams.DoTestRenderers()) { renderer_tests_register_hit(RendererTestsHitCheckpoint::RendTest_HD_Draw_Sprite_1); }

						v156 = str_F2C20ar.dword0x27_sinRoll * str_F2C20ar.dword0x09_realWidth >> 16;
						if (v156 <= 0)
							break;
						scaledHeight = (str_F2C20ar.dword0x0c_realHeight << 16) / str_F2C20ar.dword0x27_sinRoll;
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
						v161 = str_F2C20ar.dword0x1b_cosRoll * str_F2C20ar.dword0x0c_realHeight / scaledHeight;
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
						// ?deg roll
						if (CommandLineParams.DoTestRenderers()) { renderer_tests_register_hit(RendererTestsHitCheckpoint::RendTest_HD_Draw_Sprite_2); }

						v156 = str_F2C20ar.dword0x1b_cosRoll * str_F2C20ar.dword0x09_realWidth >> 16;
						if (v156 <= 0)
							break;
						scaledHeight = (str_F2C20ar.dword0x0c_realHeight << 16) / str_F2C20ar.dword0x1b_cosRoll;
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
						v160 = str_F2C20ar.dword0x27_sinRoll * str_F2C20ar.dword0x0c_realHeight / scaledHeight;
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
						// ?deg roll
						if (CommandLineParams.DoTestRenderers()) { renderer_tests_register_hit(RendererTestsHitCheckpoint::RendTest_HD_Draw_Sprite_3); }

						v156 = str_F2C20ar.dword0x27_sinRoll * str_F2C20ar.dword0x09_realWidth >> 16;
						if (v156 <= 0)
							break;
						scaledHeight = (str_F2C20ar.dword0x0c_realHeight << 16) / str_F2C20ar.dword0x27_sinRoll;
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
						v161 = str_F2C20ar.dword0x1b_cosRoll * str_F2C20ar.dword0x0c_realHeight / scaledHeight;
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
						// ?deg roll
						if (CommandLineParams.DoTestRenderers()) { renderer_tests_register_hit(RendererTestsHitCheckpoint::RendTest_HD_Draw_Sprite_4); }

						v156 = str_F2C20ar.dword0x1b_cosRoll * str_F2C20ar.dword0x09_realWidth >> 16;
						if (v156 <= 0)
							break;
						scaledHeight = (str_F2C20ar.dword0x0c_realHeight << 16) / str_F2C20ar.dword0x1b_cosRoll;
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
						v160 = str_F2C20ar.dword0x27_sinRoll * str_F2C20ar.dword0x0c_realHeight / scaledHeight;
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
						// ?deg roll
						if (CommandLineParams.DoTestRenderers()) { renderer_tests_register_hit(RendererTestsHitCheckpoint::RendTest_HD_Draw_Sprite_5); }

						v156 = str_F2C20ar.dword0x27_sinRoll * str_F2C20ar.dword0x09_realWidth >> 16;
						if (v156 <= 0)
							break;
						scaledHeight = (str_F2C20ar.dword0x0c_realHeight << 16) / str_F2C20ar.dword0x27_sinRoll;
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
						v161 = str_F2C20ar.dword0x1b_cosRoll * str_F2C20ar.dword0x0c_realHeight / scaledHeight;
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
						// ?deg roll
						if (CommandLineParams.DoTestRenderers()) { renderer_tests_register_hit(RendererTestsHitCheckpoint::RendTest_HD_Draw_Sprite_6); }

						v156 = str_F2C20ar.dword0x1b_cosRoll * str_F2C20ar.dword0x09_realWidth >> 16;
						if (v156 <= 0)
							break;
						scaledHeight = (str_F2C20ar.dword0x0c_realHeight << 16) / str_F2C20ar.dword0x1b_cosRoll;
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
						v160 = str_F2C20ar.dword0x27_sinRoll * str_F2C20ar.dword0x0c_realHeight / scaledHeight;
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
									v121x = &str_F2C20ar.dword0x02_data[spriteWidth * (str_F2C20ar.dword0x0a_actIdx >> 16)];

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
						// right rotation 0-45 deg roll
						if (CommandLineParams.DoTestRenderers()) { renderer_tests_register_hit(RendererTestsHitCheckpoint::RendTest_HD_Draw_Sprite_7); }

						v156 = str_F2C20ar.dword0x27_sinRoll * str_F2C20ar.dword0x09_realWidth >> 16;

						if (v156 <= 0)
							break;
						scaledHeight = (str_F2C20ar.dword0x0c_realHeight << 16) / str_F2C20ar.dword0x27_sinRoll;
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
						v161 = str_F2C20ar.dword0x1b_cosRoll * str_F2C20ar.dword0x0c_realHeight / scaledHeight;
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
						DrawSorcererNameAndHealthBar_2CB30(
                            str_F2C20ar.dword0x14x, str_F2C20ar.dword0x04_screenY, (int16_t)str_F2C20ar.dword0x03_screenX, str_F2C20ar.dword0x09_realWidth,
	                        m_ptrColorPalette[0]
                        );
					}
					if (x_D41A0_BYTEARRAY_4_struct.showHelp_10)
						DrawSpriteHelp_sub_88740(
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
				DrawSpriteHelp_sub_88740(
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
						//v2 = (char*)(spriteWidth * (str_F2C20ar.dword0x0a_actIdx >> 16) + (int)str_F2C20ar.dword0x02_data + *(x_DWORD*)(m_ptrDWORD_E9C38_smalltit + 36964));//sprite
						ptrSpriteRenderSrc_v2x = &str_F2C20ar.dword0x02_data[spriteWidth * (str_F2C20ar.dword0x0a_actIdx >> 16) + *(x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 + 4)];//sprite
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
						ptrSpriteRenderSrc_v14 = (x_BYTE*)(spriteWidth * (str_F2C20ar.dword0x0a_actIdx >> 16) + str_F2C20ar.dword0x02_data + *(x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 + 4));
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
						ptrSpriteRenderSrc_v18 = (x_BYTE*)(spriteWidth * (str_F2C20ar.dword0x0a_actIdx >> 16) + str_F2C20ar.dword0x02_data + *(x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 + 4));
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
						ptrSpriteRenderSrc_v23 = (x_BYTE*)(spriteWidth * (str_F2C20ar.dword0x0a_actIdx >> 16) + str_F2C20ar.dword0x02_data + *(x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 + 4));//from mask
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
						ptrSpriteRenderSrc_v28 = (x_BYTE*)(spriteWidth * (str_F2C20ar.dword0x0a_actIdx >> 16) + str_F2C20ar.dword0x02_data + *(x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 + 4));
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
						ptrSpriteRenderSrc_v34 = (x_BYTE*)(spriteWidth * (str_F2C20ar.dword0x0a_actIdx >> 16) + str_F2C20ar.dword0x02_data + *(x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 + 4));
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
						ptrSpriteRenderSrc_v39 = (x_BYTE*)(spriteWidth * (str_F2C20ar.dword0x0a_actIdx >> 16) + str_F2C20ar.dword0x02_data + *(x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 + 4));
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
						ptrSpriteRenderSrc_v45 = (x_BYTE*)(spriteWidth * (str_F2C20ar.dword0x0a_actIdx >> 16) + str_F2C20ar.dword0x02_data + *(x_DWORD*)(m_ptrDWORD_E9C38_smalltit + m_bufferOffset_E9C38_1 + 4));
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

