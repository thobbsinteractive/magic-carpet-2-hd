#include "DecompressTMAPS.h"

typedef struct huftable_s {
	uint32 l1; // +0
	uint16 l2; // +4
	uint32 l3; // +6
	uint16 bit_depth; // +A
} huftable_t;

typedef struct vars_s {
	uint16 max_matches;
	uint16 enc_key;
	uint32 pack_block_size;
	uint16 dict_size;
	uint32 method;
	uint32 pus_mode;
	uint32 input_size;
	uint32 file_size;

	// inner
	uint32 bytes_left;
	uint32 packed_size;
	uint32 processed_size;
	uint32 v7;
	uint32 pack_block_pos;
	uint16 pack_token, bit_count, v11;
	uint16 last_min_offset;
	uint32 v17;
	uint32 pack_block_left_size;
	uint16 match_count;
	uint16 match_offset;
	uint32 v20, v21;
	uint32 bit_buffer;

	uint32 unpacked_size;
	uint32 rnc_data_size;
	uint16 unpacked_crc, unpacked_crc_real;
	uint16 packed_crc;
	uint32 leeway;
	uint32 chunks_count;

	uint8* mem1;
	uint8* pack_block_start;
	uint8* pack_block_max;
	uint8* pack_block_end;
	uint16* mem2;
	uint16* mem3;
	uint16* mem4;
	uint16* mem5;

	uint8* decoded;
	uint8* window;

	size_t read_start_offset, write_start_offset;
	uint8* input, * output, * temp;
	size_t input_offset, output_offset, temp_offset;

	uint8 tmp_crc_data[2048];
	huftable_t raw_table[16];
	huftable_t pos_table[16];
	huftable_t len_table[16];
} vars_t;

#define RNC_SIGN 0x524E43 // RNC
#define RNC_HEADER_SIZE 0x12
#define MAX_BUF_SIZE 0x90000

uint8 read_byte(uint8 * buf, size_t * offset)
{
	return buf[(*offset)++];
}

uint16 read_word_be(uint8* buf, size_t* offset)
{
	uint8 b1 = read_byte(buf, offset);
	uint8 b2 = read_byte(buf, offset);

	return (b1 << 8) | b2;
}

uint32 read_dword_be(uint8* buf, size_t* offset)
{
	uint16 w1 = read_word_be(buf, offset);
	uint16 w2 = read_word_be(buf, offset);

	return (w1 << 16) | w2;
}

static const uint16 crc_table[] = {
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

uint16 crc_block(uint8* buf, size_t offset, int size)
{
	uint16 crc = 0;

	while (size--)
	{
		crc ^= read_byte(buf, &offset);
		crc = (crc >> 8) ^ crc_table[crc & 0xFF];
	}

	return crc;
}

void read_buf(uint8* dest, uint8* source, size_t* offset, int size)
{
	memmove(dest, &source[*offset], size);
	*offset += size;
}

uint8 read_source_byte(vars_t* v)
{
	if (v->pack_block_start == &v->mem1[0xFFFD])
	{
		int left_size = v->file_size - v->input_offset;

		int size_to_read;
		if (left_size <= 0xFFFD)
			size_to_read = left_size;
		else
			size_to_read = 0xFFFD;

		v->pack_block_start = v->mem1;

		read_buf(v->pack_block_start, v->input, &v->input_offset, size_to_read);

		if (left_size - size_to_read > 2)
			left_size = 2;

		read_buf(&v->mem1[size_to_read], v->input, &v->input_offset, left_size);
		v->input_offset -= left_size;
	}

	return *v->pack_block_start++;
}

uint32 input_bits_m2(vars_t* v, short count)
{
	uint32 bits = 0;

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



uint32 input_bits_m1(vars_t* v, short count)
{
	uint32 bits = 0;
	uint32 prev_bits = 1;

	while (count--)
	{
		if (!v->bit_count)
		{
			uint8 b1 = read_source_byte(v);
			uint8 b2 = read_source_byte(v);
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

int input_bits(vars_t* v, short count)
{
	if (v->method != 2)
		return input_bits_m1(v, count);
	else
		return input_bits_m2(v, count);
}

void clear_table(huftable_t* data, int count)
{
	for (int i = 0; i < count; ++i)
	{
		data[i].l1 = 0;
		data[i].l2 = 0xFFFF;
		data[i].l3 = 0;
		data[i].bit_depth = 0;
	}
}

uint32 inverse_bits(uint32 value, int count)
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

void proc_20(huftable_t* data, int count)
{
	int val = 0;
	uint32 div = 0x80000000;
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

void make_huftable(vars_t* v, huftable_t* data, int count)
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

uint32 decode_table_data(vars_t* v, huftable_t* data)
{
	uint32 i = 0;

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

void write_buf(uint8* dest, size_t* offset, uint8* source, int size)
{
	memmove(&dest[*offset], source, size);
	*offset += size;
}

void write_decoded_byte(vars_t* v, uint8 b)
{
	if (&v->decoded[0xFFFF] == v->window)
	{
		write_buf(v->output, &v->output_offset, &v->decoded[v->dict_size], 0xFFFF - v->dict_size);
		memmove(v->decoded, &v->window[-v->dict_size], v->dict_size);
		v->window = &v->decoded[v->dict_size];
	}

	*v->window++ = b;
	v->unpacked_crc_real = crc_table[(v->unpacked_crc_real ^ b) & 0xFF] ^ (v->unpacked_crc_real >> 8);
}

void ror_w(uint16* x)
{
	if (*x & 1)
		* x = 0x8000 | (*x >> 1);
	else
		*x >>= 1;
}

int unpack_data_m1(vars_t* v)
{
	while (v->processed_size < v->input_size)
	{
		make_huftable(v, v->raw_table, _countof(v->raw_table));
		make_huftable(v, v->len_table, _countof(v->len_table));
		make_huftable(v, v->pos_table, _countof(v->pos_table));

		int subchunks = input_bits_m1(v, 16);

		while (subchunks--)
		{
			uint32 data_length = decode_table_data(v, v->raw_table);
			v->processed_size += data_length;

			if (data_length)
			{
				while (data_length--)
					write_decoded_byte(v, (v->enc_key ^ read_source_byte(v)) & 0xFF);

				ror_w(&v->enc_key);

				v->bit_buffer = (((v->pack_block_start[2] << 16) | (v->pack_block_start[1] << 8) | v->pack_block_start[0]) << v->bit_count) | (v->bit_buffer & ((1 << v->bit_count) - 1));
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

void decode_match_count(vars_t* v)
{
	v->match_count = input_bits_m2(v, 1) + 4;

	if (input_bits_m2(v, 1))
		v->match_count = ((v->match_count - 1) << 1) + input_bits_m2(v, 1);
}

void decode_match_offset(vars_t* v)
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

int unpack_data_m2(vars_t* v)
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
						uint32 data_length = (input_bits_m2(v, 4) << 2) + 12;
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

int do_unpack_data(vars_t* v)
{
	int start_pos = v->input_offset;

	uint32 sign = read_dword_be(v->input, &v->input_offset);
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

	v->mem1 = (uint8*)malloc(0xFFFF);
	v->decoded = (uint8*)malloc(0xFFFF);
	v->pack_block_start = &v->mem1[0xFFFD];
	v->window = &v->decoded[v->dict_size];

	v->unpacked_crc_real = 0;
	v->bit_count = 0;
	v->bit_buffer = 0;
	v->processed_size = 0;

	uint16 specified_key = v->enc_key;

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

int do_unpack(vars_t* v)
{
	v->packed_size = v->file_size;

	if (v->file_size < RNC_HEADER_SIZE)
		return 6;

	int result = do_unpack_data(v); // data
	if (result == 0)return v->input_size;
	return -result;
}

vars_t* init_vars()
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


int sub_9894C_decompress(Bit8u* a1, Bit8u* a2) {
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
	v->input = (uint8*)malloc(MAX_BUF_SIZE);

	Bit32u signature = a1[0] + (a1[1] << 8) + (a1[2] << 16);
	if (signature == 0x434e52)
	{
		Bit32u inputsize = a1[11] + (a1[10] << 8) + (a1[9] << 16) + (a1[8] << 24);
		memmove(v->input, a1, inputsize + 0x12);
	}
	else
	{
		v->input[0] = 0;
	}
	//v->input = a1;

	v->output = a2;
	v->temp = (uint8*)malloc(MAX_BUF_SIZE);

	int error_code = 0;
	/*switch (v->pus_mode)
	{
	case 0: error_code = do_pack(v); break;
	case 1: error_code = do_unpack(v); break;
	case 2: error_code = do_search(v); break;
	}*/
	error_code = do_unpack(v);

	if (v->pus_mode == 2)
	{
		free(v->input);
		free(v->output);
		free(v->temp);
		free(v->input);
		free(v);

		return error_code;
	}

	/*if (error_code)
	 {
		switch (error_code) {
		case 4: printf("Corrupted input data.\n"); break;
		case 5: printf("CRC check failed.\n"); break;
		case 6:
		case 7:
			printf("Wrong RNC header.\n"); break;
		case 10: printf("No RNC archives were found.\n"); break;
		default: printf("Cannot process file. Error code: %x\n", error_code); break;
		}
	}*/

	free(v->input);
	//free(v->output);
	free(v->temp);
	free(v);
	if (error_code == -6)return 0;
	//if (error_code == 0)return 0;
	return error_code;
}


signed int sub_5C3D0_file_decompress(Bit8u* input, Bit8u* output)//23d3d0
{
	//char v3; // [esp+0h] [ebp-8h]
	//char v4; // [esp+1h] [ebp-7h]
	//char v5; // [esp+2h] [ebp-6h]
	//char v6; // [esp+3h] [ebp-5h]
	//char v7; // [esp+4h] [ebp-4h]

	char RNSSING[] = "RNC\x1";
	//v3 = 82;
	//v4 = 78;
	//v6 = 1;
	//v5 = 67;
	//v7 = 0;
	if (strncmp((const char*)input, RNSSING, 4))
		return 0;
	sub_9894C_decompress(input, output);
	return 1;
}

const int bytesPerPixel = 4; /// red, green, blue
const int fileHeaderSize = 14;
const int infoHeaderSize = 40;

unsigned char* createBitmapFileHeader(int height, int width, int pitch, int paddingSize) {
	int fileSize = fileHeaderSize + infoHeaderSize + (/*bytesPerPixel*width*/pitch + paddingSize) * height;

	static unsigned char fileHeader[] = {
		0,0, /// signature
		0,0,0,0, /// image file size in bytes
		0,0,0,0, /// reserved
		0,0,0,0, /// start of pixel array
	};

	fileHeader[0] = (unsigned char)('B');
	fileHeader[1] = (unsigned char)('M');
	fileHeader[2] = (unsigned char)(fileSize);
	fileHeader[3] = (unsigned char)(fileSize >> 8);
	fileHeader[4] = (unsigned char)(fileSize >> 16);
	fileHeader[5] = (unsigned char)(fileSize >> 24);
	fileHeader[10] = (unsigned char)(fileHeaderSize + infoHeaderSize);

	return fileHeader;
}

unsigned char* createBitmapInfoHeader(int height, int width) {
	static unsigned char infoHeader[] = {
		0,0,0,0, /// header size
		0,0,0,0, /// image width
		0,0,0,0, /// image height
		0,0, /// number of color planes
		0,0, /// bits per pixel
		0,0,0,0, /// compression
		0,0,0,0, /// image size
		0,0,0,0, /// horizontal resolution
		0,0,0,0, /// vertical resolution
		0,0,0,0, /// colors in color table
		0,0,0,0, /// important color count
	};

	infoHeader[0] = (unsigned char)(infoHeaderSize);
	infoHeader[4] = (unsigned char)(width);
	infoHeader[5] = (unsigned char)(width >> 8);
	infoHeader[6] = (unsigned char)(width >> 16);
	infoHeader[7] = (unsigned char)(width >> 24);
	infoHeader[8] = (unsigned char)(height);
	infoHeader[9] = (unsigned char)(height >> 8);
	infoHeader[10] = (unsigned char)(height >> 16);
	infoHeader[11] = (unsigned char)(height >> 24);
	infoHeader[12] = (unsigned char)(1);
	infoHeader[14] = (unsigned char)(bytesPerPixel * 8);

	return infoHeader;
}

inline void setRGBA(png_byte* ptr, Bit8u* val)
{
	ptr[0] = val[0];
	ptr[1] = val[1];
	ptr[2] = val[2];
	ptr[3] = val[3];
}

void writeImagePNG(char* filename, int width, int height, Bit8u* buffer, char* title)
{
		int code = 0;
		FILE* fp = NULL;
		png_structp png_ptr = NULL;
		png_infop info_ptr = NULL;
		png_bytep row = NULL;

		// Open file for writing (binary mode)
		fopen_s(&fp,filename, "wb");
		if (fp == NULL) {
			fprintf(stderr, "Could not open file %s for writing\n", filename);
			code = 1;
			goto finalise;
		}

		// Initialize write structure
		png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (png_ptr == NULL) {
			fprintf(stderr, "Could not allocate write struct\n");
			code = 1;
			goto finalise;
		}

		// Initialize info structure
		info_ptr = png_create_info_struct(png_ptr);
		if (info_ptr == NULL) {
			fprintf(stderr, "Could not allocate info struct\n");
			code = 1;
			goto finalise;
		}

		// Setup Exception handling
		if (setjmp(png_jmpbuf(png_ptr))) {
			fprintf(stderr, "Error during png creation\n");
			code = 1;
			goto finalise;
		}

		png_init_io(png_ptr, fp);

		// Write header (8 bit colour depth)
		png_set_IHDR(png_ptr, info_ptr, width, height,
			8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

		// Set title
		if (title != NULL) {
			png_text title_text;
			title_text.compression = PNG_TEXT_COMPRESSION_NONE;
			title_text.key = (png_charp)"Title";
			title_text.text = title;
			png_set_text(png_ptr, info_ptr, &title_text, 1);
		}

		png_write_info(png_ptr, info_ptr);

		// Allocate memory for one row (3 bytes per pixel - RGB)
		row = (png_bytep)malloc(4 * width * sizeof(png_byte));

		// Write image data
		int x, y;
		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				setRGBA(&(row[x * 4]), buffer + (y * width + x) * 4);
			}
			png_write_row(png_ptr, row);
		}

		// End write
		png_write_end(png_ptr, NULL);

	finalise:
		if (fp != NULL) fclose(fp);
		if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
		if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		if (row != NULL) free(row);

		//return code;
		printf("img: %s created\n", filename);
}

Bit8u temobuffer[100000 * 4];
void removeAlpha(Bit8u* buffer, int width, int height) {
	for(int i=0;i<4* width * height;i++)
		temobuffer[i] = buffer[i];
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
		{
			if ((i > 0)&& (buffer[(i * width + j) * 4 + 3]==0)&&(temobuffer[((i-1) * width + (j - 1)) * 4 + 3] !=0))
			{
				buffer[(i * width + j) * 4 + 0] = temobuffer[((i-1) * width + j) * 4 + 0];
				buffer[(i * width + j) * 4 + 1] = temobuffer[((i-1) * width + j) * 4 + 1];
				buffer[(i * width + j) * 4 + 2] = temobuffer[((i-1) * width + j) * 4 + 2];
				buffer[(i * width + j) * 4 + 3] = 255;
			}
		}
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
		{
			if ((i < height - 1) && (buffer[(i * width + j) * 4 + 3] == 0) && (temobuffer[((i-1) * width + j) * 4 + 3] != 0))
			{
				buffer[(i * width + j) * 4 + 0] = temobuffer[((i+1) * width + j) * 4 + 0];
				buffer[(i * width + j) * 4 + 1] = temobuffer[((i+1) * width + j) * 4 + 1];
				buffer[(i * width + j) * 4 + 2] = temobuffer[((i+1) * width + j) * 4 + 2];
				buffer[(i * width + j) * 4 + 3] = 255;
			}
		}
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
		{
			if ((j < width -1) && (buffer[(i * width + j) * 4 + 3] == 0) && (temobuffer[(i * width + (j+1)) * 4 + 3] != 0))
			{
				buffer[(i * width + j) * 4 + 0] = temobuffer[(i * width + (j+1)) * 4 + 0];
				buffer[(i * width + j) * 4 + 1] = temobuffer[(i * width + (j+1)) * 4 + 1];
				buffer[(i * width + j) * 4 + 2] = temobuffer[(i * width + (j+1)) * 4 + 2];
				buffer[(i * width + j) * 4 + 3] = 255;
			}
		}
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
		{
			if ((j > 0) && (buffer[(i * width + j) * 4 + 3] == 0) && (temobuffer[(i * width + (j-1)) * 4 + 3] != 0))
			{
				buffer[(i * width + j) * 4 + 0] = temobuffer[(i * width + (j-1)) * 4 + 0];
				buffer[(i * width + j) * 4 + 1] = temobuffer[(i * width + (j-1)) * 4 + 1];
				buffer[(i * width + j) * 4 + 2] = temobuffer[(i * width + (j-1)) * 4 + 2];
				buffer[(i * width + j) * 4 + 3] = 255;
			}
		}
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
		{
			buffer[(i * width + j) * 4 + 3] = 255;
		}
};

void writeUsedColor(char* filename, std::vector<uint8_t> usedColors) {
	FILE* colfile;
	fopen_s(&colfile, filename, "wt");
	int oldColor = -1;
	int j = 0;
	for (int i = 0;i< usedColors.size();i++)
	{
		if (oldColor != usedColors[i])
		{
			oldColor = usedColors[i];
			fprintf(colfile, "0x%02X,", usedColors[i]);
			if (j % 16 == 15)
				fprintf(colfile, "\n");
			j++;
		}
	}
	fclose(colfile);
};

#pragma pack (1)
typedef struct {//size 26
	Bit32s dword_0;
	Bit32s dword_4;
	Bit8u* dword_8_data;
	Bit32u* dword_12x;//Bit32u*
	Bit32u* dword_16x;//Bit32u*
	Bit16s word_20;
	Bit16s word_22;
	Bit16s word_24;
	Bit8u data[];
	//Bit32u dword_26;
} type_x_DWORD_E9C28_str;

typedef struct {//lenght 28
	Bit32s dword_0;
	Bit32s dword_4;
	Bit32s dword_8;
	Bit16s word_12;
	Bit16u word_14;
	Bit16s word_16;
	Bit16s word_18;
	Bit16s word_20;
	Bit16s word_22;
	Bit16s word_24;
	Bit16s word_26;
}
type_animations1;

typedef struct {
	Bit16s word_0;
	type_animations1* dword_2;
}
type_E9C08;
#pragma pack (16)

__int16 x_WORD_E130C = 0; // weak
Bit8u* TMAPS00TAB_BEGIN_BUFFER;// ?? must set
char x_BYTE_E2A20 = 0; // weak
int x_DWORD_E1304 = 0; // weak
int x_DWORD_E1308 = 0; // weak
type_E9C08* x_DWORD_E9C08x;  // weak ?? must set
int x_DWORD_F66F0[504]; // idb//2c76f0 ?? must set
__int16 x_WORD_E130E = 0; // weak
__int16 x_WORD_E1310 = 0; // weak
__int16 x_WORD_E1312ar[2] = { 0,0 }; // weak
__int16 x_WORD_E1316 = 0; // weak
char x_BYTE_E29E8 = 1; // weak
Bit8u x_DWORD_17ECA0[4608]; // weak
type_x_DWORD_E9C28_str* x_DWORD_E9C28_str;

type_animations1* sub_724F0(type_E9C08* a1x, __int16 a2)
{
	//type_animations1* resultx; // eax
	int resulty;
	//int v3; // edx
	Bit16s v3x;
	//HIWORD(v3) = HIWORD(a1);
	//resultx = a1x->dword_2;
	resulty = 0;
	//LOWORD(v3) = *(x_WORD *)a1;
	v3x = a1x->word_0;
	if (!a1x->word_0)
		return 0;
	while (!a1x->dword_2[resulty].dword_4 || a2 != a1x->dword_2[resulty].word_26)
	{
		--v3x;
		//result += 28;
		//resultx++;
		resulty++;
		if (!v3x)
			return 0;
	}
	return &(a1x->dword_2[resulty]);
}

void sub_765FC(__int16 a1, __int16 a2)
{
	x_WORD_E130E = a1;
	x_WORD_E1310 = a2;
}

void qmemcpy(void* a, void* b, size_t c) {
	memcpy(a, b, c);
};

int sub_7677C()
{
	int result; // eax

	result = (unsigned __int16)x_WORD_E1312ar[0] - 6;
	x_DWORD_E1308 += result;
	return result;
}

__int16 sub_7678D()//animate sprite
{
	x_WORD* v0; // edi
	x_WORD* v1; // esi
	__int16 result; // ax
	x_BYTE* v3; // edi
	x_BYTE* v4; // esi
	char v5; // dl
	x_WORD* v6; // [esp-4h] [ebp-14h]
	x_WORD* v7; // [esp+0h] [ebp-10h]
	__int16 i; // [esp+6h] [ebp-Ah]
	__int16 v9; // [esp+8h] [ebp-8h]
	int v10; // [esp+Ch] [ebp-4h]

	v10 = (unsigned __int16)x_WORD_E130E;
	v0 = (x_WORD*)x_DWORD_E1304;
	v1 = (x_WORD*)(x_DWORD_E1308 + 2);
	v9 = *(x_WORD*)x_DWORD_E1308;
	do
	{
		while (1)
		{
			result = *v1;
			++v1;
			v7 = v0;
			if (result >= 0)
				goto LABEL_10;
			if (!(result & 0x4000))
				break;
			v0 = (x_WORD*)((char*)v0 + v10 * -result);
		}
		v6 = v0;
		v3 = (char*)v0 + v10 - 1;
		if (x_BYTE_E2A20 == 1)
		{
			if ((x_BYTE)result)
				* v3 = result;
		}
		else
		{
			*v3 = result;
		}
		v0 = v6;
	LABEL_10:
		for (i = result; i; i--)
		{
			while (1)
			{
				LOBYTE(result) = *(x_BYTE*)v1;
				v4 = (char*)v1 + 1;
				result = (unsigned __int8)result;
				v0 = (x_WORD*)((char*)v0 + (unsigned __int8)result);
				v5 = *v4;
				v1 = (x_WORD*)(v4 + 1);
				if (v5 > 0)
					break;
				result = *v1;
				++v1;
				do
				{
					*v0 = result;
					++v0;
					++v5;
				} while (v5);
				if (!--i)
					goto LABEL_18;
			}
			do
			{
				*v0 = *v1;
				++v1;
				++v0;
				--v5;
			} while (v5);
		}
	LABEL_18:
		v0 = (x_WORD*)((char*)v7 + v10);
		--v9;
	} while (v9);
	x_DWORD_E1308 = (int)v1;
	return result;
}

int sub_76840()
{
	x_BYTE* v0; // edi
	char* v1; // esi
	__int16 v2; // bx
	__int16 v3; // cx
	unsigned __int8 v4; // dl
	__int16 v5; // ax
	char v6; // dh
	char v7; // dh
	int result; // eax
	int v9; // [esp-8h] [ebp-Ch]
	int v10; // [esp-4h] [ebp-8h]
	__int16 v11; // [esp+2h] [ebp-2h]

	v11 = x_WORD_E130E;
	v0 = (x_BYTE*)x_DWORD_E1304;
	v1 = (char*)x_DWORD_E1308;
	v10 = (unsigned __int16)x_WORD_E1312ar[0] - 6;
	v9 = x_DWORD_E1308;
	v2 = x_WORD_E1310;
	do
	{
		++v1;
		v3 = v11;
		do
		{
			v4 = *v1++;
			if ((char)v4 > 0)
			{
				v3 -= v4;
			}
			else
			{
				HIBYTE(v5) = -1;
				LOBYTE(v5) = v4;
				v3 += v5;
			}
			if ((char)v4 > 0)
			{
				v7 = *v1++;
				do
				{
					if (x_BYTE_E2A20 == 1)
					{
						if (v7)
							* v0 = v7;
					}
					else
					{
						*v0 = v7;
					}
					++v0;
					--v4;
				} while (v4);
			}
			else
			{
				do
				{
					v6 = *v1++;
					if (x_BYTE_E2A20 == 1)
					{
						if (v6)
							* v0 = v6;
					}
					else
					{
						*v0 = v6;
					}
					++v0;
					++v4;
				} while (v4);
			}
		} while (v3);
		--v2;
	} while (v2);
	result = v10;
	x_DWORD_E1308 = v10 + v9;
	return result;
}

void sub_766A4()//animate sprite
{
	__int16 result; // ax

	qmemcpy(&x_WORD_E1316, (void*)x_DWORD_E1308, sizeof(x_WORD_E1316));
	x_DWORD_E1308 += 2;
	x_DWORD_E1308 += 8;
	while (x_WORD_E1316)
	{
		--x_WORD_E1316;
		qmemcpy(x_WORD_E1312ar, (void*)x_DWORD_E1308, 4u);
		x_DWORD_E1308 += 4;
		qmemcpy(&x_WORD_E130C, (void*)x_DWORD_E1308, sizeof(x_WORD_E130C));
		x_DWORD_E1308 += 2;
		switch (x_WORD_E130C)
		{
		case 7:
			result = sub_7678D();
			break;
		case 4:
			result = sub_7677C();
			break;
		case 15:
			result = sub_76840();
			break;
		default:
			result = sub_7677C();
			break;
		}
	}
	//return result;
}

__int16 sub_76752()
{
	__int16 result; // ax

	x_WORD_E130E = *(x_WORD*)(x_DWORD_E1308 + 2);
	result = *(x_WORD*)(x_DWORD_E1308 + 4);
	x_WORD_E1310 = *(x_WORD*)(x_DWORD_E1308 + 4);
	x_DWORD_E1308 += 6;
	return result;
}

int sub_76619(int a1, int a2)//animate sprite
{
	x_DWORD_E1308 = a1;
	x_DWORD_E1304 = a2;
	while (1)
	{
		qmemcpy(x_WORD_E1312ar, (void*)x_DWORD_E1308, 4u);
		x_DWORD_E1308 += 4;
		qmemcpy(&x_WORD_E130C, (void*)x_DWORD_E1308, sizeof(x_WORD_E130C));
		x_DWORD_E1308 += 2;
		if (x_WORD_E130C != -20718)
			break;
		sub_76752();
	}
	if (x_WORD_E130C != -3590)
		return 0;
	sub_766A4();
	return x_DWORD_E1308;
}

int sub_72350(type_animations1* a1x)//253350 //animates sprite
{
	int v1; // esi
	int v2; // edi
	int v3; // eax
	__int16 v4; // cx
	int result; // eax

	if (a1x->word_22 > a1x->word_16)
	{
		a1x->word_22 = 1;
		a1x->dword_8 = a1x->word_14;
	}
	v1 = *(x_DWORD*)a1x->dword_4;
	v2 = a1x->dword_8;
	sub_765FC(a1x->word_18, 0);
	v1 += 6;
	v3 = sub_76619(v1 + v2, v1);
	v4 = a1x->word_22;
	result = v3 - v1;
	a1x->dword_8 = result;
	a1x->word_22 = v4 + 1;
	return result;
}



int sub_715B0()//2525b0
{
	unsigned __int16 v0; // bx
	int result; // eax
	int* v2; // ecx
	char v3; // cl
	char v4; // cl
	type_animations1* v5x; // eax
	__int16 v6; // si
	unsigned __int16 i; // bx
	int* v8; // edi
	char v9; // cl
	type_animations1* v10x; // eax

	v0 = 0;
	do
	{
		result = 4 * v0;
		v2 = *(int**)((char*)x_DWORD_F66F0 + result);
		if (v2)
		{
			result = *v2;
			v3 = *(x_BYTE*)* v2;
			if (v3 & 8)
			{
				if (v3 & 0x20)
				{
					result = 5 * v0;
					v6 = *(x_WORD*)(TMAPS00TAB_BEGIN_BUFFER + 10 * v0 + 8);
					for (i = *(x_WORD*)(TMAPS00TAB_BEGIN_BUFFER + 10 * v0 + 8); i < 0x1F8u; i++)
					{
						result = 5 * i;
						if (v6 != *(x_WORD*)(TMAPS00TAB_BEGIN_BUFFER + 10 * i + 8))
							break;
						result = 4 * i;
						v8 = (int*)x_DWORD_F66F0[i];
						if (v8)
						{
							result = *v8;
							v9 = *(x_BYTE*)* v8 & 0xF7;
							*(x_BYTE*)* v8 = v9;
							if (v9 & 1)
							{
								v10x = sub_724F0(x_DWORD_E9C08x, i);
								result = sub_72350(v10x);
							}
						}
					}
					v0 = i - 1;
				}
				else
				{
					v4 = v3 & 0xF7;
					*(x_BYTE*)result = v4;
					if (v4 & 1)
					{
						v5x = sub_724F0(x_DWORD_E9C08x, v0);
						result = sub_72350(v5x);
					}
				}
			}
		}
		v0++;
	} while (v0 < 0x1F8u);
	return result;
}

void sub_83CC0(char a1)//264cc0
{
	//char result; // al

	//result = a1;
	x_BYTE_E29E8 = a1;
	//  return result;
}
void* sub_83CD0_malloc2(size_t a1)//264cd0
{
	return malloc(a1);
}
int sub_84000(int a1)//265000
{
	int result; // eax
	int v2; // edx
	int v3; // ebx

	result = a1;
	if (*(x_DWORD*)(a1 + 12))
	{
		*(x_BYTE*)(a1 + 16) = 0;
		v2 = *(x_DWORD*)(a1 + 12);
		if (*(x_BYTE*)(a1 + 17) == *(x_BYTE*)(v2 + 17) && !*(x_BYTE*)(v2 + 16))
		{
			v3 = *(x_DWORD*)(a1 + 8);
			if (v3)
				* (x_DWORD*)(v3 + 12) = v2;
			*(x_DWORD*)(*(x_DWORD*)(a1 + 12) + 8) = *(x_DWORD*)(a1 + 8);
			*(x_DWORD*)(*(x_DWORD*)(a1 + 12) + 4) += *(x_DWORD*)(a1 + 4);
			*(x_DWORD*)(a1 + 4) = 0;
		}
	}
	return result;
}
void sub_83E80_freemem4(Bit8u* a1)//264e80
{
	Bit8u* v1; // eax
	char v2; // bl
	int* i; // ebx

	if (a1)
	{
		v1 = (Bit8u*)& x_DWORD_17ECA0;
		v2 = 0;
		while (v1)
		{
			if ((int)a1 == v1[0])//fix
			{
				v2 = 1;
				*((x_BYTE*)v1 + 16) = 0;
				break;
			}
			v1 = (Bit8u*)(int*)v1[2];
		}
		if (v2 == 1)
		{
			for (i = (int*)& x_DWORD_17ECA0; i; i = (int*)i[2])
			{
				if (!*((x_BYTE*)i + 16))
					sub_84000((int)i);
			}
		}
		//sub_85350(); //fix
	}
}
type_E9C08* sub_72120(unsigned __int16 a1)//253120
{
	unsigned __int16 v1; // di
	type_E9C08* v2x; // esi
	type_E9C08* v3x; // ebx
	type_animations1* v4; // eax
	//int v5; // eax

	v1 = a1;
	sub_83CC0(9);
	v2x = (type_E9C08*)sub_83CD0_malloc2(6);
	v3x = v2x;
	if (v2x && (v4 = (type_animations1*)sub_83CD0_malloc2(28 * a1), (v2x->dword_2 = v4) != 0))
	{
		v2x->word_0 = a1;
		while (--v1 != 0xffff)
		{
			//v5 = 7 * v1;
			//*(x_DWORD *)(v2x->dword_2 + 4 * v5 + 4) = 0;
			//*(x_DWORD *)(v2x->dword_2 + 4 * v5) = 0;
			v2x->dword_2[v1].dword_4 = 0;
			v2x->dword_2[v1].dword_0 = 0;
		}
	}
	else if (v2x)
	{
		sub_83E80_freemem4((Bit8u*)v2x);
		v3x = 0;
	}
	sub_83CC0(10);
	return v3x;
}




signed int sub_71CD0(type_x_DWORD_E9C28_str* a1y)//252cd0
{
	int i; // edx

	for (i = 0; (signed __int16)i < (signed int)a1y->word_22; i++)
	{
		if (!*(x_DWORD*)(14 * (signed __int16)i + a1y->dword_8_data + 4))
			return i;
	}
	return -1;
}

Bit8u* sub_71E70(type_x_DWORD_E9C28_str* a1y, unsigned int a2, __int16 a3)//252e70
{
	signed __int16 v3; // si
	signed __int16 v4; // ax
	signed __int16 v5; // dx
	int v6; // ecx
	int v7; // eax
	Bit8u* result; // eax

	v3 = -1;
	if (a2 < a1y->dword_4)
	{
		v4 = sub_71CD0(a1y);
		v5 = v4;
		v6 = v4;
		v3 = v4;
		if (v4 > -1)
		{
			v7 = 14 * v4;
			*(x_WORD*)(a1y->dword_8_data + v7 + 10) = v5;
			*(x_DWORD*)(a1y->dword_8_data + v7 + 4) = a2;
			*(x_DWORD*)(a1y->dword_8_data + v7) = a1y->dword_0 + (int)a1y->dword_16x - a1y->dword_4;
			*(x_WORD*)(a1y->dword_8_data + v7 + 12) = a3;
			a1y->dword_4 -= a2;
			*(x_WORD*)(a1y->dword_8_data + v7 + 8) = a1y->word_20;
			*(x_DWORD*)(a1y->dword_12x + (unsigned __int16)(a1y->word_20)++) = (Bit32u)a1y->dword_8_data + 14 * v6;
		}
	}
	if (v3 <= -1)
		result = 0;
	else
		result = 14 * v3 + a1y->dword_8_data;
	return result;
}


type_x_DWORD_E9C28_str* sub_71B40(int a1, unsigned __int16 a2, type_x_DWORD_E9C28_str* a3y)//252b40
{
	unsigned __int16 v3; // di
	int v4; // eax
	int v5; // eax
	int v6; // edx
	type_x_DWORD_E9C28_str* v7y; // esi
	//int v8; // eax
	type_x_DWORD_E9C28_str* v10y; // ebx
	Bit8u* v11x; // eax
	Bit8u* v12x; // eax
	Bit8u* v13x; // eax
	int v14; // [esp+0h] [ebp-Ch]

	v3 = a2;
	if (a3y)
	{
		v4 = 14 * a2;
		v14 = v4;
		v5 = 4 * a2 + v4 + 26;
		v6 = a1 - v5;
		if (a1 == v5)
			return 0;
		a3y->word_20 = 0;
		a3y->word_24 = 2;
		a3y->dword_0 = v6;
		a3y->dword_4 = v6;
		v7y = a3y;
		a3y->word_22 = a2;
		a3y->dword_8_data = a3y->data;
		//v8 = v14 + a3x + 26;
		a3y->dword_12x = (Bit32u*)& a3y->data[v14];// (Bit32u)(v14 + (Bit8u*)a3y + 26);//must fix for 64 bit version - data
		a3y->dword_16x = (Bit32u*)& a3y->data[v14 + 4 * a2];//(Bit32u)(4 * a2 + (v14 + (Bit8u*)a3y + 26));//must fix for 64 bit version - data
		while (--v3 != 0xffff)
			* (x_DWORD*)(a3y->dword_8_data + 14 * v3 + 4) = 0;
	}
	else
	{
		v10y = (type_x_DWORD_E9C28_str*)sub_83CD0_malloc2(26);
		v7y = v10y;
		if (!v10y
			|| (v11x = (Bit8u*)sub_83CD0_malloc2(a1), (v10y->dword_16x = (Bit32u*)v11x) == 0)
			|| (v12x = (Bit8u*)sub_83CD0_malloc2(14 * a2), (v10y->dword_8_data = (Bit8u*)v12x) == 0)
			|| (v13x = (Bit8u*)sub_83CD0_malloc2(4 * a2), (v10y->dword_12x = (Bit32u*)v13x) == 0))
		{
			if (v10y)
			{
				if (v10y->dword_16x)
				{
					if (v10y->dword_8_data)
						sub_83E80_freemem4((Bit8u*)v10y->dword_8_data);
					sub_83E80_freemem4((Bit8u*)v10y->dword_16x);
				}
				sub_83E80_freemem4((Bit8u*)v10y);
			}
			exit(1);
		}
		v10y->word_20 = 0;
		v10y->word_24 = 1;
		v10y->word_22 = a2;
		v10y->dword_0 = a1;
		v10y->dword_4 = a1;
		while (--v3 != 0xffff)
			* (x_DWORD*)(v10y->dword_8_data + 14 * v3 + 4) = 0;
	}
	return v7y;
}

type_animations1* sub_721C0_initTmap(type_E9C08* a1x, int* a2, __int16 a3)//2531c0
{
	signed __int16 v3; // cx
	signed __int16 v4; // si
	signed __int16 i; // bx
	//x_DWORD *v6; // edx
	type_animations1* v6x;
	int v7; // ebx
	int v8; // ecx
	__int16 v9; // ST08_2
	//int v10; // edx
	signed __int16 v12; // [esp+Ch] [ebp-4h]

	v3 = -1;
	v4 = -1;
	if (!(a1x->word_0))
		return 0;
	for (i = 0; i < a1x->word_0; i++)
	{
		v6x = &a1x->dword_2[i];
		if (v6x->dword_4)
		{
			if (!v6x->dword_0)
				v4 = i;
		}
		else
		{
			v3 = i;
		}
	}
	v12 = v3 <= 0 ? v4 : v3;
	if (v12 <= -1)
		return 0;
	v7 = *a2;
	v8 = *(unsigned __int16*)(*a2 + 4) * *(unsigned __int16*)(*a2 + 2);
	v9 = *(x_WORD*)(v8 + *a2 + 6);
	//v10 = 28 * v12;
	a1x->dword_2[v12].dword_4 = (x_DWORD)a2;
	a1x->dword_2[v12].word_12 = 6;
	a1x->dword_2[v12].word_14 = v8 + 6;
	a1x->dword_2[v12].word_16 = v9;
	a1x->dword_2[v12].word_18 = *(x_WORD*)(v7 + 2);
	a1x->dword_2[v12].word_20 = *(x_WORD*)(v7 + 4);
	a1x->dword_2[v12].dword_8 = v8 + 6;
	a1x->dword_2[v12].word_22 = 1;
	a1x->dword_2[v12].dword_0 = 1;
	a1x->dword_2[v12].word_24 = v12;
	a1x->dword_2[v12].word_26 = a3;
	//return v10 + a1x->dword_2;
	return &a1x->dword_2[v12];
}

bool isOther(int* other_folder,int index) {
	if (other_folder != nullptr)
	{
		for (int i = 0; other_folder[i] != -1; i++)
			if (index == other_folder[i])
				return true;
	}
	return false;
}

int main(int argc, char** argv) {

	int max_images = 504;
	std::string palletPath;
	std::string tmapsDat;
	std::string tmapsTab;
	std::string outputPath = fs::current_path().u8string() + "/out";
	int padding = 0;
	std::string folderPath;
	std::string format;
	ImageType imageType = ImageType::png;
	bool showHelp = false;
	bool caveSprites = false;

	std::vector<std::string> params;
	params.clear();
	for (int i = 1; i < argc; ++i) {
		params.emplace_back(argv[i]);
	}

	for (auto p = params.cbegin(); p != params.cend(); ++p) {
		const auto param = *p;
		if ((param == "-p") || (param == "--pallet")) 
		{
			palletPath = *(++p);
			if (!fs::exists(palletPath))
			{
				palletPath = fs::current_path().u8string() + "/" + palletPath;
				if (!fs::exists(palletPath))
					showHelp = true;
			}
		}
		else if ((param == "-t") || (param == "--tmaps-dat")) 
		{
			tmapsDat = *(++p);

			if (!fs::exists(tmapsDat))
			{
				tmapsDat = fs::current_path().u8string() + "/" + tmapsDat;
				if (!fs::exists(tmapsDat))
					showHelp = true;
			}

			tmapsTab = fs::path(tmapsDat).replace_extension("tab").u8string();

			folderPath = fs::path(tmapsTab).filename().replace_extension("").u8string() + "-";

		}
		else if ((param == "-i") || (param == "--image-type"))
		{
			format = *(++p);

			if (strcmp(format.c_str(), "data") == 0)
			{
				imageType = ImageType::data;
			}
			if (strcmp(format.c_str(), "rnc") == 0)
			{
				imageType = ImageType::rnc;
			}
			if (strcmp(format.c_str(), "bmp") == 0)
			{
				imageType = ImageType::bmp;
			}
			if (strcmp(format.c_str(), "pnga") == 0)
			{
				imageType = ImageType::pnga;
			}
		}
		else if ((param == "-f") || (param == "--folder-pattern"))
		{
			std::string folderPattern = *(++p);

			if (strcmp(folderPattern.c_str(), "0") == 0)
			{
				other_folder = other_folder0;
			}
			if (strcmp(folderPattern.c_str(), "1") == 0)
			{
				other_folder = other_folder1;
			}
			if (strcmp(folderPattern.c_str(), "2") == 0)
			{
				other_folder = other_folder2;
				caveSprites = true;
				max_images = 464;
			}
		}
		else if (param == "--padding")
		{
			padding = std::stoi(*(++p));
		}
		else if ((param == "-o") || (param == "--output-path"))
		{
			outputPath = *(++p);
		}
		else if ((param == "-h") || (param == "--help")) {
			showHelp = true;
		}
	}

	if (palletPath.length() == 0 || tmapsDat.length() == 0 || tmapsTab.length() == 0)
	{
		printf("Missing required parameters!\n");
		showHelp = true;
	}

	if (!fs::exists(palletPath))
	{
		printf("Pallet file not found!\n");
		showHelp;
	}

	if (!fs::exists(tmapsDat))
	{
		printf("TMaps DAT file not found!\n");
		showHelp;
	}

	if (!fs::exists(tmapsTab))
	{
		printf("TMaps TAB file not found!\n");
		showHelp;
	}

	if (!fs::is_directory(outputPath) || !fs::exists(outputPath)) { // Check if outputPath folder exists
		fs::create_directory(outputPath); // create src folder
	}

	if (showHelp)
	{
		printf("-p --pallet: (Required) Pallet file path\n");
		printf("-t --tmaps-dat: (Required) Tmap .DAT file path. Needs a .TAB file of the same name\n");
		printf("-d --data: (Optional) Extracts a *.Data file path. Needs the .DAT and .TAB file to get dimensions\n");
		printf("-i --image-type: (Default png) File output format to use rnc, data, bmp, png or pnga\n");
		printf("-f --folder-pattern: (Optional) Required for full error free extraction of MC2 Data\n");
		printf("--padding: (Optional) For PNG you can specify how many pixels of padding around the image you want\n");
		printf("-o --output-path: (Default) ./out\n");
		printf("For night levels:\n");
		printf("-p PALN-0.DAT -t TMAPS1-0.DAT -f 1 -o out-night\n");
		printf("For day levels:\n");
		printf("-p PALD-0.DAT -t TMAPS0-0.DAT -f 0 -o out-day\n");
		printf("For cave levels:\n");
		printf("-p PALC-0.DAT -t TMAPS2-0.DAT -f 2 -o out-cave\n");
		//printf("-p in/PALN-0.DAT -t in/MSPRN0-0.DAT -f 2 -o out-MSPRN\n");
		return -1;
	}

	try
	{
		//return sub_main(palletPath.c_str(), tmapsDat.c_str(), tmapsTab.c_str(), folderPath.c_str(), max_images, imageType, padding, caveSprites, outputPath.c_str());
		return sub_mainNoRNC(palletPath.c_str(), tmapsDat.c_str(), tmapsTab.c_str(), folderPath.c_str(), max_images, imageType, padding, caveSprites, outputPath.c_str());
	}
	catch (std::exception& e)
	{
		printf(e.what());
		return -100;
	}
}

uint8_t pdwScreenBuffer_351628[100000];
int16_t x_WORD_180660_VGA_type_resolution = 8;
int x_DWORD_18063C_sprite_sizex = 0;
uint32_t screenHeight_180624=320;
uint32_t screenWidth_18062C=200;
int x_DWORD_180644_map_resolution2_y=200;
int x_DWORD_180648_map_resolution2_x=320;
int x_DWORD_180630_screen_height=200; // weak
int x_DWORD_180634_screen_width=320; // weak
__int16 x_WORD_E36D4 = 64;
int x_DWORD_180650_positiony=0;
int x_DWORD_E3890 = 0;

typedef          __int64 ll;
typedef unsigned __int64 ull;
typedef ull             uint64;
typedef ll              int64;
template<class T> int8 __SETS__(T x)
{
	if (sizeof(T) == 1)
		return int8(x) < 0;
	if (sizeof(T) == 2)
		return int16(x) < 0;
	if (sizeof(T) == 4)
		return int32(x) < 0;
	return int64(x) < 0;
}
template<class T, class U> int8 __OFADD__(T x, U y)
{
	if (sizeof(T) < sizeof(U))
	{
		U x2 = x;
		int8 sx = __SETS__(x2);
		return ((1 ^ sx) ^ __SETS__(y)) & (sx ^ __SETS__(x2 + y));
	}
	else
	{
		T y2 = y;
		int8 sx = __SETS__(x);
		return ((1 ^ sx) ^ __SETS__(y2)) & (sx ^ __SETS__(x + y2));
	}
}
template<class T, class U> int8 __OFSUB__(T x, U y)
{
	if (sizeof(T) < sizeof(U))
	{
		U x2 = x;
		int8 sx = __SETS__(x2);
		return (sx ^ __SETS__(y)) & (sx ^ __SETS__(x2 - y));
	}
	else
	{
		T y2 = y;
		int8 sx = __SETS__(x);
		return (sx ^ __SETS__(y2)) & (sx ^ __SETS__(x - y2));
	}
}
template<class T>  int16 __PAIR__(int8  high, T low) { return (((int16)high) << sizeof(high) * 8) | uint8(low); }
template<class T>  int32 __PAIR__(int16 high, T low) { return (((int32)high) << sizeof(high) * 8) | uint16(low); }
template<class T>  int64 __PAIR__(int32 high, T low) { return (((int64)high) << sizeof(high) * 8) | uint32(low); }
template<class T> uint16 __PAIR__(uint8  high, T low) { return (((uint16)high) << sizeof(high) * 8) | uint8(low); }
template<class T> uint32 __PAIR__(uint16 high, T low) { return (((uint32)high) << sizeof(high) * 8) | uint16(low); }
template<class T> uint64 __PAIR__(uint32 high, T low) { return (((uint64)high) << sizeof(high) * 8) | uint32(low); }

uint8_t x_BYTE_F6EE0_tablesx[83456];

void GameBitmapDrawBitmap(uint8_t* ptrBitmapData, uint8_t* ptrScreenBuffer, uint32_t stride, int16_t posX, int16_t posY, uint8_t posHeight, uint8_t scale)
{
	ptrScreenBuffer = (stride * posY + posX + ptrScreenBuffer);
	int8_t width = 0;
	int8_t posWidth = 0;
	int8_t startOffsetX = -1;
	uint8_t pixel = 0;
	uint8_t* ptrScreenBufferLineStart = ptrScreenBuffer;
	int lineStartBytes = 0;
	int countBytes = 0;
	int scaledLinesDrawn = 0;

	do
	{
		while (1)
		{
			while (1)
			{
				startOffsetX = *ptrBitmapData++;
				countBytes++;

				//Is width byte
				if (startOffsetX)
					break;

				//Move row
				if (scaledLinesDrawn < scale - 1)
				{
					int lineLengthBytes = countBytes - lineStartBytes;
					ptrBitmapData -= lineLengthBytes;
					countBytes -= lineLengthBytes;
					scaledLinesDrawn++;
				}
				else
				{
					posHeight--;
					scaledLinesDrawn = 0;
					lineStartBytes = countBytes;
				}

				ptrScreenBufferLineStart += stride;
				ptrScreenBuffer = ptrScreenBufferLineStart;
				if (!posHeight)
					return;
			}

			//Is width byte
			if ((startOffsetX & 0x80u) == 0)
			{
				//Start Drawing
				break;
			}
			//Is a change of start coordinate
			int offset = (char)startOffsetX;
			ptrScreenBuffer -= offset * scale;
			if (!posHeight)
				return;
		}

		posWidth = startOffsetX;
		width = startOffsetX;

		//Draw line
		if (scale > 1)
		{
			do
			{
				pixel = *ptrBitmapData++;
				countBytes++;

				for (int s = 0; s < scale; s++)
				{
					*ptrScreenBuffer++ = pixel;
				}
				posWidth--;
			} while (posWidth);
		}
		else
		{
			qmemcpy(ptrScreenBuffer, ptrBitmapData, width);
			ptrBitmapData += width;
			ptrScreenBuffer += width;
			countBytes += width;
		}
	} while (posHeight);
};

void GameBitmapDrawBitmap(uint8_t* ptrBitmapData, uint8_t* ptrScreenBuffer, uint32_t stride, int16_t posX, int16_t posY, uint16_t width, uint16_t height, uint8_t v134)
{
	ptrScreenBuffer = (stride * posY + posX + ptrScreenBuffer);
	uint8_t pixel;
	int32_t startOffsetX;
	int32_t posWidth;
	uint8_t* ptrScreenBufferLineStart = ptrScreenBuffer;

	do
	{
		while (1)
		{
			while (1)
			{
				startOffsetX = *ptrBitmapData++;

				//Is width byte
				if ((startOffsetX & 0x80u) == 0)
				{
					//Start Drawing
					break;
				}
				ptrScreenBuffer += startOffsetX;
				width = width - startOffsetX;
			}
			if (!startOffsetX)
				break;
			posWidth = startOffsetX;

			//Draw Line
			do
			{
				pixel = *ptrBitmapData++;
				width = width + 1;
				if ((width & 0x80u) == 0)
					*ptrScreenBuffer = pixel;
				--ptrScreenBuffer;
				--posWidth;
			} while (posWidth);
		}
		ptrScreenBufferLineStart += stride;
		ptrScreenBuffer = ptrScreenBufferLineStart;
		width = __PAIR__(height, v134) - 256;
	} while (height);
};

void GameBitmapDrawColourizedBitmap(uint8_t* ptrBitmapData, uint8_t colour, uint8_t* ptrScreenBuffer, uint32_t stride, int16_t posX, int16_t posY, uint8_t posHeight, uint8_t scale)
{
	ptrScreenBuffer = (stride * posY + posX + ptrScreenBuffer);
	int8_t width = 0;
	int8_t posWidth = 0;
	int8_t startOffsetX = -1;
	uint8_t pixel = 0;
	uint8_t* ptrScreenBufferLineStart = ptrScreenBuffer;
	int lineStartBytes = 0;
	int countBytes = 0;
	int scaledLinesDrawn = 0;

	do
	{
		while (1)
		{
			while (1)
			{
				startOffsetX = *ptrBitmapData++;
				countBytes++;

				//Is width byte
				if (startOffsetX)
					break;

				//Move row
				if (scaledLinesDrawn < scale - 1)
				{
					int lineLengthBytes = countBytes - lineStartBytes;
					ptrBitmapData -= lineLengthBytes;
					countBytes -= lineLengthBytes;
					scaledLinesDrawn++;
				}
				else
				{
					posHeight--;
					scaledLinesDrawn = 0;
					lineStartBytes = countBytes;
				}

				ptrScreenBufferLineStart += stride;
				ptrScreenBuffer = ptrScreenBufferLineStart;
				if (!posHeight)
					return;
			}

			//Is width byte
			if ((startOffsetX & 0x80u) == 0)
			{
				//Start Drawing
				break;
			}
			//Is a change of start coordinate
			int offset = (char)startOffsetX;
			ptrScreenBuffer -= offset * scale;
			if (!posHeight)
				return;
		}

		posWidth = startOffsetX;
		width = startOffsetX;

		//Draw line
		if (scale > 1)
		{
			do
			{
				pixel = *ptrBitmapData++;
				countBytes++;

				for (int s = 0; s < scale; s++)
				{
					*ptrScreenBuffer++ = colour;
				}
				posWidth--;
			} while (posWidth);
		}
		else
		{
			memset(ptrScreenBuffer, colour, width);
			ptrBitmapData += width;
			ptrScreenBuffer += width;
			countBytes += width;
		}
	} while (posHeight);
};

void GameBitmapDrawColourizedBitmap(int16_t posX, int16_t posY, bitmap_pos_struct_t a3, uint8_t colour, uint8_t scale)
{
	if (x_WORD_180660_VGA_type_resolution == 1)
	{
		GameBitmapDrawColourizedBitmap(a3.data, colour, pdwScreenBuffer_351628, screenWidth_18062C, posX / 2, posY / 2, a3.height_5 / 2, 1);
	}
	else
	{
		GameBitmapDrawColourizedBitmap(a3.data, colour, pdwScreenBuffer_351628, screenWidth_18062C, posX, posY, a3.height_5, scale);
	}
}

void GameBitmapDrawTransparentBitmap_2DE80(int16_t posX, int16_t posY, bitmap_pos_struct_t a3, uint8_t scale)//20ee80
{
	int32_t startOffsetX; // eax
	int16_t posHeight; // bx
	uint8_t* ptrScreenBuffer;
	uint8_t* ptrBitmapData = nullptr; // edx
	uint8_t* ptrBitmapPixel = nullptr; // esi
	int32_t posWidth; // ecx
	int v15; // [esp+0h] [ebp-Ch]
	int32_t width; // [esp+0h] [ebp-Ch]
	uint8_t* ptrScreenBufferLineStart; // [esp+4h] [ebp-8h]

	if (x_WORD_180660_VGA_type_resolution == 1)
	{
		posHeight = a3.height_5 / 2;
		startOffsetX = posY / 2 * screenWidth_18062C + posX / 2;
		ptrScreenBuffer = (startOffsetX + pdwScreenBuffer_351628);
		ptrBitmapData = a3.data;
		for (ptrScreenBufferLineStart = startOffsetX + pdwScreenBuffer_351628; posHeight; ptrBitmapData += v15)
		{
			while (1)
			{
				while (1)
				{
					LOBYTE(startOffsetX) = *ptrBitmapData++;
					if ((x_BYTE)startOffsetX)
						break;
					posHeight--;
					ptrScreenBufferLineStart += screenWidth_18062C;
					ptrScreenBuffer = ptrScreenBufferLineStart;
					if (!posHeight)
						return;
				}
				if ((startOffsetX & 0x80u) == 0)
					break;
				ptrScreenBuffer -= (char)startOffsetX;
				if (!posHeight)
					return;
			}
			startOffsetX = (char)startOffsetX;//20ef1f
			ptrBitmapPixel = ptrBitmapData;
			posWidth = startOffsetX;
			v15 = (char)startOffsetX;
			HIWORD(startOffsetX) = 0;
			do
			{
				LOBYTE(startOffsetX) = *ptrBitmapPixel++;
				HIBYTE(startOffsetX) = *ptrScreenBuffer;
				LOBYTE(startOffsetX) = x_BYTE_F6EE0_tablesx[0x4000 + startOffsetX];
				*ptrScreenBuffer++ = startOffsetX;
				posWidth--;
			} while (posWidth);
		}
	}
	else
	{
		if (a3.height_5)
		{
			startOffsetX = posX + screenWidth_18062C * posY;
			posHeight = a3.height_5;

			ptrScreenBuffer = (startOffsetX + pdwScreenBuffer_351628);
			ptrScreenBufferLineStart = (startOffsetX + pdwScreenBuffer_351628);
			ptrBitmapData = a3.data;
			int lineStartBytes = 0;
			int countBytes = 0;
			int scaledLinesDrawn = 0;

			do
			{
				while (1)
				{
					while (1)
					{
						LOBYTE(startOffsetX) = *ptrBitmapData++;
						countBytes++;

						//If it has value
						if ((x_BYTE)startOffsetX)
							break;

						//Move row
						if (scaledLinesDrawn < scale - 1)
						{
							int lineLengthBytes = countBytes - lineStartBytes;
							ptrBitmapData -= lineLengthBytes;
							countBytes -= lineLengthBytes;
							scaledLinesDrawn++;
						}
						else
						{
							posHeight--;
							scaledLinesDrawn = 0;
							lineStartBytes = countBytes;
						}

						ptrScreenBufferLineStart += screenWidth_18062C;
						ptrScreenBuffer = ptrScreenBufferLineStart;
						if (!posHeight)
							return;
					}

					//Is width byte
					if ((startOffsetX & 0x80u) == 0)
					{
						//Start Drawing
						break;
					}
					//Is a change of start coordinate
					int offset = (char)startOffsetX;
					ptrScreenBuffer -= offset * scale;
					if (!posHeight)
						return;
				}
				posWidth = LOBYTE(startOffsetX);
				width = LOBYTE(startOffsetX);
				ptrBitmapPixel = ptrBitmapData;
				HIWORD(startOffsetX) = 0;
				//Draw line
				do
				{
					for (int s = 0; s < scale; s++)
					{
						LOBYTE(startOffsetX) = *ptrBitmapPixel;
						HIBYTE(startOffsetX) = *ptrScreenBuffer;
						LOBYTE(startOffsetX) = x_BYTE_F6EE0_tablesx[0x4000 + startOffsetX];
						*ptrScreenBuffer++ = startOffsetX;
					}
					ptrBitmapPixel++;
					countBytes++;
					posWidth--;
				} while (posWidth);
				ptrBitmapData += width;
			} while (posHeight);
		}
	}
};

void GameBitmapDrawMenuGraphic(uint16_t width, uint16_t height, uint8_t scale, uint8_t* ptrSrc, uint8_t* ptrDest)
{
	int lineCount = 0;
	int index = 0;
	int lineStartIndex = 0;
	int byteCount = 0;
	int32_t pixel = 0;

	while (lineCount < height)
	{
		while (lineCount < height)
		{
			LOBYTE(pixel) = ptrSrc[index];
			index++;
			if ((char)pixel)
				break;

			//line ended, move row
			lineStartIndex += width;
			byteCount = lineStartIndex;
			lineCount++;
		}

		if (lineCount < height)
		{
			if ((pixel & 0x80u) == 0)
			{
				uint16_t lnWidth = (char)pixel * scale;

				//Draw line
				for (int x = 0; x < lnWidth; x++)
				{
					ptrDest[byteCount] = ptrSrc[index];
					byteCount++;
					index++;
				}
			}
			else
			{
				byteCount -= (char)pixel * scale;
			}
		}
	}
};

void sub_8F935_bitmap_draw_final(uint8_t width, uint8_t height, uint16_t tiley, int tilex, uint8_t* texture, uint8_t setbyte, char a6, uint8_t scale)//270935
{
	uint8_t* ptrScreenBuffer;
	uint8_t* ptrScreenBufferLineStart;
	uint8_t* pixel_buffer_index; // edi
	int32_t startOffsetX;
	int32_t posWidth;
	uint8_t pixel; // al

	char v12; // al
	char v13; // al
	int v15; // ecx
	char v18; // al
	char v19; // al
	char v20; // al
	unsigned int v29; // ecx
	uint8_t* v32; // edi
	char v33; // al
	char* v34; // esi
	int v35; // ebp
	int v37; // ecx
	int v40; // eax
	int v42; // ecx
	unsigned __int8 i; // dl
	char v44; // al
	char v45; // al
	unsigned __int8 v46; // of
	char v47; // dl
	unsigned __int8 v48; // al
	char v50; // al
	char v51; // dl
	char v52; // al
	char v53; // dl
	unsigned int v54; // ebx
	const void* v55; // esi
	unsigned int v62; // ecx
	uint8_t* v65; // edi
	char v66; // al
	char* v67; // esi
	uint8_t v69l;
	uint8_t v69h;
	int8_t v72l; // ecx
	char v73; // al
	char v74; // al
	bool v75; // zf
	bool v76; // sf
	char v77; // al
	char v78; // al
	int v79; // ebx
	int v85; // ebx
	x_BYTE* v86; // edi
	int v87; // ecx
	int v92; // ecx
	int v93; // ebx
	char v96; // al
	int v106; // ecx
	int v107; // ebx
	char v110; // al
	int v112; // ecx
	int v113; // ebx
	char v116; // al
	unsigned int v117; // ebx
	x_BYTE* v118; // edi
	x_BYTE* v119; // edx
	int v120; // ecx
	char v121; // al
	char v122; // al
	char v123; // al
	char v124; // al
	int v125; // eax
	int v126; // edi
	int v128; // eax
	int v129; // eax
	int v130; // eax
	char* v131; // [esp-4h] [ebp-Ch]
	uint8_t v132; // [esp+2h] [ebp-6h]
	unsigned __int8 v133; // [esp+2h] [ebp-6h]
	unsigned __int8 v134; // [esp+2h] [ebp-6h]
	unsigned __int8 v135; // [esp+2h] [ebp-6h]
	unsigned __int8 v137; // [esp+2h] [ebp-6h]
	unsigned __int8 v138; // [esp+2h] [ebp-6h]
	unsigned __int8 v139; // [esp+2h] [ebp-6h]
	char v140; // [esp+3h] [ebp-5h]
	char v141; // [esp+3h] [ebp-5h]

	if (!(height))//453558
		return;
	pixel_buffer_index = pdwScreenBuffer_351628 + x_DWORD_18063C_sprite_sizex + screenWidth_18062C * x_DWORD_180650_positiony;
	if (x_WORD_180660_VGA_type_resolution & 1)//if 320x200 is resolved, the value is halved
	{
		width /= 2;
		height /= 2;
		tilex /= 2;
		tiley /= 2;
	}
	if (tiley < 0)
	{
		if (x_WORD_E36D4 & 2)
		{
			v128 = tiley + height;
			v46 = __OFADD__(1, v128);
			v129 = v128 + 1;
			if (((v129 < 0) ^ v46) | (v129 == 0))
				return;
			tiley = -1;
			height = v129;
		}
		else
		{
			v130 = tiley + height;
			if (((tiley + height < 0) ^ __OFADD__(tiley, height)) | (tiley + height == 0))
				return;
			height += tiley;
			v130 = 0;
			do
			{
				while (1)
				{
					v130 = *texture++;
					if (!v130)
						break;
					if ((v130 & 0x80u) == 0)
						texture += v130;
				}
				tiley++;
			} while (tiley);
		}
	}
	else if (tiley + height >= x_DWORD_180644_map_resolution2_y)
	{
		if (x_WORD_E36D4 & 2)
		{
			if (tiley + 1 >= x_DWORD_180644_map_resolution2_y)
				return;
			v125 = height + tiley + 1 - x_DWORD_180644_map_resolution2_y;
			height = x_DWORD_180644_map_resolution2_y - (tiley + 1);
			v126 = v125;
			startOffsetX = 0;
			do
			{
				while (1)
				{
					startOffsetX = *texture++;
					if (!startOffsetX)
						break;
					if ((startOffsetX & 0x80u) == 0)
						texture += startOffsetX;
				}
				--v126;
			} while (v126);
			pixel_buffer_index = pdwScreenBuffer_351628;
		}
		else
		{
			if (x_DWORD_180644_map_resolution2_y <= tiley)
				return;
			height = x_DWORD_180644_map_resolution2_y - tiley;
		}
	}
	if (tilex >= 0)
	{
		if (tilex + width >= x_DWORD_180648_map_resolution2_x)
		{
			if (x_WORD_E36D4)
			{
				if (x_WORD_E36D4 & 1)
				{
					if (x_WORD_E36D4 & 2)
					{
						if (x_DWORD_180634_screen_width - tilex >= 0)
						{
							v79 = width + tilex;
							ptrScreenBuffer = (screenWidth_18062C * (height + tiley) + v79 + pixel_buffer_index);
							posWidth = 0;
							v79 = x_DWORD_180634_screen_width - v79 - 2;
							width = v79;
							v133 = v79;
							startOffsetX = -1;
							ptrScreenBufferLineStart = ptrScreenBuffer;
							do
							{
								while (1)
								{
									while (1)
									{
										startOffsetX = *texture++;
										if ((startOffsetX & 0x80u) == 0)
											break;
										ptrScreenBuffer += startOffsetX;
										width = width - startOffsetX;
									}
									if (!startOffsetX)
										break;
									posWidth = startOffsetX;
									do
									{
										pixel = *texture++;
										width = width + 1;
										if ((width & 0x80u) == 0)
											*ptrScreenBuffer = pixel;
										--ptrScreenBuffer;
										--posWidth;
									} while (posWidth);
								}
								ptrScreenBufferLineStart -= screenWidth_18062C;
								ptrScreenBuffer = ptrScreenBufferLineStart;
								width = __PAIR__(height, v133) - 256;
							} while (height);
						}
					}
					else if (x_DWORD_180634_screen_width - tilex >= 0)
					{
						v85 = width + tilex;
						v86 = (x_BYTE*)(screenWidth_18062C * tiley + v85 + pixel_buffer_index);
						v87 = 0;
						v85 = x_DWORD_180634_screen_width - v85 - 2;
						width = v85;
						v134 = v85;
						GameBitmapDrawBitmap(texture, pixel_buffer_index, screenWidth_18062C, v85, tiley, width, height, v134);
					}
				}
				else if (x_WORD_E36D4 & 2)
				{
					ptrScreenBuffer = (screenWidth_18062C * (height + tiley) + tilex + pixel_buffer_index);
					v92 = 0;
					v93 = x_DWORD_180634_screen_width - tilex;
					if (v93 >= 0)
					{
						width = v93;
						v135 = v93;
						startOffsetX = -1;
						ptrScreenBufferLineStart = ptrScreenBuffer;
						do
						{
							while (1)
							{
								while (1)
								{
									startOffsetX = *texture++;
									if ((startOffsetX & 0x80u) == 0)
										break;
									ptrScreenBuffer -= startOffsetX;
									width = startOffsetX + width;
								}
								if (!startOffsetX)
									break;
								v92 = startOffsetX;
								do
								{
									v96 = *texture++;
									width = width - 1;
									if ((width & 0x80u) == 0)
										*ptrScreenBuffer++ = v96;
									--v92;
								} while (v92);
							}
							ptrScreenBufferLineStart -= screenWidth_18062C;
							ptrScreenBuffer = ptrScreenBufferLineStart;
							width = __PAIR__(height, v135) - 256;
						} while (width);
					}
				}
			}
			else
			{
				if (x_DWORD_180634_screen_width - tilex >= 0)
				{
					ptrScreenBuffer = (screenWidth_18062C * tiley + tilex + pixel_buffer_index);
					v69l = x_DWORD_180634_screen_width - tilex;
					v69h = height;//ebx
					v132 = v69l;//ebp-6
					ptrScreenBufferLineStart = ptrScreenBuffer;//edx edi
					v72l = 0;//ecx
					do
					{
						while (1)
						{
							while (1)
							{
								v73 = *texture++;
								if (v73 >= 0)
									break;
								v46 = __OFADD__(v73, v69l);//fix
								v75 = v73 + v69l == 0;//fix
								v76 = (char)(v73 + v69l) < 0;//fix
								v69l = v73 + v69l;//fix
								if (!((v76 ^ v46) | v75))
								{
									ptrScreenBuffer -= v73;
									v77 = *texture++;
									v72l = v77;
									do
									{
										v78 = *texture++;
										v69l--;
										if ((v69l & 0x80u) == 0)//fix
											*ptrScreenBuffer++ = v78;
										v72l--;
									} while (v72l);
								}
							}
							if (!v73)
								break;
							v72l = v73;
							do
							{
								v74 = *texture++;
								v69l--;
								if ((v69l & 0x80u) == 0)//fix
									*ptrScreenBuffer++ = v74;
								v72l--;
							} while (v72l);
						}
						ptrScreenBufferLineStart += screenWidth_18062C;
						ptrScreenBuffer = ptrScreenBufferLineStart;

						v69l = /*(v69l&0xff00)+*/v132;//fix
						v69h--;
					} while (v69h);//fix
				}
			}
		}
		else if (x_WORD_E36D4 & 1)
		{
			if (x_WORD_E36D4 & 2)
			{
				ptrScreenBuffer = (screenWidth_18062C * (height + tiley) + width + tilex + pixel_buffer_index);
				posWidth = 0;
				startOffsetX = -1;
				ptrScreenBufferLineStart = ptrScreenBuffer;
				do
				{
					while (1)
					{
						while (1)
						{
							startOffsetX = *texture++;

							//Is width byte
							if ((startOffsetX & 0x80u) == 0)
							{
								//Start Drawing
								break;
							}
							ptrScreenBuffer += startOffsetX;
							v12 = *texture++;
							posWidth = v12;
							do
							{
								v13 = *texture++;
								*ptrScreenBuffer-- = v13;
								--posWidth;
							} while (posWidth);
						}
						if (!startOffsetX)
							break;
						posWidth = startOffsetX;
						do
						{
							pixel = *texture++;
							*ptrScreenBuffer-- = pixel;
							--posWidth;
						} while (posWidth);
					}
					ptrScreenBufferLineStart -= screenWidth_18062C;
					ptrScreenBuffer = ptrScreenBufferLineStart;
					--height;
				} while (height);
			}
			else
			{
				ptrScreenBuffer = (screenWidth_18062C * tiley + width + tilex + pixel_buffer_index);
				v15 = 0;
				startOffsetX = -1;
				ptrScreenBufferLineStart = ptrScreenBuffer;
				do
				{
					while (1)
					{
						while (1)
						{
							startOffsetX = *texture++;
							if ((startOffsetX & 0x80u) == 0)
							{
								//Start Drawing
								break;
							}
							ptrScreenBuffer += startOffsetX;
							v19 = *texture++;
							v15 = v19;
							do
							{
								v20 = *texture++;
								*ptrScreenBuffer-- = v20;
								--v15;
							} while (v15);
						}
						if (!startOffsetX)
							break;
						v15 = startOffsetX;
						do
						{
							v18 = *texture++;
							*ptrScreenBuffer-- = v18;
							--v15;
						} while (v15);
					}
					ptrScreenBufferLineStart += screenWidth_18062C;
					ptrScreenBuffer = ptrScreenBufferLineStart;
					--height;
				} while (height);
			}
		}
		else if (x_WORD_E36D4)
		{
			if (x_WORD_E36D4 & 2)
			{
				ptrScreenBuffer = (screenWidth_18062C * (height + tiley) + tilex + pixel_buffer_index);
				v29 = 0;
				startOffsetX = -1;
				ptrScreenBufferLineStart = ptrScreenBuffer;
				do
				{
					while (1)
					{
						while (1)
						{
							startOffsetX = *texture++;
							if ((startOffsetX & 0x80u) == 0)
							{
								//Start Drawing
								break;
							}
							v32 = &ptrScreenBuffer[-startOffsetX];
							v33 = *texture;
							v34 = (char*)(texture + 1);
							v29 = v33;
							qmemcpy(v32, v34, v29);
							texture = (uint8_t*)&v34[v29];
							ptrScreenBuffer = &v32[v29];
							v29 = 0;
						}
						if (!startOffsetX)
							break;
						v29 = startOffsetX;
						qmemcpy(ptrScreenBuffer, texture, v29);
						texture += v29;
						ptrScreenBuffer += v29;
						v29 = 0;
					}
					ptrScreenBufferLineStart -= screenWidth_18062C;
					ptrScreenBuffer = ptrScreenBufferLineStart;
					--height;
				} while (height);
			}
			else if (x_WORD_E36D4 & 4)
			{
				v35 = x_DWORD_E3890;
				ptrScreenBuffer = (screenWidth_18062C * tiley + tilex + pixel_buffer_index);
				v37 = 0;
				ptrScreenBufferLineStart = ptrScreenBuffer;
				do
				{
					while (1)
					{
						while (1)
						{
							startOffsetX = *texture++;
							if ((startOffsetX & 0x80u) == 0)
							{
								//Start Drawing
								break;
							}
							ptrScreenBuffer -= (char)startOffsetX;
						}
						if (!startOffsetX)
							break;
						v37 = startOffsetX;
						v40 = startOffsetX;
						do
						{
							v40 = *texture++;
							v40 = *ptrScreenBuffer;
							*ptrScreenBuffer++ = *(x_BYTE*)(v40 + v35);
							--v37;
						} while (v37);
					}
					ptrScreenBufferLineStart += screenWidth_18062C;
					ptrScreenBuffer = ptrScreenBufferLineStart;
					--height;
				} while (height);
			}
			else if (x_WORD_E36D4 & 8)
			{
				ptrScreenBuffer = (screenWidth_18062C * tiley + tilex + pixel_buffer_index);
				v42 = 0;
				ptrScreenBufferLineStart = ptrScreenBuffer;
				v140 = height;
				for (i = setbyte; ; i = setbyte)
				{
					while (1)
					{
						while (1)
						{
							v44 = *texture++;
							if (v44 >= 0)
								break;
							v42 = (unsigned __int8)-v44;
							do
							{
								v46 = __OFSUB__(i--, 1);
								if (((i & 0x80u) != 0) ^ v46)
								{
									i = setbyte;
									++ptrScreenBuffer;
								}
								--v42;
							} while (v42);
						}
						if (!v44)
							break;
						LOBYTE(v42) = v44;
						do
						{
							v45 = *texture++;
							v46 = __OFSUB__(i--, 1);
							if (((i & 0x80u) != 0) ^ v46)
							{
								i = setbyte;
								*ptrScreenBuffer++ = v45;
							}
							--v42;
						} while (v42);
					}
					if (!--v140)
						break;
					v47 = a6;
					while (--v47 >= 0)
					{
						while (1)
						{
							do
								v48 = *texture++;
							while ((v48 & 0x80u) != 0);
							if (!v48)
								break;
							texture += v48;
						}
					}
					ptrScreenBufferLineStart += screenWidth_18062C;
					ptrScreenBuffer = ptrScreenBufferLineStart;
				}
			}
			else if (x_WORD_E36D4 & 0x20)
			{
				ptrScreenBuffer = (screenWidth_18062C * tiley + tilex + pixel_buffer_index);
				ptrScreenBufferLineStart = ptrScreenBuffer;
				v141 = height;
				do
				{
					while (1)
					{
						while (1)
						{
							v50 = *texture++;
							if (v50 >= 0)
								break;
							ptrScreenBuffer += (setbyte * -v50);
						}
						if (!v50)
							break;
						v51 = v50;
						do
						{
							v52 = *texture++;
							memset(ptrScreenBuffer, v52, setbyte);
							ptrScreenBuffer += setbyte;
							v46 = __OFSUB__(v51--, 1);
						} while (!(((v51 < 0) ^ v46) | (v51 == 0)));
					}
					v53 = a6 - 1;
					if (a6 > 1)
					{
						v131 = (char*)texture;
						v54 = ptrScreenBuffer - ptrScreenBufferLineStart;
						do
						{
							v55 = ptrScreenBufferLineStart;
							ptrScreenBufferLineStart += screenWidth_18062C;
							qmemcpy(ptrScreenBufferLineStart, (void*)v55, v54);
							v46 = __OFSUB__(v53--, 1);
						} while (!(((v53 < 0) ^ v46) | (v53 == 0)));
						texture = (uint8_t*)v131;
					}
					ptrScreenBufferLineStart += screenWidth_18062C;
					ptrScreenBuffer = ptrScreenBufferLineStart;
					--v141;
				} while (v141);
			}
			else if (x_WORD_E36D4 & 0x40)
			{
				GameBitmapDrawColourizedBitmap(texture, setbyte, pixel_buffer_index, screenWidth_18062C, tilex, tiley, height, scale);
			}
			else
			{
				ptrScreenBuffer = (screenWidth_18062C * tiley + tilex + pixel_buffer_index);
				v62 = 0;
				startOffsetX = -1;
				ptrScreenBufferLineStart = ptrScreenBuffer;
				do
				{
					while (1)
					{
						while (1)
						{
							startOffsetX = *texture++;
							if ((startOffsetX & 0x80u) == 0)
							{
								//Start Drawing
								break;
							}
							v65 = &ptrScreenBuffer[-startOffsetX];
							v66 = *texture;
							v67 = (char*)(texture + 1);
							v62 = v66;
							qmemcpy(v65, v67, v62);
							texture = (uint8_t*)&v67[v62];
							ptrScreenBuffer = &v65[v62];
							v62 = 0;
						}
						if (!startOffsetX)
							break;
						v62 = startOffsetX;
						qmemcpy(ptrScreenBuffer, texture, v62);
						texture += v62;
						ptrScreenBuffer += v62;
						v62 = 0;
					}
					ptrScreenBufferLineStart += screenWidth_18062C;
					ptrScreenBuffer = ptrScreenBufferLineStart;
					--height;
				} while (height);
			}
		}
		else
		{
			GameBitmapDrawBitmap(texture, pixel_buffer_index, screenWidth_18062C, tilex, tiley, height, scale);
		}
		return;
	}
	if (!(x_WORD_E36D4 & 1))
	{
		if (!(x_WORD_E36D4 & 2))
			return;
		v117 = -tilex;
		if (width <= v117)
			return;
		v118 = (x_BYTE*)(screenWidth_18062C * (height + tiley) + pixel_buffer_index);
		v117 = height;
		v119 = v118;
		v139 = v117;
		v120 = 0;
		while (1)
		{
			while (1)
			{
				while (1)
				{
					v121 = *texture++;
					if (v121 < 0)
						break;
					if (v121)
					{
						v120 = v121;
						do
						{
							v122 = *texture++;
							v117 = v117 - 1;
							if ((v117 & 0x80u) != 0)
								*v118++ = v122;
							v120--;
						} while (v120);
					}
					else
					{
						v119 -= screenWidth_18062C;
						v118 = v119;
						v117 = __PAIR__(v117, v139) - 256;
						if (!v117)
							return;
					}
				}
				if ((char)v117 > 0)
					break;
			LABEL_225:
				v118 -= v121;
				v123 = *texture++;
				v120 = v123;
				do
				{
					v124 = *texture++;
					v117 = v117 - 1;
					if ((v117 & 0x80u) != 0)
						*v118++ = v124;
					v120--;
				} while (v120);
			}
			v117 = v121 + v117;
			if ((v117 & 0x80u) != 0)
			{
				v121 = v117;
				goto LABEL_225;
			}
		}
	}
	if (x_WORD_E36D4 & 2)
	{
		ptrScreenBuffer = (screenWidth_18062C * (height + tiley) + width + tilex + pixel_buffer_index);
		v106 = 0;
		v107 = width + tilex + 1;
		if (v107 >= 0)
		{
			width = v107;
			v137 = v107;
			ptrScreenBufferLineStart = ptrScreenBuffer;
			startOffsetX = -1;
			do
			{
				while (1)
				{
					while (1)
					{
						startOffsetX = *texture++;
						if ((startOffsetX & 0x80u) == 0)
						{
							//Start Drawing
							break;
						}
						ptrScreenBuffer += startOffsetX;
						width = startOffsetX + width;
					}
					if (!startOffsetX)
						break;
					v106 = startOffsetX;
					do
					{
						v110 = *texture++;
						width = width - 1;
						if ((width & 0x80u) == 0)
							*ptrScreenBuffer = v110;
						ptrScreenBuffer--;
						v106--;
					} while (v106);
				}
				ptrScreenBufferLineStart -= screenWidth_18062C;
				ptrScreenBuffer = ptrScreenBufferLineStart;
				width = __PAIR__(height, v137) - 256;
			} while (height);
		}
	}
	else
	{
		ptrScreenBuffer = (screenWidth_18062C * tiley + width + tilex + pixel_buffer_index);
		v112 = 0;
		v113 = width + tilex + 1;
		if (v113 >= 0)
		{
			width = v113;
			v138 = v113;
			ptrScreenBufferLineStart = ptrScreenBuffer;
			startOffsetX = -1;
			do
			{
				while (1)
				{
					while (1)
					{
						startOffsetX = *texture++;
						if ((startOffsetX & 0x80u) == 0)
						{
							//Start Drawing
							break;
						}
						ptrScreenBuffer += startOffsetX;
						width = startOffsetX + width;
					}
					if (!startOffsetX)
						break;
					posWidth = startOffsetX;
					do
					{
						v116 = *texture++;
						width = width - 1;
						if ((width & 0x80u) == 0)
							*ptrScreenBuffer = v116;
						ptrScreenBuffer--;
						posWidth--;
					} while (posWidth);
				}
				ptrScreenBufferLineStart += screenWidth_18062C;
				ptrScreenBuffer = ptrScreenBufferLineStart;
				width = __PAIR__(width, v138) - 256;
			} while (height);
		}
	}
}


int sub_mainNoRNC(const char palfilename[], const char tmapsdatfilename[], const char tmapstabfilename[], const char tmapsstr[], int max_images, ImageType imageType, int padding, bool caveSprites, const char outputPath[])
{
	std::vector<std::string> filesToDelete;
	double colourMultiplier = 4;

	FILE* fptrTMAPSdata;
	fopen_s(&fptrTMAPSdata, tmapsdatfilename, "rb");
	fseek(fptrTMAPSdata, 0L, SEEK_END);
	long sz = ftell(fptrTMAPSdata);
	fseek(fptrTMAPSdata, 0L, SEEK_SET);
	Bit8u* contentTMAPSdat = (Bit8u*)malloc(sz * sizeof(char*));
	fread(contentTMAPSdat, sz, 1, fptrTMAPSdata);
	fclose(fptrTMAPSdata);

	FILE* fptrTMAPStab;
	fopen_s(&fptrTMAPStab, tmapstabfilename, "rb");
	fseek(fptrTMAPStab, 0L, SEEK_END);
	long sztab = ftell(fptrTMAPStab);
	fseek(fptrTMAPStab, 0L, SEEK_SET);
	bitmap_pos_struct_tm* contentTMAPStab = (bitmap_pos_struct_tm*)malloc(sztab * sizeof(char*));
	fread(contentTMAPStab, sztab, 1, fptrTMAPStab);
	fclose(fptrTMAPStab);

	int count = sztab / sizeof(bitmap_pos_struct_t);

	//Bit8u buffer[100000];
	//Bit8u prevbuffer[100000];

	int indextab = 0;
	int index = 0;

	//int dword_0xE6_heapsize_230 = 0x400000;
	//Bit8u* pointer_0xE2_heapbuffer_226 = (Bit8u*)sub_83CD0_malloc2(dword_0xE6_heapsize_230);
	//x_DWORD_E9C28_str = sub_71B40(dword_0xE6_heapsize_230, 0x1F8u, (type_x_DWORD_E9C28_str*)pointer_0xE2_heapbuffer_226);
	//TMAPS00TAB_BEGIN_BUFFER = contentTMAPStab;
	//x_DWORD_E9C08x = sub_72120(0x1F8u);

	char outname[512];
	char outnameAlpha[512];
	char title[512];

	Bit8u pallettebuffer[768];
	FILE* palfile = nullptr;

	fopen_s(&palfile, palfilename, "rb");
	fread(pallettebuffer, 768, 1, palfile);
	fclose(palfile);

	while (index < count)
	{
		int begin = contentTMAPStab[index].data;
		int end = contentTMAPStab[index+1].data;
		int size = end - begin;
		
		Bit8u* stmpdat = &contentTMAPSdat[begin];

		int width = contentTMAPStab[index].width_4;
		int height = contentTMAPStab[index].height_5;

		//buffer = stmpdat;
		//memcpy(pdwScreenBuffer_351628, stmpdat, size);
		bitmap_pos_struct_t a3;
		a3.data = (uint8*)contentTMAPStab[index].data;
		a3.width_4 = contentTMAPStab[index].width_4;
		a3.height_5 = contentTMAPStab[index].height_5;
		//GameBitmapDrawTransparentBitmap_2DE80(0, 0, a3, 0);//20ee80
		memset(pdwScreenBuffer_351628, 0, 100000);
		GameBitmapDrawMenuGraphic(contentTMAPStab[index].width_4, contentTMAPStab[index].height_5, 1, stmpdat, pdwScreenBuffer_351628);
		//sub_8F935_bitmap_draw_final(contentTMAPStab[index].width_4*2, contentTMAPStab[index].height_5*2, 0, 0, stmpdat, 0, 0, 1);

		if (imageType == ImageType::bmp)
		{
			sprintf_s(outname, "%s\\%s%03i-00.bmp", outputPath, tmapsstr, index);
			BitmapIO::WriteRGBAImageBufferAsImageBMP(outname, width, height, pallettebuffer, pdwScreenBuffer_351628, colourMultiplier);
		}

		if (imageType == ImageType::png)
		{
			if (isOther(other_folder, index))
				sprintf_s(outname, "%s\\%s%03i-00-other", outputPath, tmapsstr, index);
			else
				sprintf_s(outname, "%s\\%s%03i-00", outputPath, tmapsstr, index);

			sprintf_s(title, "%s%03i", tmapsstr, index);
			BitmapIO::WritePosistructToPng(pallettebuffer, pdwScreenBuffer_351628, width, height, outname, title, padding, colourMultiplier);
		}

		if (imageType == ImageType::pnga)
		{
			sprintf_s(outname, "%s\\%s%03i-00", outputPath, tmapsstr, index);
			sprintf_s(title, "%s%03i", tmapsstr, index);
			BitmapIO::WritePosistructToPng(pallettebuffer, pdwScreenBuffer_351628, width, height, outname, title, padding, colourMultiplier);
			sprintf_s(outname, "%s\\%s%03i-alpha-00", outputPath, tmapsstr, index);
			BitmapIO::WritePosistructToAlphaPng(pallettebuffer, pdwScreenBuffer_351628, width, height, outname, title, padding);
		}

		if (caveSprites && index < 452)
			indextab += 10;
		else if (!caveSprites)
			indextab += 10;
		
		index++;
	}

	printf("Extraction Completed\n");
	return 0;
}

int sub_main(const char palfilename[], const char tmapsdatfilename[], const char tmapstabfilename[], const char tmapsstr[], int max_images, ImageType imageType, int padding, bool caveSprites, const char outputPath[])
{
	std::vector<std::string> filesToDelete;
	double colourMultiplier = 4;

	FILE* fptrTMAPSdata;
	fopen_s(&fptrTMAPSdata, tmapsdatfilename, "rb");
	fseek(fptrTMAPSdata, 0L, SEEK_END);
	long sz = ftell(fptrTMAPSdata);
	fseek(fptrTMAPSdata, 0L, SEEK_SET);
	Bit8u* contentTMAPSdat = (Bit8u*)malloc(sz * sizeof(char*));
	fread(contentTMAPSdat, sz, 1, fptrTMAPSdata);
	fclose(fptrTMAPSdata);

	FILE* fptrTMAPStab;
	fopen_s(&fptrTMAPStab, tmapstabfilename, "rb");
	fseek(fptrTMAPStab, 0L, SEEK_END);
	long sztab = ftell(fptrTMAPStab);
	fseek(fptrTMAPStab, 0L, SEEK_SET);
	Bit8u* contentTMAPStab = (Bit8u*)malloc(sztab * sizeof(char*));
	fread(contentTMAPStab, sztab, 1, fptrTMAPStab);
	fclose(fptrTMAPStab);

	Bit8u buffer[100000];
	Bit8u prevbuffer[100000];

	int indextab = 0;
	int index = 0;

	int dword_0xE6_heapsize_230 = 0x400000;
	Bit8u* pointer_0xE2_heapbuffer_226 = (Bit8u*)sub_83CD0_malloc2(dword_0xE6_heapsize_230);
	x_DWORD_E9C28_str = sub_71B40(dword_0xE6_heapsize_230, 0x1F8u, (type_x_DWORD_E9C28_str*)pointer_0xE2_heapbuffer_226);
	TMAPS00TAB_BEGIN_BUFFER = contentTMAPStab;
	x_DWORD_E9C08x = sub_72120(0x1F8u);

	char outname[512];
	char outnameAlpha[512];
	char title[512];

	Bit8u pallettebuffer[768];
	FILE* palfile = nullptr;

	fopen_s(&palfile, palfilename, "rb");
	fread(pallettebuffer, 768, 1, palfile);
	fclose(palfile);

	while (index < max_images)
	{
		//if (index == 186)
		//{
		//	index++;
		//	index--;
		//}
		//int size = *(Bit32u*)&contentTMAPStab[indextab];
		int shift = *(Bit32u*)&contentTMAPStab[indextab + 4];
		//if (shift > 500)shift = shift - 500;
		Bit8u* stmpdat = &contentTMAPSdat[shift];

		while ((*(Bit32u*)stmpdat) != 0x1434e52) { shift++; stmpdat = &contentTMAPSdat[shift + 1]; }

		Bit32u size = stmpdat[11] + (stmpdat[10] << 8) + (stmpdat[9] << 16) + (stmpdat[8] << 24) + 12;
		Bit32u unpacksize = stmpdat[7] + (stmpdat[6] << 8) + (stmpdat[5] << 16) + (stmpdat[4] << 24);

		if (imageType == ImageType::rnc)
		{
			FILE* fptw;
			char filename[300];
			sprintf_s(filename, "%s\\out\\%s%03i.rnc", outputPath, tmapsstr, index);
			fopen_s(&fptw, filename, "wb");
			fwrite(&contentTMAPSdat[shift], size, 1, fptw);
			fclose(fptw);
		}

		int decompsize = *(Bit32u*)&contentTMAPSdat[shift + 6];
		sub_5C3D0_file_decompress(&contentTMAPSdat[shift], buffer);

		Bit8u* index2 = 10 * index + TMAPS00TAB_BEGIN_BUFFER;
		x_DWORD_F66F0[index] = (int)sub_71E70(x_DWORD_E9C28_str, (unsigned __int16)(4 * ((unsigned int)(*(x_DWORD*)index2 + 13) >> 2)), index);
		int index6 = x_DWORD_F66F0[index];
		Bit8u** subpointer = (Bit8u**)x_DWORD_F66F0[index];
		*subpointer = (Bit8u*)malloc(unpacksize);
		memcpy(*subpointer, buffer, unpacksize);

		if (**(x_BYTE**)index6 & 1)
			/*index = */sub_721C0_initTmap(x_DWORD_E9C08x, (int*)index6, index);
		/*
				if (**(x_BYTE **)index6 & 1)
			index = sub_721C0_initTmap((unsigned __int16*)x_DWORD_E9C08, (int*)index6, index);
		*/

		int width = *(Bit16u*)&buffer[2];
		int height = *(Bit16u*)&buffer[4];

		//Used for later comparison
		FILE* fptw2;
		char filenamedata[300];
		sprintf_s(filenamedata, "%s\\%s%03i-00.data", outputPath, tmapsstr, index);
		fopen_s(&fptw2, filenamedata, "wb");
		fwrite(buffer, unpacksize, 1, fptw2);
		fclose(fptw2);
		filesToDelete.push_back(filenamedata);

		if (imageType == ImageType::bmp)
		{
			sprintf_s(outname, "%s\\%s%03i-00.bmp", outputPath, tmapsstr, index);
			BitmapIO::WriteRGBAImageBufferAsImageBMP(outname, width, height, pallettebuffer, buffer + 6, colourMultiplier);
		}

		if (imageType == ImageType::png)
		{
			if (isOther(other_folder, index))
				sprintf_s(outname, "%s\\%s%03i-00-other", outputPath, tmapsstr, index);
			else
				sprintf_s(outname, "%s\\%s%03i-00", outputPath, tmapsstr, index);

			sprintf_s(title, "%s%03i", tmapsstr, index);
			BitmapIO::WritePosistructToPng(pallettebuffer, buffer + 6, width, height, outname, title, padding, colourMultiplier);
		}

		if (imageType == ImageType::pnga)
		{
			sprintf_s(outname, "%s\\%s%03i-00", outputPath, tmapsstr, index);
			sprintf_s(title, "%s%03i", tmapsstr, index);
			BitmapIO::WritePosistructToPng(pallettebuffer, buffer + 6, width, height, outname, title, padding, colourMultiplier);
			sprintf_s(outname, "%s\\%s%03i-alpha-00", outputPath, tmapsstr, index);
			BitmapIO::WritePosistructToAlphaPng(pallettebuffer, buffer + 6, width, height, outname, title, padding);
		}

		if (caveSprites && index < 452)
			indextab += 10;
		else if (!caveSprites)
			indextab += 10;

		index++;
	}

	for (int mainindex = 0; mainindex < 24; mainindex++)
	{
		index = 0;
		while (index < max_images) {
			Bit8u* subpointer = *(Bit8u**)x_DWORD_F66F0[index];
			subpointer[0] |= 8;
			index++;
		}

		sub_715B0();
		//image2
		indextab = 0;
		index = 0;
		while (index < max_images)
		{

			Bit8u* subpointer = *(Bit8u**)x_DWORD_F66F0[index];
			//memcpy(buffer, subpointer, unpacksize);

			//int shift = *(Bit32u*)&contentTMAPStab[indextab + 4];
			Bit8u* stmpdat = &subpointer[0/*shift*/];
			//Bit32u size = stmpdat[11] + (stmpdat[10] << 8) + (stmpdat[9] << 16) + (stmpdat[8] << 24) + 12;
			//Bit32u unpacksize = stmpdat[7] + (stmpdat[6] << 8) + (stmpdat[5] << 16) + (stmpdat[4] << 24);

			int width = *(Bit16u*)&stmpdat[2];
			int height = *(Bit16u*)&stmpdat[4];

			memcpy(buffer, stmpdat, width * height + 6);
			/*
			FILE* fptw;
			char filename[300];
			sprintf_s(filename, "c:\\prenos\\remc2\\tools\\decompressTMAPS\\out\\TMAPS2-0-%03i.rnc", index);
			fopen_s(&fptw, filename, "wb");
			fwrite(&contentTMAPSdat[shift], size, 1, fptw);
			fclose(fptw);

			int decompsize = *(Bit32u*)&contentTMAPSdat[shift + 6];
			sub_5C3D0_file_decompress(&contentTMAPSdat[shift], buffer);



			Bit8u* index2 = 10 * index + TMAPS00TAB_BEGIN_BUFFER;
			x_DWORD_F66F0[index] = (int)sub_71E70(x_DWORD_E9C28_str, (unsigned __int16)(4 * ((unsigned int)(*(x_DWORD*)index2 + 13) >> 2)), index);
			Bit8u* subpointer = *(Bit8u * *)x_DWORD_F66F0[index];
			subpointer = (Bit8u*)malloc(unpacksize);
			memcpy(subpointer, buffer, unpacksize);
			*/

			FILE* fptw2_prev;
			char filenamedata[300];
			sprintf_s(filenamedata, "%s\\%s%03i-00.data", outputPath, tmapsstr, index);
			fopen_s(&fptw2_prev, filenamedata, "rb");
			fread(prevbuffer, width * height + 6, 1, fptw2_prev);
			fclose(fptw2_prev);
			filesToDelete.push_back(filenamedata);

			bool same = true;
			for (int kk = 0; kk < width * height + 6; kk++)
			{
				if (buffer[kk] != prevbuffer[kk])
					same = false;
			}
			if (same)
			{
				index++; continue;
			}

			//FILE* fptw2_prev;
			//char filenamedata[300];
			sprintf_s(filenamedata, "%s\\%s%03i-%02i.data", outputPath, tmapsstr, index, mainindex);
			fopen_s(&fptw2_prev, filenamedata, "rb");
			if (fptw2_prev == NULL)
			{
				index++; continue;
			}
			fread(prevbuffer, width * height + 6, 1, fptw2_prev);
			fclose(fptw2_prev);
			filesToDelete.push_back(filenamedata);

			same = true;
			for (int kk = 0; kk < width * height + 6; kk++)
			{
				if (buffer[kk] != prevbuffer[kk])
					same = false;
			}
			if (same)
			{
				index++; continue;
			}

			FILE* fptw2;
			sprintf_s(filenamedata, "%s\\%s%03i-%02i.data", outputPath, tmapsstr, index, mainindex + 1);
			fopen_s(&fptw2, filenamedata, "wb");
			fwrite(buffer, width * height + 6, 1, fptw2);
			fclose(fptw2);
			filesToDelete.push_back(filenamedata);

			if (imageType == ImageType::bmp)
			{
				sprintf_s(outname, "%s\\%s%03i-00.bmp", outputPath, tmapsstr, index);
				BitmapIO::WriteRGBAImageBufferAsImageBMP(outname, width, height, pallettebuffer, buffer + 6, colourMultiplier);
			}

			if (imageType == ImageType::png)
			{
				if (isOther(other_folder, index))
					sprintf_s(outname, "%s\\%s%03i-%02i-other", outputPath, tmapsstr, index, mainindex + 1);
				else
					sprintf_s(outname, "%s\\%s%03i-%02i", outputPath, tmapsstr, index, mainindex + 1);

				sprintf_s(title, "%s%03i", tmapsstr, index);
				BitmapIO::WritePosistructToPng(pallettebuffer, buffer + 6, width, height, outname, title, padding, colourMultiplier);
			}

			if (imageType == ImageType::pnga)
			{
				sprintf_s(outname, "%s\\%s%03i-alpha-%02i", outputPath, tmapsstr, index, mainindex + 1);
				sprintf_s(title, "%s%03i", tmapsstr, index);
				BitmapIO::WritePosistructToPng(pallettebuffer, buffer + 6, width, height, outname, title, padding, colourMultiplier);
				sprintf_s(outname, "%s\\%s%03i-alpha-00", outputPath, tmapsstr, index);
				BitmapIO::WritePosistructToAlphaPng(pallettebuffer, buffer + 6, width, height, outname, title, padding);
			}

			if (caveSprites && index < 452)
				indextab += 10;
			else if(!caveSprites)
				indextab += 10;

			index++;
		}
	}

	//Clean up
	if (imageType != ImageType::data && filesToDelete.size() > 0)
	{
		printf("Cleaning up .data files...\n");

		for(int i = 0; i < filesToDelete.size(); i++)
		{
			if (std::filesystem::exists(filesToDelete[i]))
			{
				std::filesystem::remove(filesToDelete[i]);
			}
		}

		printf("Completed cleanup\n");
	}
	printf("Extraction Completed\n");
	return 0;
}