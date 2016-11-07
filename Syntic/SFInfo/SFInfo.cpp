// SFInfo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

#include "../Shared/SF.h"

void PrintUsage()
{

}

void PrintAppInfo()
{

}

sftHeader_s ReadHeader(FILE * f)
{
	char magic[sizeof(SFMagic)];
	unsigned char version;
	unsigned short fileCount;
	fread(magic, sizeof(SFMagic), sizeof(char), f);
	//fseek(f, 6, SEEK_SET);
	fread(&version, 1, sizeof(unsigned char), f);
	fread(&fileCount, 1, sizeof(unsigned short), f);
	//fileCount = _byteswap_ushort(fileCount);
	std::cout << "Magic: " << magic << std::endl << "Version: " << (int)version << std::endl << "File Count: " << fileCount << std::endl;
	sftHeader_s header;
	memset(&header, 0, sizeof(header));
	header.fileCount = fileCount;
	strcpy_s(header.magic, SFMagic);
	header.version = version;
	return header;
}

sftFileEntry_s ReadFileEntry(FILE * f)
{
	/*
	fwrite((void *)ft.name, sizeof(ft.name), 1, f);
	fwrite((void *)&ft.type, 1, sizeof(unsigned char), f);
	fwrite((void *)&ft.offset, 1, sizeof(unsigned long), f);
	*/
	char name[256];
	unsigned char type;
	unsigned long offset;
	sftFileEntry_s fe;
	memset(&fe, 0, sizeof(fe));
	fread((void *)name, 256, 1, f);
	fread((void *)&type, 1, sizeof(unsigned char), f);
	fread((void *)&offset, 1, sizeof(unsigned long), f);
	//std::cout << "Name: " << name << std::endl << "Type: " << (int)type << std::endl << "Offset: " << offset << std::endl;
	strcpy_s(fe.name, name);
	fe.type = type;
	fe.offset = offset;
	return fe;
}

sfFile_s ReadCompressedFile(FILE * f)
{
	/*
				fwrite((void *)&f.uncompressedSize, 1, sizeof(f.uncompressedSize), out);
			fwrite((void *)&f.compressedSize, 1, sizeof(f.compressedSize), out);
			fwrite((void *)f.compressedFile, f.compressedSize, 1, out);
			*/
	sfFile_s sff;
	memset(&sff, 0, sizeof(sff));
	fread((void *)&sff.uncompressedSize, 1, sizeof(sff.uncompressedSize), f);
	fread((void *)&sff.compressedSize, 1, sizeof(sff.compressedSize), f);
	std::cout << "Uncompressed size: " << sff.uncompressedSize << std::endl << "Compressed size: " << sff.compressedSize << std::endl;
	sff.compressedFile = new unsigned char[sff.compressedSize];
	fread((void *)sff.compressedFile, sff.compressedSize, sizeof(char), f);
	return sff;
}

int main(int argc, char * argv[])
{
	if (argc < 2)
	{
		PrintUsage();
		return 0;
	}
	for (int i = 0; i < argc - 1; i++)
	{
		FILE * f;
		FILE * ft;
		char sf[512];
		char sft[512];
		sprintf_s(sf, "%s.sf", argv[i+1]);
		sprintf_s(sft, "%s.sft", argv[i+1]);
		fopen_s(&ft, sft, "rb");
		if (ft == NULL)
		{
			std::cout << "ERROR WHILE OPENING STREAM FOR " << sft << std::endl;
			continue;
		}
		sftHeader_s header = ReadHeader(ft);
		sft_s sftFile;
		memset(&sftFile, 0, sizeof(sftFile));
		sftFile.header = header;
		sftFile.entries = new sftFileEntry_s[header.fileCount];
		for (int p = 0; p < header.fileCount; p++)
		{
			//std::cout << std::endl << "File Entry #" << p << std::endl;
			sftFileEntry_s o = ReadFileEntry(ft);
			sftFile.entries[p] = o;
		}
		fclose(ft);
		ft = NULL;
		// WE FINISHED READING THE FILE ENTRY TABLE AND WE'VE LOADED IT TO MEMORY, WE CAN CLOSE THE STREAM NOW
		fopen_s(&f, sf, "rb");

		sfFile_s *sfFiles = new sfFile_s[sftFile.header.fileCount];
		for (int ip = 0; ip < sftFile.header.fileCount; ip++)
		{
			std::cout << std::endl << "File #" << ip << ":" << std::endl;
			std::cout << "Name: " << sftFile.entries[ip].name << std::endl << "Type: " << (int)sftFile.entries[ip].type << std::endl << "Offset: " << sftFile.entries[ip].offset << std::endl;
			sfFiles[ip] = ReadCompressedFile(f);
		}

		fclose(f);
		f = NULL;
	}
    return 0;
}

