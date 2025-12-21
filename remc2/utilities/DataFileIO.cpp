#include "DataFileIO.h"

#include "../engine/Basic.h"
#include "../engine/engine_support.h"

#ifndef _countof
#define _countof(arr) sizeof(arr) / sizeof(arr[0])
//dirty
#endif

uint16_t DataFileIO::crc_table[256] = {
		0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
		0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
		0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
		0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
		0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
		0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
		0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
		0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
		0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
		0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
		0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
		0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
		0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
		0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
		0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
		0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
		0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
		0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
		0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
		0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
		0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
		0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
		0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
		0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
		0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
		0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
		0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
		0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
		0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
		0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
		0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
		0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

Pathstruct pstr[100] = {
xasearchd_2bac30 ,//00
xafonts0dat,//01
xafonts0tab,//02
xafonts1dat,//03
xafonts1tab,//04
xadatatmaps00tab,//05
xapaldata,//06
xapalmem,//07
xadatapointersdat,//08
xadatapointerstab,//09
xazero,//10-a
xadatabuild00dat,//11-b
xadatabuild00tab,//12-c
xazero,//13
xadataetextdat,//14
xazero,//15
xadataftextdat,//16
xazero,//17
xadatagtextdat,//18
xazero,//19
xadataitextdat,//20
xazero,//21
xadatablock16dat,//22
xadatablock32dat,//23
xazero,//24
xabscreen,//25
xadatapald0dat,//26
xazero,//27
xawscreen_351628,//28
xabscreen2,//29
xadatapald0dat2,//30
xazero,//31
xadatamsprd00dat,//32
xadatamsprd00tab,//33
xazero,//34
xadatahsprd00dat,//35
xadatahsprd00tab,//36
xadatahfont3dat,//37
xadatahfont3tab,//38
xazero,//39
xadataclrd0dat,//40
xazero,//41
xadataspellsdatx,//42
xazero };//43

void DataFileIO::SetCDFilePaths(const char* cdDataPath, Pathstruct pstr[])
{
	sprintf(pstr[psxafonts0dat].path, "%s/%s", cdDataPath, "DATA/FONT0.DAT\0");
	sprintf(pstr[psxafonts0tab].path, "%s/%s", cdDataPath, "DATA/FONT0.TAB\0");
	sprintf(pstr[psxafonts1dat].path, "%s/%s", cdDataPath, "DATA/FONT1.DAT\0");
	sprintf(pstr[psxafonts1tab].path, "%s/%s", cdDataPath, "DATA/FONT1.TAB\0");
	sprintf(pstr[psxadatatmaps00tab].path, "%s/%s", cdDataPath, "DATA/TMAPS0-0.TAB");
	sprintf(pstr[psxadatapointersdat].path, "%s/%s", cdDataPath, "DATA/POINTERS.DAT\0");
	sprintf(pstr[psxadatapointerstab].path, "%s/%s", cdDataPath, "DATA/POINTERS.TAB\0");
	sprintf(pstr[psxadatabuild00dat].path, "%s/%s", cdDataPath, "DATA/BUILD0-0.DAT\0");
	sprintf(pstr[psxadatabuild00tab].path, "%s/%s", cdDataPath, "DATA/BUILD0-0.TAB\0");
	sprintf(pstr[psxadataetextdat].path, "%s/%s", cdDataPath, "DATA/ETEXT.DAT\0");
	sprintf(pstr[psxadataftextdat].path, "%s/%s", cdDataPath, "DATA/FTEXT.DAT\0");
	sprintf(pstr[psxadatagtextdat].path, "%s/%s", cdDataPath, "DATA/GTEXT.DAT\0");
	sprintf(pstr[psxadataitextdat].path, "%s/%s", cdDataPath, "DATA/ITEXT.DAT\0");
	sprintf(pstr[psxadatablock16dat].path, "%s/%s", cdDataPath, "DATA/BLOCK16.DAT\0");
	sprintf(pstr[psxadatablock32dat].path, "%s/%s", cdDataPath, "DATA/BLOCK32.DAT\0");
	sprintf(pstr[psxadatapald0dat].path, "%s/%s", cdDataPath, "DATA/PALD-0.DAT\0");
	sprintf(pstr[psxadatapald0dat2].path, "%s/%s", cdDataPath, "DATA/PALD-0.DAT\0");
	sprintf(pstr[psxadatamsprd00dat].path, "%s/%s", cdDataPath, "DATA/MSPRD0-0.DAT\0");
	sprintf(pstr[psxadatamsprd00tab].path, "%s/%s", cdDataPath, "DATA/MSPRD0-0.TAB\0");
	sprintf(pstr[psxadatahsprd00dat].path, "%s/%s", cdDataPath, "DATA/HSPRD0-0.DAT\0");
	sprintf(pstr[psxadatahsprd00tab].path, "%s/%s", cdDataPath, "DATA/HSPRD0-0.TAB\0");
	sprintf(pstr[psxadatahfont3dat].path, "%s/%s", cdDataPath, "DATA/HFONT3.DAT\0");
	sprintf(pstr[psxadatahfont3tab].path, "%s/%s", cdDataPath, "DATA/HFONT3.TAB\0");
	sprintf(pstr[psxadataclrd0dat].path, "%s/%s", cdDataPath, "DATA/CLRD-0.DAT\0");
	sprintf(pstr[psxadataspellsdat].path, "%s/%s", cdDataPath, "DATA/SPELLS.DAT\0");
}


DataFileIO::DataFileIO() {};

int DataFileIO::ReadFileAndDecompress(const char* path, uint8_t** data)
{
	int result = 0; // eax // length of file or 0 on error
	FILE* file; // ebx
	uint32_t length; // esi
	file = CreateOrOpenFile((char*)path, 0x200);

	if (file)
	{
		length = FileLengthBytes(file);
		Read(file, *data, length);
		Close(file);
		result = (int)Decompress(*data, *data);
		if (result >= 0)
		{
			if (!result)
				result = length;
		}
		else
		{
			Logger->error("ERROR decompressing");
			result = -2;
		}
	}
	return result;
}

int DataFileIO::Decompress(uint8_t* src, uint8_t* a2) {
	vars_t* v = init_vars();
	if (v->method == 1)
	{
		if (v->dict_size > 0x8000)
			v->dict_size = 0x8000;
		v->max_matches = 0x1000;
	}
	else if (v->method == 2)
	{
		if (v->dict_size > 0x1000)
			v->dict_size = 0x1000;
		v->max_matches = 0xFF;
	}

	v->file_size = MAX_BUF_SIZE;
	v->input = (uint8_t*)malloc(MAX_BUF_SIZE);

	uint32_t signature = src[0] + (src[1] << 8) + (src[2] << 16);
	if (signature == 0x434e52)
	{
		uint32_t inputsize = src[11] + (src[10] << 8) + (src[9] << 16) + (src[8] << 24);
		memmove(v->input, src, inputsize + 0x12);
	}
	else
	{
		v->input[0] = 0;
	}
	v->output = a2;
	v->temp = (uint8_t*)malloc(MAX_BUF_SIZE);

	int error_code = 0;
	error_code = Unpack(v);

	if (v->pus_mode == 2)
	{
		free(v->input);
		free(v->output);
		free(v->temp);
		free(v->input);
		free(v);

		return error_code;
	}

	free(v->input);
	free(v->temp);
	free(v);
	if (error_code == -6)return 0;
	return error_code;
}

DataFileIO::vars_t* DataFileIO::init_vars()
{
	vars_t* v = (vars_t*)malloc(sizeof(vars_t));
	v->enc_key = 0;
	v->max_matches = 0x1000;
	v->unpacked_crc_real = 0;
	v->pack_block_size = 0x3000;
	v->dict_size = 0xFFFF;
	v->method = 1;
	v->pus_mode = 0;

	v->read_start_offset = 0;
	v->write_start_offset = 0;
	v->input_offset = 0;
	v->output_offset = 0;
	v->temp_offset = 0;

	memset(v->tmp_crc_data, 0, sizeof(v->tmp_crc_data));
	memset(v->raw_table, 0, sizeof(v->raw_table));
	memset(v->pos_table, 0, sizeof(v->pos_table));
	memset(v->len_table, 0, sizeof(v->len_table));

	return v;
}

int DataFileIO::UnpackData(vars_t* v)
{
	int start_pos = v->input_offset;

	uint32_t sign = read_dword_be(v->input, &v->input_offset);
	if ((sign >> 8) != RNC_SIGN)
		return 6;

	v->method = sign & 3;
	v->input_size = read_dword_be(v->input, &v->input_offset);
	v->packed_size = read_dword_be(v->input, &v->input_offset);
	if (v->file_size < v->packed_size)
		return 7;
	v->unpacked_crc = read_word_be(v->input, &v->input_offset);
	v->packed_crc = read_word_be(v->input, &v->input_offset);

	/*v->leeway = */read_byte(v->input, &v->input_offset);
	/*v->chunks_count = */read_byte(v->input, &v->input_offset);

	if (crc_block(v->input, v->input_offset, v->packed_size) != v->packed_crc)
		return 4;

	v->mem1 = (uint8_t*)malloc(65536);
	v->decoded = (uint8_t*)malloc(65536);
	v->pack_block_start = &v->mem1[65533];
	v->window = &v->decoded[v->dict_size];

	v->unpacked_crc_real = 0;
	v->bit_count = 0;
	v->bit_buffer = 0;
	v->processed_size = 0;

	uint16_t specified_key = v->enc_key;

	int error_code = 0;
	if (input_bits(v, 1) && !v->pus_mode)
		error_code = 9;

	if (!error_code)
	{
		if (input_bits(v, 1) && !v->enc_key) // key is needed, but not specified as argument
			error_code = 10;
	}

	if (!error_code)
	{
		switch (v->method)
		{
		case 1: error_code = unpack_data_m1(v); break;
		case 2: error_code = unpack_data_m2(v); break;
		}
	}

	v->enc_key = specified_key;

	free(v->mem1);
	free(v->decoded);

	v->input_offset = start_pos + v->packed_size + RNC_HEADER_SIZE;

	if (error_code)
		return error_code;

	if (v->unpacked_crc != v->unpacked_crc_real)
		return 5;

	return 0;
}

int DataFileIO::Unpack(vars_t* v)
{
	v->packed_size = v->file_size;

	if (v->file_size < RNC_HEADER_SIZE)
		return 6;

	int result = UnpackData(v); // data
	if (result == 0)return v->input_size;
	return -result;
}

FILE* DataFileIO::CreateOrOpenFile(const char* pathname, int __pmode)
{
	FILE* file; // ST10_4


	if (__pmode == 0x222)
	{
#ifdef _MSC_VER
		file = CreateFile(pathname, GENERIC_ALL);
#else
		file = CreateFile(pathname, 0x1c0);
#endif
		//x_setmode(v2, 0x200);
		if (file) {
			Close(file);
		}
	}
	return Open(pathname, __pmode, 0x40);
}

FILE* DataFileIO::CreateFile(const char* path, uint32_t flags)
{
	return mycreate(path, flags);
}

FILE* DataFileIO::Open(const char* path, int pmode, uint32_t flags) {
	return myopen(path, pmode, flags);
}

int32_t DataFileIO::Close(FILE* file) {
	return myclose(file);
}

int32_t DataFileIO::Seek(FILE* file, x_DWORD position, char type) {
	return mylseek(file, position, type);
}

size_t DataFileIO::Read(FILE* file, uint8_t* data, uint32_t length) {
	size_t result = fread(data, 1, length, file);
	Logger->trace("Read fread length {} result {}", length, result);
	return result;
};

long DataFileIO::FileLengthBytes(FILE* file) 
{
	long size;
	myfseek(file, 0, SEEK_END); // seek to end of file
	size = myftell(file); // get current file pointer
	myfseek(file, 0, SEEK_SET); // seek back to beginning of file
	return size;
}

uint8_t DataFileIO::read_byte(uint8_t* buf, size_t* offset)
{
	return buf[(*offset)++];
}

uint16_t DataFileIO::read_word_be(uint8_t* buf, size_t* offset)
{
	uint8_t b1 = read_byte(buf, offset);
	uint8_t b2 = read_byte(buf, offset);

	return (b1 << 8) | b2;
}

uint32_t DataFileIO::read_dword_be(uint8_t* buf, size_t* offset)
{
	uint16_t w1 = read_word_be(buf, offset);
	uint16_t w2 = read_word_be(buf, offset);

	return (w1 << 16) | w2;
}

int DataFileIO::unpack_data_m1(vars_t* v)
{
	while (v->processed_size < v->input_size)
	{
		make_huftable(v, v->raw_table, _countof(v->raw_table));
		make_huftable(v, v->len_table, _countof(v->len_table));
		make_huftable(v, v->pos_table, _countof(v->pos_table));

		int subchunks = input_bits_m1(v, 16);

		while (subchunks--)
		{
			uint32_t data_length = decode_table_data(v, v->raw_table);
			v->processed_size += data_length;

			if (data_length)
			{
				while (data_length--)
					write_decoded_byte(v, (v->enc_key ^ read_source_byte(v)) & 0xFF);

				ror_w(&v->enc_key);

				// FIXME: runtime error: left shift of 8355742 by 11 places cannot be represented in type 'int'
				//        using int64_t is just a quick an dirty hack
				int64_t fix_overflow = ((v->pack_block_start[2] << 16) | (v->pack_block_start[1] << 8) | v->pack_block_start[0]);
				v->bit_buffer = (fix_overflow << v->bit_count) | (v->bit_buffer & ((1 << v->bit_count) - 1));
			}

			if (subchunks)
			{
				v->match_offset = decode_table_data(v, v->len_table) + 1;
				v->match_count = decode_table_data(v, v->pos_table) + 2;
				v->processed_size += v->match_count;

				while (v->match_count--)
					write_decoded_byte(v, v->window[-v->match_offset]);
			}
		}
	}

	write_buf(v->output, &v->output_offset, &v->decoded[v->dict_size], v->window - &v->decoded[v->dict_size]);
	return 0;
}

int DataFileIO::unpack_data_m2(vars_t* v)
{
	while (v->processed_size < v->input_size)
	{
		while (1)
		{
			if (!input_bits_m2(v, 1))
			{
				write_decoded_byte(v, (v->enc_key ^ read_source_byte(v)) & 0xFF);

				ror_w(&v->enc_key);

				v->processed_size++;
			}
			else
			{
				if (input_bits_m2(v, 1))
				{
					if (input_bits_m2(v, 1))
					{
						if (input_bits_m2(v, 1))
						{
							v->match_count = read_source_byte(v) + 8;

							if (v->match_count == 8)
							{
								input_bits_m2(v, 1);
								break;
							}
						}
						else
							v->match_count = 3;

						decode_match_offset(v);
					}
					else
					{
						v->match_count = 2;
						v->match_offset = read_source_byte(v) + 1;
					}

					v->processed_size += v->match_count;

					while (v->match_count--)
						write_decoded_byte(v, v->window[-v->match_offset]);
				}
				else
				{
					decode_match_count(v);

					if (v->match_count != 9)
					{
						decode_match_offset(v);
						v->processed_size += v->match_count;

						while (v->match_count--)
							write_decoded_byte(v, v->window[-v->match_offset]);
					}
					else
					{
						uint32_t data_length = (input_bits_m2(v, 4) << 2) + 12;
						v->processed_size += data_length;

						while (data_length--)
							write_decoded_byte(v, (v->enc_key ^ read_source_byte(v)) & 0xFF);

						ror_w(&v->enc_key);
					}
				}
			}
		}
	}

	write_buf(v->output, &v->output_offset, &v->decoded[v->dict_size], v->window - &v->decoded[v->dict_size]);
	return 0;
}

void DataFileIO::decode_match_count(vars_t* v)
{
	v->match_count = input_bits_m2(v, 1) + 4;

	if (input_bits_m2(v, 1))
		v->match_count = ((v->match_count - 1) << 1) + input_bits_m2(v, 1);
}

void DataFileIO::decode_match_offset(vars_t* v)
{
	v->match_offset = 0;
	if (input_bits_m2(v, 1))
	{
		v->match_offset = input_bits_m2(v, 1);

		if (input_bits_m2(v, 1))
		{
			v->match_offset = ((v->match_offset << 1) | input_bits_m2(v, 1)) | 4;

			if (!input_bits_m2(v, 1))
				v->match_offset = (v->match_offset << 1) | input_bits_m2(v, 1);
		}
		else if (!v->match_offset)
			v->match_offset = input_bits_m2(v, 1) + 2;
	}

	v->match_offset = ((v->match_offset << 8) | read_source_byte(v)) + 1;
}

uint16_t DataFileIO::crc_block(uint8_t* buf, size_t offset, int size)
{
	uint16_t crc = 0;

	while (size--)
	{
		crc ^= read_byte(buf, &offset);
		crc = (crc >> 8) ^ crc_table[crc & 0xFF];
	}

	return crc;
}

uint32_t DataFileIO::input_bits_m1(vars_t* v, short count)
{
	uint32_t bits = 0;
	uint32_t prev_bits = 1;

	while (count--)
	{
		if (!v->bit_count)
		{
			uint8_t b1 = read_source_byte(v);
			uint8_t b2 = read_source_byte(v);
			v->bit_buffer = (v->pack_block_start[1] << 24) | (v->pack_block_start[0] << 16) | (b2 << 8) | b1;

			v->bit_count = 16;
		}

		if (v->bit_buffer & 1)
			bits |= prev_bits;

		v->bit_buffer >>= 1;
		prev_bits <<= 1;
		v->bit_count--;
	}

	return bits;
}

uint32_t DataFileIO::input_bits_m2(vars_t* v, short count)
{
	uint32_t bits = 0;

	while (count--)
	{
		if (!v->bit_count)
		{
			v->bit_buffer = read_source_byte(v);
			v->bit_count = 8;
		}

		bits <<= 1;

		if (v->bit_buffer & 0x80)
			bits |= 1;

		v->bit_buffer <<= 1;
		v->bit_count--;
	}

	return bits;
}

int DataFileIO::input_bits(vars_t* v, short count)
{
	if (v->method != 2)
		return input_bits_m1(v, count);
	else
		return input_bits_m2(v, count);
}

void DataFileIO::write_decoded_byte(vars_t* v, uint8_t b)
{
	if (&v->decoded[65535] == v->window)
	{
		write_buf(v->output, &v->output_offset, &v->decoded[v->dict_size], 65535 - v->dict_size);
		memmove(v->decoded, &v->window[-v->dict_size], v->dict_size);
		v->window = &v->decoded[v->dict_size];
	}

	*v->window++ = b;
	v->unpacked_crc_real = crc_table[(v->unpacked_crc_real ^ b) & 0xFF] ^ (v->unpacked_crc_real >> 8);
}

void DataFileIO::write_buf(uint8_t* dest, size_t* offset, uint8_t* source, int size)
{
	memmove(&dest[*offset], source, size);
	*offset += size;
}

uint8_t DataFileIO::read_source_byte(vars_t* v)
{
	if (v->pack_block_start == &v->mem1[65533])
	{
		int left_size = v->file_size - v->input_offset;

		int size_to_read;
		if (left_size <= 65533)
			size_to_read = left_size;
		else
			size_to_read = 65533;

		v->pack_block_start = v->mem1;

		read_buf(v->pack_block_start, v->input, &v->input_offset, size_to_read);

		if (left_size - size_to_read > 2)
			left_size = 2;

		read_buf(&v->mem1[size_to_read], v->input, &v->input_offset, left_size);
		v->input_offset -= left_size;
	}

	return *v->pack_block_start++;
}

void DataFileIO::read_buf(uint8_t* dest, uint8_t* source, size_t* offset, int size)
{
	memmove(dest, &source[*offset], size);
	*offset += size;
}

void DataFileIO::ror_w(uint16_t* x)
{
	if (*x & 1)
		*x = 0x8000 | (*x >> 1);
	else
		*x >>= 1;
}

uint32_t  DataFileIO::inverse_bits(uint32_t value, int count)
{
	int i = 0;
	while (count--)
	{
		i <<= 1;

		if (value & 1)
			i |= 1;

		value >>= 1;
	}

	return i;
}

void  DataFileIO::proc_20(huftable_t* data, int count)
{
	int val = 0;
	uint32_t div = 0x80000000;
	int bits_count = 1;

	while (bits_count <= 16)
	{
		int i = 0;

		while (1)
		{
			if (i >= count)
			{
				bits_count++;
				div >>= 1;
				break;
			}

			if (data[i].bit_depth == bits_count)
			{
				data[i].l3 = inverse_bits(val / div, bits_count);
				val += div;
			}

			i++;
		}
	}
}

void  DataFileIO::make_huftable(vars_t* v, huftable_t* data, int count)
{
	clear_table(data, count);

	int leaf_nodes = input_bits_m1(v, 5);

	if (leaf_nodes)
	{
		if (leaf_nodes > 16)
			leaf_nodes = 16;

		for (int i = 0; i < leaf_nodes; ++i)
			data[i].bit_depth = input_bits_m1(v, 4);

		proc_20(data, leaf_nodes);
	}
}

uint32_t DataFileIO::decode_table_data(vars_t* v, huftable_t* data)
{
	uint32_t i = 0;

	while (1)
	{
		if (data[i].bit_depth && (data[i].l3 == (v->bit_buffer & ((1 << data[i].bit_depth) - 1))))
		{
			input_bits_m1(v, data[i].bit_depth);

			if (i < 2)
				return i;

			return input_bits_m1(v, i - 1) | (1 << (i - 1));
		}

		i++;
	}
}

void DataFileIO::clear_table(huftable_t* data, int count)
{
	for (int i = 0; i < count; ++i)
	{
		data[i].l1 = 0;
		data[i].l2 = 0xFFFF;
		data[i].l3 = 0;
		data[i].bit_depth = 0;
	}
}

//----- (00055C00) --------------------------------------------------------
bool DataFileIO::sub_55C00_TestSaveFile2(int16_t a1)//236c00
{
	return sub_55750_TestExistingSaveFile(0, a1);
}

//----- (00055750) --------------------------------------------------------
bool DataFileIO::sub_55750_TestExistingSaveFile(uint8_t fileindex, int levelindex)//236750 //load in game
{
	FILE* saveslevfile;
	int32_t readState;
	bool correctReadState;
	uint32_t filesize2;
	FILE* savesmapfile;
	uint32_t filesize;
	bool result = false;
	sprintf(printbuffer, "%s/%s/%s%d.DAT", gameDataPath.c_str(), "SAVE", "SVER", fileindex + 1);
	if (DataFileIO::ReadFileAndDecompress(printbuffer, &readbuffer) == 8 && *(uint32_t*)&readbuffer[4] == levelindex && *(uint32_t*)&readbuffer[0] == 0xf)
	{
		sprintf(printbuffer, "%s/%s/%s%d.DAT", gameDataPath.c_str(), "SAVE", "SLEV", fileindex + 1);
		saveslevfile = DataFileIO::CreateOrOpenFile(printbuffer, 512);
		if (saveslevfile)
		{
			readState = DataFileIO::Read(saveslevfile, readbuffer, 8);
			correctReadState = readState != 8;
			if (readState == 8 && *(uint32_t*)&readbuffer[4] != D41A0_0.dword_0x4)
				correctReadState = true;
			filesize2 = DataFileIO::FileLengthBytes(saveslevfile);
			DataFileIO::Close(saveslevfile);
			if (filesize2 == sizeof(type_shadow_D41A0_BYTESTR_0) && !correctReadState)
			{
				sprintf(printbuffer, "%s/%s/%s%d.DAT", gameDataPath.c_str(), "SAVE", "SMAP", fileindex + 1);
				savesmapfile = DataFileIO::CreateOrOpenFile(printbuffer, 512);
				if (savesmapfile)
				{
					filesize = DataFileIO::FileLengthBytes(savesmapfile);
					DataFileIO::Close(savesmapfile);
					if (filesize == 0x712C2)
						result = true;
				}
			}
		}
	}
	return result;
}

//----- (00090D3F) --------------------------------------------------------
char* DataFileIO::sub_90D3F_unload_file_array(int psindex)//271d3f
{
	char* result; // eax

	while (1)
	{
		result = pstr[psindex].path;
		if (!pstr[psindex].colorPalette_var28)
			break;
		ClearMemoryForPath(pstr[psindex]);
		//pstr[++psindex];
		psindex++;
	}
	return result;
}

//----- (00084250) --------------------------------------------------------
bool DataFileIO::sub_84250_load_file_array(int psindex)//265250
{
	//Pathstruct v1; // ebx
	uint16_t result; // si
	//Pathstruct v3; // ebx
	__int16 v4; // ax
	//char str[50];//char v6; // [esp+0h] [ebp-64h]

	int oldpsindex = psindex;
	//sub_85070(); //fix it
	//v1 = pstr[psindex];//ebx=edi
	result = 0;//esi=0
	if (pstr[psindex].colorPalette_var28)
	{
		do
		{
			ClearMemoryForPath(pstr[psindex++]);
			//v1 = pstr[++psindex];
		} while (pstr[psindex].colorPalette_var28);
	}
	psindex = oldpsindex;
	//v3 = pstr[psindex];
	//oldpsindex = psindex;
	while (pstr[psindex].colorPalette_var28)
	{
		v4 = UnpackAndLoadMemoryFromPath(pstr[psindex]);//27B32d - dalo mu to 1 - set 2bab20

		if (v4 >= 0)
		{
			if (v4)
			{
				psindex++;
				//v3 = pstr[++psindex];
				continue;
			}
			//goto LABEL_10;
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
		//gets((x_DWORD)&v6);
	//LABEL_10:
		psindex++;
	}
	return result;
}
// 9A464: using guessed type x_DWORD gets(x_DWORD);
// EA3D8: using guessed type int *xadatapald0dat2.colorPalette_var28;

//----- (0009A2F5) --------------------------------------------------------
uint8_t* DataFileIO::ClearMemoryForPath(Pathstruct path)//27B2f5
{
	uint8_t* result; // eax
	//2bac30
	result = *path.colorPalette_var28;//fix it 2bac30
	//result = 0;//fix it
	if (result)
	{
		FreeMem_83E80(*path.colorPalette_var28);
		result = *path.colorPalette_var28;
		*result = 0;
	}
	return result;
}

//----- (0009A32D) --------------------------------------------------------
signed int DataFileIO::UnpackAndLoadMemoryFromPath(Pathstruct path)//27B32d
{
	//int v1; // edx
	//int *v2; // eax
	//void* v3; // edx
	//int *v4; // eax
	//void* (*v6)(int); // [esp+0h] [ebp-8h]

	//sub_85070();
	/*if (path.var40_alloc_type & 1 )
	  v6 = sub_83D70_malloc1;
	else
	  v6 = sub_83CD0_malloc2;*/
	ClearMemoryForPath(path);
	if (path.path[0] == 0x2A)//fix
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
		//v2 = *(int *)(path.colorPalette_var28);
		//*v2 = v1;
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
		//v4 = *(int **)path.colorPalette_var28;
		//*v4 = v3;
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

//----- (000AB9E1) --------------------------------------------------------
signed int DataFileIO::sub_AB9E1_get_file_unpack_size(const char* path)//28c9e1
{
	uint8_t v2[10]; // [esp+0h] [ebp-1Ch]
	//unsigned __int8 v3; // [esp+4h] [ebp-18h]
	//unsigned __int8 v4; // [esp+5h] [ebp-17h]
	//unsigned __int8 v5; // [esp+6h] [ebp-16h]
	//unsigned __int8 v6; // [esp+7h] [ebp-15h]
	//char v7; // [esp+8h] [ebp-14h]
	//char v8; // [esp+9h] [ebp-13h]
	//char v9; // [esp+Ah] [ebp-12h]
	//char v10; // [esp+Bh] [ebp-11h]
	//char v11; // [esp+Ch] [ebp-10h]
	int ret_size; // [esp+14h] [ebp-8h]
	FILE* file; // [esp+18h] [ebp-4h]

	//fix it
	//v3 = 0;
	//v4 = 0;
	//v5 = 0;
	//v6 = 0;
	//v9 = 0;
	//v10 = 0;
	//fix it

	ret_size = -1;
	//v7 = 'R';
	//v8 = 'N';
	//v9 = 'C';
	//v10 = 1;
	//v11 = 0;
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
// 988DA: using guessed type x_DWORD filelength(x_DWORD);
// 99682: using guessed type x_DWORD strncmp(x_DWORD, x_DWORD, x_DWORD);

//----- (00098C48) --------------------------------------------------------
signed int DataFileIO::sub_98C48_open_nwrite_close(const char* filename, uint8_t* buffer, uint32_t count)//279c48
{
	int result; // ST14_4
	FILE* file; // [esp+4h] [ebp-8h]

	file = DataFileIO::CreateOrOpenFile(filename, 546);
	if (file == NULL)
		return -1;//mozna null
	result = WriteFile_98CAA(file, buffer, count);
	DataFileIO::Close(file);
	return result;
}

size_t DataFileIO::x_write2(FILE* descriptor, uint8_t* buffer, uint32_t size) {
	return fwrite(buffer, 1, size, descriptor);
};// weak

//----- (00098CAA) --------------------------------------------------------
size_t DataFileIO::WriteFile_98CAA(FILE* filename, uint8_t* buffer, uint32_t num_bytes)//279caa
{
	return x_write2(filename, buffer, num_bytes);
}
