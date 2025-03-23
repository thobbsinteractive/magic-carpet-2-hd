#include <cstdint>

#include <gtest/gtest.h>
#include "../../remc2/engine/Basic.h"
#include "../../remc2/engine/GameRenderHDSprites.h"
#include "../../remc2/engine/GameRenderOriginalSprites.h"

TEST(GameRenderHD, DrawSprite)
{
	// Arrange:
    //   - GameRenderInterface
    //   - set up 640x480 screen buffer
    //   - set up sprite data

    int width = 640;
    int height = 480;
    uint8_t* pdwScreenBuffer_351628 = (uint8_t*)malloc((width * height) * 3);

    //GameRenderInterface* pRenderHD = (GameRenderInterface*)new GameRenderHD(pdwScreenBuffer_351628, *xadatapald0dat2.colorPalette_var28, 0, false);
    //GameRenderInterface* pRenderOriginal = (GameRenderInterface*)new GameRenderOriginal();

	//str_F2C20ar.cos_0x11 = 65536;
	//Str_E9C38_smalltit[0].pnt1_16 = 33637;
	//str_F2C20ar.sin_0x0d = 0;
	//Str_E9C38_smalltit[0].pnt2_20 = -9735;
	//str_F2C20ar.dword0x24 = 960;

	// Run original code
	//DrawSprite_41BD3(uint32 a1);

	// Run HD code
    //GameRenderHD::DrawSprite_41BD3(uint32 a1, const int32_t spriteWidth);


//void DrawSprite_41BD3(
//    uint32_t a1, 
//    const int32_t spriteWidth,
//    const char x_BYTE_F2CC6,
//    type_F2C20ar &str_F2C20ar,
//	uint8_t* m_ptrDWORD_E9C38_smalltit,
//	const int m_bufferOffset_E9C38_1,
//	const int m_bufferOffset_E9C38_2,
//	const int m_bufferOffset_E9C38_3,
//    const int iScreenWidth_DE560,
//    uint8_t* ViewPortRenderBufferStart_DE558,
//	type_unk_F0E20x m_str_F0E20x[GAME_RES_MAX_WIDTH + 100],
//    uint8_t x_BYTE_F6EE0_tablesx[83456],
//    type_x_D41A0_BYTEARRAY_4_struct x_D41A0_BYTEARRAY_4_struct,
//	const uint8_t* m_ptrColorPalette
//    )
//{

	// Check results match
	//ASSERT_EQ(pnt1_16o, -30939);
	//ASSERT_EQ(pnt1_16n, 34597);
}

//			std::string help_buffer_name;
//			std::string screenbuffer_buffer_name;
//			if (typeid(*m_ptrGameRender) == typeid(GameRenderHD))
//			{
//				delete m_ptrGameRender;
//				m_ptrGameRender = nullptr;
//				m_ptrGameRender = (GameRenderInterface*)new GameRenderOriginal();
//				help_buffer_name = "ScreenBuffer_HD.bmp";
//				screenbuffer_buffer_name = "ScreenBuffer_Original.bmp";
//			}
//			else
//			{
//				delete m_ptrGameRender;
//				m_ptrGameRender = nullptr;
//				m_ptrGameRender = (GameRenderInterface*)new GameRenderHD(pdwScreenBuffer_351628, *xadatapald0dat2.colorPalette_var28, (multiThreadedRender ? numberOfRenderThreads : 0), assignToSpecificCores);
//				help_buffer_name = "ScreenBuffer_Original.bmp";
//				screenbuffer_buffer_name = "ScreenBuffer_HD.bmp";
//			}

//			int difference = 0;
//			for (int test_compi = 0; test_compi < screenWidth_18062C * screenHeight_180624; test_compi++) {
//				if (pdwScreenBuffer_351628[test_compi] != help_ScreenBuffer[test_compi]) {
//					difference++;
//				}
//			}
//
//			if (difference > 0) {
//				std::ostringstream screenBufferName;
//				screenBufferName << "Frame-" << renderer_tests_frame_count << "-Level-" << CommandLineParams.GetSetLevel() << "-" << screenbuffer_buffer_name;
//				std::ostringstream helpScreenBufferName;
//				helpScreenBufferName << "Frame-" << renderer_tests_frame_count << "-Level-" << CommandLineParams.GetSetLevel() << "-" << help_buffer_name;
//
//				renderer_tests[CommandLineParams.GetSetLevel()].differences += difference;
//				Logger->error("Differences between HD and Original renderer in frame {0}: {1}", renderer_tests_frame_count, difference);
//				WriteBufferToBMP(screenWidth_18062C, screenHeight_180624, *xadatapald0dat2.colorPalette_var28, pdwScreenBuffer_351628, screenBufferName.str());
//				WriteBufferToBMP(screenWidth_18062C, screenHeight_180624, *xadatapald0dat2.colorPalette_var28, help_ScreenBuffer, helpScreenBufferName.str());
//			}
//
//			if (typeid(*m_ptrGameRender) == typeid(GameRenderHD))
//			{
//				delete m_ptrGameRender;
//				m_ptrGameRender = nullptr;
//				m_ptrGameRender = (GameRenderInterface*)new GameRenderOriginal();
//			}
//			else
//			{
//				delete m_ptrGameRender;
//				m_ptrGameRender = nullptr;
//				m_ptrGameRender = (GameRenderInterface*)new GameRenderHD(pdwScreenBuffer_351628, *xadatapald0dat2.colorPalette_var28, (multiThreadedRender ? numberOfRenderThreads : 0), assignToSpecificCores);
//			}

//GameRenderInterface* m_ptrGameRender;
//
//uint8_t loc_A0000_vga_buffer[307200];
//TColor x_DWORD_EA3B8x[256]; // weak?x_DWORD_E9C4C_langindexbuffer[475]
//int16_t x_WORD_180662_graphics_handle; // weak
//bitmap_pos_struct2_t* x_DWORD_D4188 = 0; // weak
//bitmap_pos_struct2_t* x_DWORD_D418C = 0; // weak
//bitmap_pos_struct2_t* x_DWORD_D4190 = 0; // weak
//
//
////----- (000A0D50) --------------------------------------------------------
//void sub_A0D50_set_viewport(uint16_t posX, uint16_t posY, uint16_t width, uint16_t height)//281d50
//{
//	x_DWORD_18063C_sprite_sizex = posX;
//	x_DWORD_180650_positiony = posY;
//	x_DWORD_180648_map_resolution2_x = width;
//	x_DWORD_180644_map_resolution2_y = height;
//	x_DWORD_180634_screen_width = width + posX;
//	x_DWORD_180630_screen_height = height + posY;
//}
