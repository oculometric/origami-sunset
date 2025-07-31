#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include "field_helpers.h"

#pragma pack(push)
#pragma pack(1)
struct CTHDEntry
{
	char name[20];
	double ra;
	double dec;
	float bii;
	float lii;
	uint16_t classi;
	char spectral_type[4];
	float vmag;
	float pgmag;
	uint32_t hd_number;
	double dircos1;
	double dircos2;
	double dircos3;
	uint16_t variability_flag;
	uint16_t multiplicity_flag;
};
#pragma pack(pop)

inline std::vector<CTHDEntry> readTDat_HD(std::string path)
{
	std::cout << "reading HD catalog... " << std::endl;
	
	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cout << "failed!" << std::endl;
		return { };
	}

	std::string line;
	line.resize(512, '\0');
	size_t total_rows = 0;
	std::string last_modified;
	if (!findTDatDataStart(file, line, total_rows, last_modified))
		return { };

	std::cout << "    detected " << total_rows << " total rows" << std::endl;
	std::cout << "    last modified " << last_modified << std::endl;
	
	std::cout << "    reading entries..." << std::endl;

	std::vector<CTHDEntry> entries;
	size_t complete_percent = 0;
	while (true)
	{
		file.getline((char*)line.c_str(), 512, '\n');
		if (strcmp(line.c_str(), "<END>") == 0 || strlen(line.c_str()) == 0)
			break;

		CTHDEntry entry;
		size_t field_start = 0;
		size_t field_end = line.find('|', field_start);
		FIELD_FLOAT4(entry.bii);
		FIELD_INT(entry.classi);
		FIELD_FLOAT8(entry.dec);
		FIELD_FLOAT8(entry.dircos1);
		FIELD_FLOAT8(entry.dircos2);
		FIELD_FLOAT8(entry.dircos3);
		FIELD_LONG(entry.hd_number);
		FIELD_FLOAT4(entry.lii);
		FIELD_INT(entry.multiplicity_flag);
		FIELD_STR(entry.name);
		FIELD_FLOAT4(entry.pgmag);
		FIELD_FLOAT8(entry.ra);
		FIELD_STR(entry.spectral_type);
		FIELD_INT(entry.variability_flag);
		FIELD_FLOAT4(entry.vmag);

		entries.push_back(entry);
		size_t tmp = complete_percent;
		complete_percent = entries.size() * 100 / total_rows;
		if (complete_percent > tmp)
			std::cout << "\r    " << complete_percent << "% complete";
	}
	std::cout << std::endl;

	file.close();

	std::cout << "done (read " << entries.size() << " entries)." << std::endl;
	return entries;
}
