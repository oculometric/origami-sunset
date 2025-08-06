#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include "field_helpers.h"
#include "star.h"

#pragma pack(push)
#pragma pack(1)
struct CTFlamEntry
{
	uint16_t num = 0;
	char constel[3] = { 0 };
	uint8_t flamsteed_num = 0;
	bool in_lalande = false;
	char ptolemy_constel[3] = { 0 };
	bool ptolemy_informis = false;
	uint8_t ptolemy_num = 0;
	char tycho_constel[3] = { 0 };
	uint8_t tycho_num = 0;
	char hevelius_constel[3] = { 0 };
	uint8_t hevelius_num = 0;
	char bayer_constel[3] = { 0 };
	char bayer_letter[3] = { 0 };
	uint8_t bayer_index = 0;
	bool colon_flag = 0;
	uint8_t ra_deg_1690 = 0;
	uint8_t ra_amin_1690 = 0;
	uint8_t ra_asec_1690 = 0;
	uint8_t pd_deg_1690 = 0;
	uint8_t pd_amin_1690 = 0;
	uint8_t pd_asec_1690 = 0;
	uint8_t longitude_zodiac_sign = 0;
	uint8_t long_deg_1690 = 0;
	uint8_t long_amin_1690 = 0;
	uint8_t long_asec_1690 = 0;
	bool latitude_positive = false;
	uint8_t lat_deg_1690 = 0;
	uint8_t lat_amin_1690 = 0;
	uint8_t lat_asec_1690 = 0;
	uint8_t var_ra_min = 0;
	bool var_ra_positive = false;
	uint8_t var_ra_sec = 0;
	uint8_t var_pd_min = 0;
	bool var_pd_positive = false;
	uint8_t var_pd_sec = 0;
	bool magnitude_shared = false;
	char magnitude[3] = { 0 };
};

struct CTFlamIDEntry
{
	uint16_t num = 0;
	char constel[3] = { 0 };
	uint8_t flamsteed_num = 0;
	bool unidentified = false;
	uint16_t hr_num = 0;
	uint32_t hd_num = 0;
};
#pragma pack(pop)

struct FlamComparator
{
	inline bool operator() (const CTFlamEntry& a, const CTFlamEntry& b)
	{
		if (a.num == 0)
			return false;
		if (b.num == 0)
			return true;
		return (a.num < b.num);
	}
};

struct FlamIDComparator
{
	inline bool operator() (const CTFlamIDEntry& a, const CTFlamIDEntry& b)
	{
		if (a.num == 0)
			return false;
		if (b.num == 0)
			return true;
		return (a.num < b.num);
	}
};

inline std::vector<CTFlamEntry> readDat_Flam(std::string path)
{
	std::cout << "    reading flamsteed catalog... " << std::endl;

	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cout << "    failed!" << std::endl;
		return { };
	}

	std::string line;
	line.resize(512, '\0');
	size_t total_rows = 0;
	std::string last_modified;

	std::cout << "        reading entries..." << std::endl;

	std::vector<CTFlamEntry> entries;
	while (true)
	{
		memset((char*)line.c_str(), 0, line.size());
		file.getline((char*)line.c_str(), line.size(), '\n');
		if (strlen(line.c_str()) == 0)
			break;

		CTFlamEntry entry;
		size_t field_start = 0;
		DATA_INT(entry.num, 4);
		DATA_NONE(1);
		DATA_STR(entry.constel, 3);
		DATA_NONE(1);
		DATA_INT(entry.flamsteed_num, 3);
		DATA_EXISTS(entry.in_lalande, '*');
		DATA_STR(entry.ptolemy_constel, 3);
		DATA_EXISTS(entry.ptolemy_informis, '~');
		DATA_INT(entry.ptolemy_num, 2);
		DATA_NONE(1);
		DATA_STR(entry.tycho_constel, 3);
		DATA_NONE(1);
		DATA_INT(entry.tycho_num, 2);
		DATA_NONE(1);
		DATA_STR(entry.hevelius_constel, 3);
		DATA_NONE(1);
		DATA_INT(entry.hevelius_num, 2);
		DATA_NONE(1);
		DATA_STR(entry.bayer_constel, 3);
		DATA_NONE(1);
		DATA_STR(entry.bayer_letter, 3);
		DATA_NONE(1);
		DATA_INT(entry.bayer_index, 1);
		DATA_EXISTS(entry.colon_flag, ':');
		DATA_INT(entry.ra_deg_1690, 3);
		DATA_NONE(1);
		DATA_INT(entry.ra_amin_1690, 2);
		DATA_NONE(1);
		DATA_INT(entry.ra_asec_1690, 2);
		DATA_NONE(1);
		DATA_INT(entry.pd_deg_1690, 3);
		DATA_NONE(1);
		DATA_INT(entry.pd_amin_1690, 2);
		DATA_NONE(1);
		DATA_INT(entry.pd_asec_1690, 2);
		DATA_NONE(1);
		DATA_INT(entry.longitude_zodiac_sign, 2);
		DATA_NONE(1);
		DATA_INT(entry.long_deg_1690, 2);
		DATA_NONE(1);
		DATA_INT(entry.long_amin_1690, 2);
		DATA_NONE(1);
		DATA_INT(entry.long_asec_1690, 2);
		DATA_NONE(1);
		DATA_EXISTS(entry.latitude_positive, '+');
		DATA_INT(entry.lat_deg_1690, 2);
		DATA_NONE(1);
		DATA_INT(entry.lat_amin_1690, 2);
		DATA_NONE(1);
		DATA_INT(entry.lat_asec_1690, 2);
		DATA_NONE(1);
		DATA_INT(entry.var_ra_min, 3);
		DATA_EXISTS(entry.var_ra_positive, '+');
		DATA_INT(entry.var_ra_sec, 2);
		DATA_NONE(1);
		DATA_INT(entry.var_pd_min, 2);
		DATA_EXISTS(entry.var_pd_positive, '+');
		DATA_INT(entry.var_pd_sec, 2);
		DATA_NONE(1);
		DATA_EXISTS(entry.magnitude_shared, '*');
		DATA_STR(entry.magnitude, 3);

		entries.push_back(entry);
	}

	file.close();

	std::cout << "    done (read " << entries.size() << " entries)." << std::endl;
	return entries;
}

inline std::vector<CTFlamIDEntry> readDat_FlamIDs(std::string path)
{
	std::cout << "    reading flamsteed ID catalog... " << std::endl;

	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cout << "    failed!" << std::endl;
		return { };
	}

	std::string line;
	line.resize(64, '\0');
	size_t total_rows = 0;
	std::string last_modified;

	std::cout << "        reading entries..." << std::endl;

	std::vector<CTFlamIDEntry> entries;
	while (true)
	{
		memset((char*)line.c_str(), 0, line.size());
		file.getline((char*)line.c_str(), line.size(), '\n');
		if (strlen(line.c_str()) == 0)
			break;

		CTFlamIDEntry entry;
		size_t field_start = 0;
		DATA_INT(entry.num, 4);
		DATA_NONE(1);
		DATA_STR(entry.constel, 3);
		DATA_NONE(1);
		DATA_INT(entry.flamsteed_num, 3);
		DATA_EXISTS(entry.unidentified, '*');
		DATA_INT(entry.hr_num, 4);
		DATA_NONE(1);
		DATA_INT(entry.hd_num, 6);

		entries.push_back(entry);
	}

	file.close();

	std::cout << "    done (read " << entries.size() << " entries)." << std::endl;
	return entries;
}

inline std::vector<CTCelestial> loadFlam(std::string base_path)
{
	std::cout << "loading flamsteed catalog and ID table..." << std::endl;

	std::vector<CTFlamEntry> data;
	readCache(data, base_path + "/cache/flam.dat");
	if (data.empty())
	{
		data = readDat_Flam(base_path + "/flamsteed_f.dat");
		writeCache(data, base_path + "/cache/flam.dat");
		std::cout << "    wrote cache file to " << base_path << "/cache/flam.dat" << std::endl;
	}
	else
		std::cout << "    cache found, reading that instead." << std::endl;
	
	std::vector<CTFlamIDEntry> data_id;
	readCache(data_id, base_path + "/cache/flam_id.dat");
	if (data_id.empty())
	{
		data_id = readDat_FlamIDs(base_path + "/flamsteed_ids.dat");
		writeCache(data_id, base_path + "/cache/flam_id.dat");
		std::cout << "    wrote cache file to " << base_path << "/cache/flam_id.dat" << std::endl;
	}
	else
		std::cout << "    cache found, reading that instead." << std::endl;

	std::cout << "    generating standardised data and cross-identifying..." << std::endl;
	sort(data.begin(), data.end(), FlamComparator());
	sort(data_id.begin(), data_id.end(), FlamIDComparator());

	std::vector<CTCelestial> standardised_data;
	size_t search_start = 0;
	for (const auto& datum : data)
	{
		CTCelestial c;
		if (datum.flamsteed_num != 0)
		{
			std::string corrected_name;
			corrected_name += std::to_string((int)datum.flamsteed_num);
			corrected_name += ' ';
			corrected_name.insert(corrected_name.size(), datum.constel, 3);
			c.names.push_back(corrected_name);
		}
		c.flamsteed_number = datum.flamsteed_num;
		memcpy(c.flamsteed_constel, datum.constel, 3);
		if (datum.bayer_constel[0] != 0 && datum.bayer_constel[0] != ' ')
		{
			std::string ident;
			ident.insert(0, datum.bayer_letter, 3);
			if (datum.bayer_index != 0)
			{
				ident += ' ';
				ident += std::to_string((int)datum.bayer_index);
			}
			ident += ' ';
			ident.insert(ident.size(), datum.bayer_constel, 3);
			c.bayer_identifier = ident;
		}
		for (size_t i = search_start; i < data_id.size(); i++)
		{
			if (data_id[i].num > datum.num)
				break;
			if (data_id[i].num == datum.num)
			{
				if (!data_id[i].unidentified)
				{
					c.henry_draper_number = data_id[i].hd_num;
					c.harvard_revised_number = data_id[i].hr_num;
				}
				if (search_start == i)
					search_start = i + 1;
				break;
			}
		}
		memcpy(c.constellation, datum.constel, 3);
		for (int i = 0; i < 3; i++) c.constellation[i] = toupper(c.constellation[i]);
		c.object_type = STAR;

		standardised_data.push_back(c);
	}
	std::cout << "done." << std::endl;

	return standardised_data;
}
