#pragma once
#ifndef DataFileIO_H
#define DataFileIO_H

#include <cstdio>
#include <cstdint>
#include "../portability/port_filesystem.h"
#include "../portability/port_outputs.h"
#include "DataFileRNC.h"

class DataFileIO
{
private:

public:
	static int ReadFileAndDecompress(const char* path, uint8_t** data); //Reads the file path passed in into "data" array of 1 byte elements
	static FILE* CreateOrOpenFile(const char* pathname, int __pmode);
	static FILE* CreateFile(const char* path, uint32_t flags);
	static FILE* Open(const char* path, int pmode, uint32_t flags);
	static int32_t Close(FILE* file);
	static int32_t Seek(FILE* file, x_DWORD position, char type);
	static size_t Read(FILE* file, uint8_t* data, uint32_t length);
	static long FileLengthBytes(FILE* file);
	static uint8_t read_byte(uint8_t* buf, size_t* offset);
	static uint16_t read_word_be(uint8_t* buf, size_t* offset);
	static uint32_t read_dword_be(uint8_t* buf, size_t* offset);
	static void write_buf(uint8_t* dest, size_t* offset, uint8_t* source, int size);
	static void read_buf(uint8_t* dest, uint8_t* source, size_t* offset, int size);	

private:
	// Disallow creating an instance of this object
	DataFileIO();
};

#endif