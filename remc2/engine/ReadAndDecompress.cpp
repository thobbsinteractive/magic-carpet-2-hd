#include "ReadAndDecompress.h"
uint8_t BigTextureBuffer[128 * 128 * 160];

//----- (00054630) --------------------------------------------------------
void sub_54630_load_psxblock(uint16_t TextSize)//235630
{
	switch (TextSize)
	{
	case 16:
		DataFileIO::LoadFileArray_84250(psxadatablock16dat);
		break;
	case 32:
		DataFileIO::LoadFileArray_84250(psxadatablock32dat);
		break;
	case 128:
		break;
	}
}

void UpdateTileTextures(uint8_t tileSize, uint8_t* tileBuffer)
{
	int texture_addresses_index = 0;
	for (int ypos = 0; ypos < (signed int)(256 / (256 / tileSize)); ypos++)
	{
		for (int xpos = 0; xpos < (256 / tileSize); xpos++)
		{
			uint8_t* ptrTexture = (uint8_t*)((ypos * tileSize << 8) + (xpos * tileSize) + tileBuffer);

			x_DWORD_DDF50_texture_adresses.at(texture_addresses_index++) = ptrTexture;

			if (texture_addresses_index < 255)
			{
				EventDispatcher::I->DispatchEvent<ResourceType, uint32_t, const uint8_t*, uint32_t, uint32_t>(EventType::E_RESOURCE_CHANGE, ResourceType::TEXTURE_LOADED, texture_addresses_index, ptrTexture, tileSize, tileSize);
				//WriteTextureMapToBmp(texture_addresses_index, *xadatapald0dat2.colorPalette_var28, ptrTexture, tileSize, tileSize);
			}
		}
	}
}
//----- (00054660) --------------------------------------------------------
void sub_54660_read_and_decompress_sky_and_blocks(MapType_t GraphicsType, uint8_t GraphicsSize)//235660
{
	char dataPath[MAX_PATH];

	switch (GraphicsType)
	{
	case MapType_t::Day://basic graphics
	{
		switch (GraphicsSize)
		{
		case 16:
		{
			sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/BLOCK16.DAT");
			DataFileIO::ReadFileAndDecompress(dataPath, &BLOCK32DAT_BEGIN_BUFFER);//2bac2c
			break;
		}
		case 32:
		{
			sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/BLOCK32.DAT");
			DataFileIO::ReadFileAndDecompress(dataPath, &BLOCK32DAT_BEGIN_BUFFER);//2bac2c
			sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/SKYD0-0.DAT");
			DataFileIO::ReadFileAndDecompress(dataPath, &off_D41A8_sky);//2a51a8
			break;
		}
		case 128:
		{
			sprintf(dataPath, "%s/%s", highResGraphicsPath.c_str(), "block128.data");
			ReadGraphicsfile(dataPath, BigTextureBuffer);//advance graphics
			sprintf(dataPath, "%s/%s", highResGraphicsPath.c_str(), "skyd1024.data");
			ReadGraphicsfile(dataPath, off_D41A8_sky);//2a51a8
			break;
		}
		}
		sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/TMAPS0-0.TAB");
		DataFileIO::ReadFileAndDecompress(dataPath, (uint8_t**)&str_TMAPS00TAB_BEGIN_BUFFER);//2c7ed0
		break;
	}
	case MapType_t::Night://? and night
	{
		switch (GraphicsSize)
		{
		case 16:
		{
			if (D41A0_0.terrain_2FECE.byte_0x2FED2 & 2)
			{
				sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/BL16F0-0.DAT");
				DataFileIO::ReadFileAndDecompress(dataPath, &BLOCK32DAT_BEGIN_BUFFER);//2bac2c
			}
			else
			{
				sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/BL16N0-0.DAT");
				DataFileIO::ReadFileAndDecompress(dataPath, &BLOCK32DAT_BEGIN_BUFFER);//2bac2c
			}
			break;
		}
		case 32:
		{
			if (D41A0_0.terrain_2FECE.byte_0x2FED2 & 2)
			{
				sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/BL32F0-0.DAT");
				DataFileIO::ReadFileAndDecompress(dataPath, &BLOCK32DAT_BEGIN_BUFFER);//2bac2c
			}
			else
			{
				sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/BL32N0-0.DAT");
				DataFileIO::ReadFileAndDecompress(dataPath, &BLOCK32DAT_BEGIN_BUFFER);//2bac2c
			}
			sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/SKYN0-0.DAT");
			DataFileIO::ReadFileAndDecompress(dataPath, &off_D41A8_sky);//2a51a8
			break;
		}
		case 128:
		{
			if (D41A0_0.terrain_2FECE.byte_0x2FED2 & 2)
			{
				sprintf(dataPath, "%s/%s", highResGraphicsPath.c_str(), "bl128f0-0.data");
				ReadGraphicsfile(dataPath, BigTextureBuffer);//advance graphics
			}
			else
			{
				sprintf(dataPath, "%s/%s", highResGraphicsPath.c_str(), "bl128n0-0.data");
				ReadGraphicsfile(dataPath, BigTextureBuffer);//advance graphics
			}
			sprintf(dataPath, "%s/%s", highResGraphicsPath.c_str(), "skyn1024.data");
			ReadGraphicsfile(dataPath, off_D41A8_sky);//2a51a8
			break;
		}
		}
		sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/TMAPS1-0.TAB");
		DataFileIO::ReadFileAndDecompress(dataPath, (uint8_t**)&str_TMAPS00TAB_BEGIN_BUFFER);//2c7ed0
		break;
	}
	case MapType_t::Cave://cave
	{
		switch (GraphicsSize)
		{
		case 16:
		{
			sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/BL16C0-0.DAT");
			DataFileIO::ReadFileAndDecompress(dataPath, &BLOCK32DAT_BEGIN_BUFFER);//2bac2c
			break;
		}
		case 32:
		{
			sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/BL32C0-0.DAT");
			DataFileIO::ReadFileAndDecompress(dataPath, &BLOCK32DAT_BEGIN_BUFFER);//2bac2c
			break;
		}
		case 128:
		{
			sprintf(dataPath, "%s/%s", highResGraphicsPath.c_str(), "bl128c0-0.data");
			ReadGraphicsfile(dataPath, BigTextureBuffer);//advance graphics
			break;
		}
		}
		sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/TMAPS2-0.TAB");
		DataFileIO::ReadFileAndDecompress(dataPath, (uint8_t**)&str_TMAPS00TAB_BEGIN_BUFFER);//2c7ed0
		break;
	}
	}
	if (GraphicsSize < 128)
		UpdateTileTextures(GraphicsSize, BLOCK32DAT_BEGIN_BUFFER);
	else
		UpdateTileTextures(GraphicsSize, BigTextureBuffer);
}


//----- (00054800) --------------------------------------------------------
void sub_54800_read_and_decompress_tables(MapType_t a1)//235800
{
	char dataPath[MAX_PATH];

	if (a1 == MapType_t::Day)
	{
		sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/TABLESD.DAT");
		DataFileIO::ReadFileAndDecompress(dataPath, &x_BYTE_F6EE0_tablesx_pre);//2c7ee0
		keyColor2_D4B7E = 0x00;
		keyColor1_D4B7C = 0xfe;
	}
	else if (a1 == MapType_t::Night)
	{
		sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/TABLESN.DAT");
		DataFileIO::ReadFileAndDecompress(dataPath, &x_BYTE_F6EE0_tablesx_pre);
		keyColor2_D4B7E = 0xff;
		keyColor1_D4B7C = 0x00;
	}
	else if (a1 == MapType_t::Cave)
	{
		sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/TABLESC.DAT");
		DataFileIO::ReadFileAndDecompress(dataPath, &x_BYTE_F6EE0_tablesx_pre);
		keyColor1_D4B7C = 0xfe;
		keyColor2_D4B7E = 0xff;
	}
}

