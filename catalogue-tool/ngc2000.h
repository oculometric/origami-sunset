#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include "field_helpers.h"
#include "star.h"

#pragma pack(push)
#pragma pack(1)
struct CTNGCEntry
{
	char name[9] = { 0 };
	char source_type[3] = { 0 };
	double ra = 0;
	double dec = 0;
	double lii = 0;
	double bii = 0;
	char ref_revision[2] = { 0 };
	char constellation[3] = { 0 };
	char limit_ang_diameter = 0;
	float ang_diameter = 0;
	float app_mag = 0;
	char app_mag_flag = 0;
	char description[53] = { 0 };
	uint16_t classi = 0;
};
#pragma pack(pop)

inline std::vector<CTNGCEntry> readTDat_NGC(std::string path)
{
	std::cout << "reading NGC2000 catalog..." << std::endl;

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

	std::vector<CTNGCEntry> entries;
	size_t complete_percent = 0;
	while (true)
	{
		memset((char*)line.c_str(), 0, line.size());
		file.getline((char*)line.c_str(), line.size(), '\n');
		if (strcmp(line.c_str(), "<END>") == 0 || strlen(line.c_str()) == 0)
			break;

		CTNGCEntry entry;
		size_t field_start = 0;
		size_t field_end = line.find('|', field_start);
		FIELD_STR(entry.name);
		FIELD_STR(entry.source_type);
		FIELD_FLOAT8(entry.ra);
		FIELD_FLOAT8(entry.dec);
		FIELD_FLOAT8(entry.lii);
		FIELD_FLOAT8(entry.bii);
		FIELD_STR(entry.ref_revision);
		FIELD_STR(entry.constellation);
		FIELD_CHAR(entry.limit_ang_diameter);
		FIELD_FLOAT4(entry.ang_diameter);
		FIELD_FLOAT4(entry.app_mag);
		FIELD_CHAR(entry.app_mag_flag);
		FIELD_STR(entry.description);
		FIELD_INT(entry.classi);

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

inline std::vector<CTCelestial> loadNGC(std::string base_path)
{
	std::cout << "loading NGC2000 catalog..." << std::endl;

	std::vector<CTNGCEntry> data;
	readCache(data, base_path + "/cache/ngc.dat");
	if (data.empty())
	{
		data = readTDat_NGC(base_path + "/heasarc_ngc2000.tdat");
		writeCache(data, base_path + "/cache/ngc.dat");
		std::cout << "wrote cache file to " << base_path << "/cache/ngc.dat" << std::endl;
	}
	else
		std::cout << "cache found, reading that instead." << std::endl;

	std::cout << "generating standardised data..." << std::endl;
	std::vector<CTCelestial> standardised_data;
	for (const auto& datum : data)
	{
		CTCelestial c;
		std::string name = datum.name;
		name.resize(9);
		c.names.push_back(name);
		if (name.substr(0, 2) == "IC")
			c.ic_number = stoi(name.substr(3));
		else if (name.substr(0, 3) == "NGC")
			c.ngc2000_number = stoi(name.substr(4));
		c.ra_dec = { datum.ra, datum.dec };
		c.gal_lat_long = { datum.bii, datum.lii };
		c.classification = datum.classi;
		c.visual_magnitude = datum.app_mag;
		memcpy(c.constellation, datum.constellation, 3);
		for (int i = 0; i < 3; i++) c.constellation[i] = toupper(c.constellation[i]);
		std::string desc = datum.description;
		desc.resize(53);
		c.descriptions.push_back(desc);
		// TODO: object type based on datum.source_type
		//c.object_type = STAR;
		standardised_data.push_back(c);
	}
	std::cout << "done." << std::endl;

	return standardised_data;
}
