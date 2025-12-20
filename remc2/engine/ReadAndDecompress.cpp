#include "ReadAndDecompress.h"
//----- (000AB9E1) --------------------------------------------------------
signed int sub_AB9E1_get_file_unpack_size(const char* path)//28c9e1
{
	uint8_t v2[10];
	int ret_size;
	FILE* file;
	ret_size = -1;
	file = DataFileIO::CreateOrOpenFile(path, 512);
	if (file == NULL)
		return 0;
	DataFileIO::Read(file, v2, 8);
	if (!strncmp((const char*)v2, (const char*)"RNC\1", 4))
	{
		ret_size = v2[4] << 8;//reverse size in rnc format
		ret_size += v2[5];
		ret_size <<= 8;
		ret_size += v2[6];
		ret_size <<= 8;
		ret_size += v2[7];
	}
	else
	{
		ret_size = DataFileIO::FileLengthBytes(file);
	}
	DataFileIO::Close(file);
	return ret_size;
}

//----- (0009A32D) --------------------------------------------------------
signed int UnpackAndLoadMemoryFromPath(Pathstruct path)//27B32d
{
	ClearMemoryForPath(path);
	if (path.path[0] == 0x2A)
	{
		if (path.var40_alloc_type & 1)
		{
			*(path.colorPalette_var28) = (uint8_t*)Malloc_83D70(path.var36_size_buffer);
			memset(*(path.colorPalette_var28), 0, path.var36_size_buffer);
		}
		else
		{
			*(path.colorPalette_var28) = (uint8_t*)Malloc_83CD0(path.var36_size_buffer);
			memset(*(path.colorPalette_var28), 0, path.var36_size_buffer);
		}
		if (!(*(path.colorPalette_var28)))
			return 0;
	}
	else
	{
		path.var36_size_buffer = (signed int)sub_AB9E1_get_file_unpack_size(path.path);
		if (path.var36_size_buffer <= 0)
			return 0;
		if (path.var40_alloc_type & 1)
		{
			*(path.colorPalette_var28) = (uint8_t*)Malloc_83D70(path.var36_size_buffer);//asi init a malloc bufferu
			memset(*(path.colorPalette_var28), 0, path.var36_size_buffer);
		}
		else
		{
			*(path.colorPalette_var28) = (uint8_t*)Malloc_83CD0(path.var36_size_buffer);//asi init a malloc bufferu
			memset(*(path.colorPalette_var28), 0, path.var36_size_buffer);
		}
		if (!(*(path.colorPalette_var28)))
			return -1;

		if (DataFileIO::ReadFileAndDecompress(path.path, path.colorPalette_var28) != path.var36_size_buffer)
		{
			path.colorPalette_var28 = 0;
			path.var32_end_buffer = 0;
			path.var36_size_buffer = 0;
			return 0;
		}
	}
	if (path.var32_end_buffer)
		(*path.var32_end_buffer) = (*path.colorPalette_var28) + path.var36_size_buffer;//fix ma byt 1
	return 1;
}

//----- (0009A2F5) --------------------------------------------------------
uint8_t* ClearMemoryForPath(Pathstruct path)//27B2f5
{
	uint8_t* result;
	result = *path.colorPalette_var28;
	if (result)
	{
		FreeMem_83E80(*path.colorPalette_var28);
		result = *path.colorPalette_var28;
		*result = 0;
	}
	return result;
}

uint8_t BigTextureBuffer[128 * 128 * 160];
//----- (00084250) --------------------------------------------------------
bool sub_84250_load_file_array(int psindex)//265250
{
	uint16_t result; // si
	__int16 v4; // ax
	int oldpsindex = psindex;
	result = 0;
	if (pstr[psindex].colorPalette_var28)
	{
		do
		{
			ClearMemoryForPath(pstr[psindex++]);
		} while (pstr[psindex].colorPalette_var28);
	}
	psindex = oldpsindex;
	while (pstr[psindex].colorPalette_var28)
	{
		v4 = UnpackAndLoadMemoryFromPath(pstr[psindex]);//27B32d - dalo mu to 1 - set 2bab20

		if (v4 >= 0)
		{
			if (v4)
			{
				psindex++;
				continue;
			}
			sub_41A90_VGA_Palette_install((TColor*)*xadatapald0dat2.colorPalette_var28);//install Palette for text mode(show error)
			myprintf("ERROR: File %s.\n");
		}
		else
		{
			sub_41A90_VGA_Palette_install((TColor*)*xadatapald0dat2.colorPalette_var28);//install Palette for text mode(show error)
			myprintf("ERROR: Allocation %s.\n");
		}
		myprintf("Press return to continue\n");
		result++;
		getc(stdin);
		psindex++;
	}
	return result;
}

//----- (00054630) --------------------------------------------------------
void sub_54630_load_psxblock(uint16_t TextSize)//235630
{
	switch (TextSize)
	{
	case 16:
		sub_84250_load_file_array(psxadatablock16dat);
		break;
	case 32:
		sub_84250_load_file_array(psxadatablock32dat);
		break;
	case 128:
		break;
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
			sprintf(dataPath, "%s/%s", bigGraphicsPath.c_str(), "block128.data");
			ReadGraphicsfile(dataPath, BigTextureBuffer);//advance graphics
			sprintf(dataPath, "%s/%s", bigGraphicsPath.c_str(), "skyd1024.data");
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
				sprintf(dataPath, "%s/%s", bigGraphicsPath.c_str(), "bl128f0-0.data");
				ReadGraphicsfile(dataPath, BigTextureBuffer);//advance graphics
			}
			else
			{
				sprintf(dataPath, "%s/%s", bigGraphicsPath.c_str(), "bl128n0-0.data");
				ReadGraphicsfile(dataPath, BigTextureBuffer);//advance graphics
			}
			sprintf(dataPath, "%s/%s", bigGraphicsPath.c_str(), "skyn1024.data");
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
			sprintf(dataPath, "%s/%s", bigGraphicsPath.c_str(), "bl128c0-0.data");
			ReadGraphicsfile(dataPath, BigTextureBuffer);//advance graphics
			break;
		}
		}
		sprintf(dataPath, "%s/%s", cdDataPath.c_str(), "DATA/TMAPS2-0.TAB");
		DataFileIO::ReadFileAndDecompress(dataPath, (uint8_t**)&str_TMAPS00TAB_BEGIN_BUFFER);//2c7ed0
		break;
	}
	}
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
