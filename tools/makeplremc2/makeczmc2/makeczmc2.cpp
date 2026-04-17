// makeczmc2.cpp : Tento soubor obsahuje funkci main. Provádění programu se tam zahajuje a ukončuje.
//

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
	//char blue = 0xe4;
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

	/*
	for (int j = 1;j < height / 2;j++)
		for (int i = 3;i < width - 1;i++)
			if ((i < width / 2) && ((i - 1)*inheight < (j)*inwidth))
				buffer[j*width + i] = blue;
	for (int j = height / 2;j < height - 1;j++)
		for (int i = 3;i < width - 1;i++)
			if ((i < width / 2) && ((i-1)*inheight < (height-j)*inwidth))
				buffer[j*width + i] = blue;
	*/

	fwrite(buffer, 1, height*width, fptw);
};

char* removeDiakritics(char* text) {
	char* withdia = (char*)"áÁčČďĎéÉěĚíÍňŇóÓřŘšŠťŤúÚůŮýÝžŽ";
	char* withoutdia = (char*)"aAcCdDeEeEiInNoOrRsStTuUuUyYzZ";
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

void writetext(char* text) {
	text = removeDiakritics(text);
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
	fopen_s(&fptr, "d2-pl.csv", "rb");
	fseek(fptr, 0L, SEEK_END);
	long sz = ftell(fptr);
	fseek(fptr, 0L, SEEK_SET);
	content = (char *)malloc(sz * sizeof(char *));
	fread(content, sz, 1, fptr);
	fclose(fptr);

	char text[4096];
	/*sprintf_s(text1,"%s", "text1");
	sprintf_s(text2, "%s", "text2");
	sprintf_s(text3, "%s", "áÁčČďĎéÉěĚíÍňŇóÓřŘšŠťŤúÚůŮýÝžŽ");*/
	
	fopen_s(&fptw, "L8.TXT", "wb");

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

