#include "pch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>

char buffer[1000000];
FILE *fptw;
FILE *fptr;
char val;


void writeflag() {
	char red = 0xe0;
	char white = 0xe2;
	char blue = 0xe4;
	/*int width = 87;
	int height = 51;*/

	int width = 90;
	int height = 53;
	int inwidth = width - 4;
	int inheight = height - 2;

	for (int j = 0;j < height;j++)	
	{
		for (int i = 0;i < width;i++)
			buffer[j*width + i] = white;
		buffer[j*width + 0] = 0x00;
		buffer[j*width + 1] = 0x58;
	}
	for (int j = height/2;j < height-1;j++)
	{
		for (int i = 3;i < width-1;i++)
			buffer[j*width + i] = red;
	}

	for (int j = 1;j < height / 2;j++)
		for (int i = 3;i < width - 1;i++)
			if ((i < width / 2) && ((i - 1)*inheight < (j)*inwidth))
				buffer[j*width + i] = blue;
	for (int j = height / 2;j < height - 1;j++)
		for (int i = 3;i < width - 1;i++)
			if ((i < width / 2) && ((i-1)*inheight < (height-j)*inwidth))
				buffer[j*width + i] = blue;

	fwrite(buffer, 1, height*width, fptw);
};

char* removeDiakritics(char* text) {
	char* withdia =    (char*)"áÁèÈïÏéÉìÌíÍòÒóÓøØšŠúÚùÙıİ¹¥æÆêÊ³£ñÑœŒŸ¿¯";
	char* withoutdia = (char*)"aAcCdDeEeEiInNoOrRsStTuUuUyYzZaAcCeElLnNsSzZzZ";
	int len = strlen(text);
	int lendia = strlen(withdia);
	for (int i = 0;i < len;i++)
		for (int j = 0;j < lendia;j++)
		{
			if (text[i] == withdia[j])
				text[i] = withoutdia[j];
		}
	return text;
}

char* encodeDiakritics(char* text) {
	// Tabulka: znak -> kod
	struct { unsigned char ch; unsigned char code; } table[] = {
		{(unsigned char)'é', 219},
		{(unsigned char)'ù', 169},
		{(unsigned char)'æ', 193},
		{(unsigned char)'³', 208},
		{(unsigned char)'', 194},
		{(unsigned char)'Æ', 195},
		{(unsigned char)'É', 219},
		{(unsigned char)'Œ', 196},
		{(unsigned char)'œ', 197},
		{(unsigned char)'', 170},
		{(unsigned char)'', 171},
		{(unsigned char)'£', 198},
		{(unsigned char)'è', 172},
		{(unsigned char)'á', 214},
		{(unsigned char)'í', 216},
		{(unsigned char)'ó', 220},
		{(unsigned char)'ú', 218},
		{(unsigned char)'¥', 199},
		{(unsigned char)'¹', 200},
		{(unsigned char)'', 175},
		{(unsigned char)'Ê', 201},
		{(unsigned char)'ê', 202},
		{(unsigned char)'Ÿ', 203},
		{(unsigned char)'È', 176},
		{(unsigned char)'Á', 213},
		{(unsigned char)'Ì', 178},
		{(unsigned char)'¯', 204},
		{(unsigned char)'', 180},
		{(unsigned char)'¿', 205},
		{(unsigned char)'Ï', 182},
		{(unsigned char)'ï', 183},
		{(unsigned char)'Ò', 184},
		{(unsigned char)'Í', 217},
		{(unsigned char)'ì', 209},
		{(unsigned char)'Ù', 186},
		{(unsigned char)'Ó', 220},
		{(unsigned char)'Ñ', 210},
		{(unsigned char)'ñ', 211},
		{(unsigned char)'ò', 187},
		{(unsigned char)'Š', 188},
		{(unsigned char)'š', 206},
		{(unsigned char)'Ú', 212},
		{(unsigned char)'ı', 189},
		{(unsigned char)'İ', 190},
		{(unsigned char)'Ø', 207},
		{(unsigned char)'ø', 192},
		{(unsigned char)'ø', 192},
		{(unsigned char)'„', 34},
		{(unsigned char)'”', 34},
	};
	int tableSize = sizeof(table) / sizeof(table[0]);
	int len = strlen(text);

	for (int i = 0; i < len; i++) {
		for (int j = 0; j < tableSize; j++) {
			if ((unsigned char)text[i] == table[j].ch) {
				text[i] = (char)table[j].code;
				break;
			}
		}
	}
	return text;
}

void writetext(char* text) {
	text = encodeDiakritics(text);
	int len = strlen(text);
	for (int i = 0;i < len;i++)
		buffer[i]=text[i];
	fwrite(buffer, 1, len, fptw);
	val = 0x00;
	fwrite(&val, 1, 1, fptw);
};
int contindex = 0;
char *content;
void readtext(char* text) {
	char readchar;
	int count=0;
	do {
		readchar = content[contindex++];
		text[count++]= readchar;
	} while (readchar!='#');
	text[count-1] = 0;
};

int main(int argc, char *argv[])
{
	fopen_s(&fptr, "d2-cz.csv", "rb");
	fseek(fptr, 0L, SEEK_END);
	long sz = ftell(fptr);
	fseek(fptr, 0L, SEEK_SET);
	content = (char *)malloc(sz * sizeof(char *));
	fread(content, sz, 1, fptr);
	fclose(fptr);

	char text[4096];
	
	fopen_s(&fptw, "L7.TXT", "wb");

	val = 0x02;
	fwrite(&val, 1, 1, fptw);

	writeflag();

	val = 0x00;
	fwrite(&val, 1, 1, fptw);
	val = 0x7f;
	fwrite(&val, 1, 1, fptw);
	val = 0x00;
	fwrite(&val, 1, 1, fptw);
	fwrite(&val, 1, 1, fptw);
	fwrite(&val, 1, 1, fptw);
	fwrite(&val, 1, 1, fptw);
	fwrite(&val, 1, 1, fptw);
	fwrite(&val, 1, 1, fptw);
	val = 0x02;
	fwrite(&val, 1, 1, fptw);
	val = 0x00;
	fwrite(&val, 1, 1, fptw);
	fwrite(&val, 1, 1, fptw);
	fwrite(&val, 1, 1, fptw);
	val = 0x58;
	fwrite(&val, 1, 1, fptw);
	val = 0x35;
	fwrite(&val, 1, 1, fptw);
	for (int count = 0;count < 188;count++)
	{
		readtext(text);
	}
	for (int count = 0;count < 0x1d7;count++)
	{
		readtext(text);
		readtext(text);
		readtext(text);
		writetext(text);
	}
	

	val = 0x00;
	fwrite(&val, 1, 1, fptw);
	//fclose(fptr);
	fclose(fptw);
	return 0;

}

