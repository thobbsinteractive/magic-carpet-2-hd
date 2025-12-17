#include "DataFileIO.h"

#ifndef _countof
#define _countof(arr) sizeof(arr) / sizeof(arr[0])
//dirty
#endif

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
		result = (int)DataFileRNC::Decompress(*data, *data);
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

FILE* DataFileIO::CreateOrOpenFile(const char* pathname, int __pmode)
{
	FILE* file; // ST10_4


	if (__pmode == 0x222)
	{
		file = CreateFile(pathname, 0x1c0);
		//x_setmode(v2, 0x200);
		Close(file);
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

void DataFileIO::write_buf(uint8_t* dest, size_t* offset, uint8_t* source, int size)
{
	memmove(&dest[*offset], source, size);
	*offset += size;
}

void DataFileIO::read_buf(uint8_t* dest, uint8_t* source, size_t* offset, int size)
{
	memmove(dest, &source[*offset], size);
	*offset += size;
}