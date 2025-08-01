#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include "field_helpers.h"

#pragma pack(push)
#pragma pack(1)
struct CTMesEntry
{
	char alt_name[11] = { 0 };
	double bii = 0;
	uint16_t classi = 0;
	char constell[4] = { 0 };
	double dec = 0;
	char dimension[6] = { 0 };
	double lii = 0;
	char name[6] = { 0 };
	char notes[50] = { 0 };
	char object_type[2] = { 0 };
	double ra = 0;
	float vmag = 0;
	char vmag_uncert[2] = { 0 };
};
#pragma pack(pop)

inline std::vector<CTMesEntry> readTDat_Mes(std::string path)
{
	std::cout << "reading messier catalog..." << std::endl;

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

	std::vector<CTMesEntry> entries;
	size_t complete_percent = 0;
	while (true)
	{
		file.getline((char*)line.c_str(), 512, '\n');
		if (strcmp(line.c_str(), "<END>") == 0 || strlen(line.c_str()) == 0)
			break;

		CTMesEntry entry;
		size_t field_start = 0;
		size_t field_end = line.find('|', field_start);
		FIELD_STR(entry.alt_name);
		FIELD_FLOAT8(entry.bii);
		FIELD_INT(entry.classi);
		FIELD_STR(entry.constell);
		FIELD_FLOAT8(entry.dec);
		FIELD_STR(entry.dimension);
		FIELD_FLOAT8(entry.lii);
		FIELD_STR(entry.name);
		FIELD_STR(entry.notes);
		FIELD_STR(entry.object_type);
		FIELD_FLOAT8(entry.ra);
		FIELD_FLOAT4(entry.vmag);
		FIELD_STR(entry.vmag_uncert);

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
