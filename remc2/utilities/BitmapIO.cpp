#include "BitmapIO.h"

#include <stdio.h>

const int BitmapIO::TRUECOLOR_BYTES_PER_PIXEL = 3;
const int BitmapIO::RGBA_BYTES_PER_PIXEL = 4;
const int BitmapIO::BITMAP_FILE_HEADER_SIZE = 14;
const int BitmapIO::BITMAP_INFO_HEADER_SIZE = 40;
const int BitmapIO::TRANSPARENT_COLOUR = 0;

unsigned char* BitmapIO::CreateBitBitmapFileHeader(int fileHeaderSize, int infoHeaderSize, int height, int stride) {
	int fileSize = fileHeaderSize + infoHeaderSize + (stride * height);

	static unsigned char fileHeader[] = {
		0,0,     /// signature
		0,0,0,0, /// image file size in bytes
		0,0,0,0, /// reserved
		0,0,0,0 /// start of pixel array
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

unsigned char* BitmapIO::CreateBitBitmapInfoHeader(int infoHeaderSize, int width, int height, int bytesPerPixel) {
	static unsigned char infoHeader[] = {
		0,0,0,0, /// header size
		0,0,0,0, /// image width
		0,0,0,0, /// image height
		0,0,     /// number of color planes
		0,0,     /// bits per pixel
		0,0,0,0, /// compression
		0,0,0,0, /// image size
		0,0,0,0, /// horizontal resolution
		0,0,0,0, /// vertical resolution
		0,0,0,0, /// colors in color table
		0,0,0,0 /// important color count
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

void BitmapIO::WriteImageBufferAsImageBMP(const char* path, int width, int height, uint8_t* ptrPalette, uint8_t* ptrBuffer)
{
	int widthInBytes = (width * BitmapIO::TRUECOLOR_BYTES_PER_PIXEL);

	unsigned char padding[3] = { 255, 255, 255 };
	int paddingSize = (4 - (widthInBytes) % 4) % 4;

	int stride = (widthInBytes)+paddingSize;

	FILE* imageFile = fopen(path, "wb");

	unsigned char* fileHeader = CreateBitBitmapFileHeader(BitmapIO::BITMAP_FILE_HEADER_SIZE, BitmapIO::BITMAP_INFO_HEADER_SIZE, height, stride);
	fwrite(fileHeader, 1, BitmapIO::BITMAP_FILE_HEADER_SIZE, imageFile);

	unsigned char* infoHeader = CreateBitBitmapInfoHeader(BitmapIO::BITMAP_INFO_HEADER_SIZE, width, height, BitmapIO::TRUECOLOR_BYTES_PER_PIXEL);
	fwrite(infoHeader, 1, BitmapIO::BITMAP_INFO_HEADER_SIZE, imageFile);

	for (int i = (height - 1); i > -1; i--) {

		uint8_t* truColorBuffer = new uint8_t[widthInBytes];

		for (int x = (width - 1); x > -1; x--)
		{
			uint8_t colorPaletteIdx = *(ptrBuffer + ((i * width) + x));

			int truColorIdx = (x * 3);
			truColorBuffer[truColorIdx] = (unsigned char)(4 * ptrPalette[(colorPaletteIdx * 3) + 2]); //Blue
			truColorBuffer[truColorIdx + 1] = (unsigned char)(4 * ptrPalette[(colorPaletteIdx * 3) + 1]); //Green
			truColorBuffer[truColorIdx + 2] = (unsigned char)(4 * ptrPalette[(colorPaletteIdx * 3)]); //Red
		}

		fwrite(truColorBuffer, BitmapIO::TRUECOLOR_BYTES_PER_PIXEL, width, imageFile);
		if (paddingSize > 0)
		{
			fwrite(padding, 1, paddingSize, imageFile);
		}

		delete[] truColorBuffer;
	}

	fclose(imageFile);
}

void BitmapIO::WriteRGBAImageBufferAsImageBMP(const char* path, int width, int height, uint8_t* ptrPalette, uint8_t* ptrBuffer)
{
	int widthInBytes = (width * BitmapIO::RGBA_BYTES_PER_PIXEL);
	int paddingSize = (4 - (widthInBytes) % 4) % 4;
	int stride = widthInBytes + paddingSize;
	unsigned char padding[3] = { 0, 0, 0 };

	int totalSizeBytes = height * widthInBytes;
	uint8_t* truColorBuffer = new uint8_t[totalSizeBytes];

	for (int i = 0; i < width * height; i++)
	{
		int truColorIdx = (i * BitmapIO::RGBA_BYTES_PER_PIXEL);

		truColorBuffer[truColorIdx + 0] = ptrPalette[ptrBuffer[(width * height) - 1 - i] * 3 + 2];
		truColorBuffer[truColorIdx + 1] = ptrPalette[ptrBuffer[(width * height) - 1 - i] * 3 + 1];
		truColorBuffer[truColorIdx + 2] = ptrPalette[ptrBuffer[(width * height) - 1 - i] * 3];

		if (ptrBuffer[(width * height) - 1 - i] != 255)
			truColorBuffer[truColorIdx + 3] = 255;
	}

	FILE* imageFile = fopen(path, "wb");

	unsigned char* fileHeader = CreateBitBitmapFileHeader(BitmapIO::BITMAP_FILE_HEADER_SIZE, BitmapIO::BITMAP_INFO_HEADER_SIZE, height, stride);
	fwrite(fileHeader, 1, BitmapIO::BITMAP_FILE_HEADER_SIZE, imageFile);

	unsigned char* infoHeader = CreateBitBitmapInfoHeader(BitmapIO::BITMAP_INFO_HEADER_SIZE, width, height, BitmapIO::RGBA_BYTES_PER_PIXEL);
	fwrite(infoHeader, 1, BitmapIO::BITMAP_INFO_HEADER_SIZE, imageFile);

	int i;
	for (i = 0; i < height; i++) {
		fwrite(truColorBuffer + (i * widthInBytes), BitmapIO::RGBA_BYTES_PER_PIXEL, width, imageFile);
		fwrite(padding, 1, paddingSize, imageFile);
	}
	delete[] truColorBuffer;

	fclose(imageFile);
}

void BitmapIO::WritePaletteAsImageBMP(const char* path, int numColors, uint8_t* ptrPalette)
{
	int widthInBytes = (numColors * BitmapIO::TRUECOLOR_BYTES_PER_PIXEL);

	unsigned char padding[3] = { 0, 0, 0 };
	int paddingSize = (4 - (widthInBytes) % 4) % 4;

	int stride = widthInBytes + paddingSize;

	FILE* imageFile = fopen(path, "wb");

	unsigned char* fileHeader = CreateBitBitmapFileHeader(BitmapIO::BITMAP_FILE_HEADER_SIZE, BitmapIO::BITMAP_INFO_HEADER_SIZE, 100, stride);
	fwrite(fileHeader, 1, BitmapIO::BITMAP_FILE_HEADER_SIZE, imageFile);

	unsigned char* infoHeader = CreateBitBitmapInfoHeader(BitmapIO::BITMAP_INFO_HEADER_SIZE, numColors, 100, BitmapIO::TRUECOLOR_BYTES_PER_PIXEL);
	fwrite(infoHeader, 1, BitmapIO::BITMAP_INFO_HEADER_SIZE, imageFile);

	uint8_t* truColorBuffer = new uint8_t[widthInBytes];

	for (int x = 0; x < widthInBytes; x++)
	{
		truColorBuffer[x] = (unsigned char)(4 * ptrPalette[x]);
	}

	for (int i = 0; i < 100; i++) {

		fwrite(truColorBuffer, BitmapIO::TRUECOLOR_BYTES_PER_PIXEL, numColors, imageFile);
		if (paddingSize > 0)
		{
			fwrite(padding, 1, paddingSize, imageFile);
		}
	}

	delete[] truColorBuffer;

	fclose(imageFile);
}

#ifdef _DEBUG
void BitmapIO::WritePosistructToPng(uint8_t* ptrPalette, uint8_t* ptrBuffer, int width, int height, char* filename, char* title, int padding, double multiplier)
{
	BitmapIO::WritePosistructToPng(ptrPalette, ptrBuffer, width, height, filename, title, padding, multiplier, 0, 255, 0);
}

void BitmapIO::WritePosistructToPng(uint8_t* ptrPalette, uint8_t* ptrBuffer, int width, int height, char* filename, char* title, int padding, double multiplier, uint8_t transColR, uint8_t transColG, uint8_t transColB)
{
	char textbuffer[512];
	std::vector<uint8_t> buffer2((width + (2 * padding)) * (height + (2 * padding)) * 4);

	for (int y = 0; y < height + (2 * padding); y++)
	{
		for (int x = 0; x < width + (2 * padding); x++)
		{
			int x2 = x - padding;
			int y2 = y - padding;

			if ((x < padding) || (y < padding) || (x >= width + padding) || (y >= height + padding)) {
				buffer2[(y * (width + (2 * padding)) + x) * 4 + 0] = transColR;
				buffer2[(y * (width + (2 * padding)) + x) * 4 + 1] = transColG;
				buffer2[(y * (width + (2 * padding)) + x) * 4 + 2] = transColB;
				buffer2[(y * (width + (2 * padding)) + x) * 4 + 3] = 255;
			}
			else
			{
				bool isWhite = false;
				buffer2[(y * (width + (2 * padding)) + x) * 4 + 0] = BitmapIO::MultiplyValue(ptrPalette[ptrBuffer[y2 * width + x2] * 3], multiplier);
				buffer2[(y * (width + (2 * padding)) + x) * 4 + 1] = BitmapIO::MultiplyValue(ptrPalette[ptrBuffer[y2 * width + x2] * 3 + 1], multiplier);
				buffer2[(y * (width + (2 * padding)) + x) * 4 + 2] = BitmapIO::MultiplyValue(ptrPalette[ptrBuffer[y2 * width + x2] * 3 + 2], multiplier);
				buffer2[(y * (width + (2 * padding)) + x) * 4 + 3] = 255;
				if (ptrBuffer[(y2 * width + x2)] == TRANSPARENT_COLOUR)
				{
					isWhite = true;
					if (y2 > 0)
						if ((ptrBuffer[((y2 - 1) * width + x2)] != TRANSPARENT_COLOUR) && (ptrBuffer[y2 * width + x2] == TRANSPARENT_COLOUR))
						{
							buffer2[(y * (width + (2 * padding)) + x) * 4 + 0] = BitmapIO::MultiplyValue(ptrPalette[ptrBuffer[((y2 - 1) * width + x2)] * 3], multiplier);
							buffer2[(y * (width + (2 * padding)) + x) * 4 + 1] = BitmapIO::MultiplyValue(ptrPalette[ptrBuffer[((y2 - 1) * width + x2)] * 3 + 1], multiplier);
							buffer2[(y * (width + (2 * padding)) + x) * 4 + 2] = BitmapIO::MultiplyValue(ptrPalette[ptrBuffer[((y2 - 1) * width + x2)] * 3 + 2], multiplier);
							isWhite = false;
						}
					if (y2 < height - 1)
						if ((ptrBuffer[((y2 + 1) * width + x2)] != TRANSPARENT_COLOUR) && (ptrBuffer[y2 * width + x2] == TRANSPARENT_COLOUR))
						{
							buffer2[(y * (width + (2 * padding)) + x) * 4 + 0] = BitmapIO::MultiplyValue(ptrPalette[ptrBuffer[((y2 + 1) * width + x2)] * 3], multiplier);
							buffer2[(y * (width + (2 * padding)) + x) * 4 + 1] = BitmapIO::MultiplyValue(ptrPalette[ptrBuffer[((y2 + 1) * width + x2)] * 3 + 1], multiplier);
							buffer2[(y * (width + (2 * padding)) + x) * 4 + 2] = BitmapIO::MultiplyValue(ptrPalette[ptrBuffer[((y2 + 1) * width + x2)] * 3 + 2], multiplier);
							isWhite = false;
						}
					if (x2 > 0)
						if ((ptrBuffer[(y2 * width + (x2 - 1))] != TRANSPARENT_COLOUR) && (ptrBuffer[y2 * width + x2] == TRANSPARENT_COLOUR))
						{
							buffer2[(y * (width + (2 * padding)) + x) * 4 + 0] = BitmapIO::MultiplyValue(ptrPalette[ptrBuffer[(y2 * width + (x2 - 1))] * 3], multiplier);
							buffer2[(y * (width + (2 * padding)) + x) * 4 + 1] = BitmapIO::MultiplyValue(ptrPalette[ptrBuffer[(y2 * width + (x2 - 1))] * 3 + 1], multiplier);
							buffer2[(y * (width + (2 * padding)) + x) * 4 + 2] = BitmapIO::MultiplyValue(ptrPalette[ptrBuffer[(y2 * width + (x2 - 1))] * 3 + 2], multiplier);
							isWhite = false;
						}
					if (x2 < width - 1)
						if ((ptrBuffer[(y2 * width + (x2 + 1))] != TRANSPARENT_COLOUR) && (ptrBuffer[y2 * width + x2] == TRANSPARENT_COLOUR))
						{
							buffer2[(y * (width + (2 * padding)) + x) * 4 + 0] = BitmapIO::MultiplyValue(ptrPalette[ptrBuffer[(y2 * width + (x2 + 1))] * 3], multiplier);
							buffer2[(y * (width + (2 * padding)) + x) * 4 + 1] = BitmapIO::MultiplyValue(ptrPalette[ptrBuffer[(y2 * width + (x2 + 1))] * 3 + 1], multiplier);
							buffer2[(y * (width + (2 * padding)) + x) * 4 + 2] = BitmapIO::MultiplyValue(ptrPalette[ptrBuffer[(y2 * width + (x2 + 1))] * 3 + 2], multiplier);
							isWhite = false;
						}
				}
				if (isWhite)
				{
					buffer2[(y * (width + (2 * padding)) + x) * 4 + 0] = transColR;
					buffer2[(y * (width + (2 * padding)) + x) * 4 + 1] = transColG;
					buffer2[(y * (width + (2 * padding)) + x) * 4 + 2] = transColB;
					buffer2[(y * (width + (2 * padding)) + x) * 4 + 3] = 0;
				}
			}
		}
	}
	sprintf(textbuffer, "%s.png", filename);
	WriteImagePNG(textbuffer, width + (2 * padding), height + (2 * padding), buffer2.data(), title);
}

void BitmapIO::WriteImagePNG(const char* filename, int width, int height, uint8_t* buffer, char* title)
{
	int code = 0;
	FILE* fp = NULL;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytep row = NULL;

	// Open file for writing (binary mode)
	fp = fopen(filename, "wb");
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

void BitmapIO::setRGBA(png_byte* ptr, uint8_t* val)
{
	ptr[0] = val[0];
	ptr[1] = val[1];
	ptr[2] = val[2];
	ptr[3] = val[3];
}
#endif

uint8_t BitmapIO::MultiplyValue(uint8_t orig, double multiplier)
{
	int value = orig * multiplier;
	if (value < 0)
		value = 0;

	if (value > 255)
		value = 255;
	return (uint8_t)value;
}