#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include "field_helpers.h"
#include "star.h"

#pragma pack(push)
#pragma pack(1)
struct CTBSCEntry
{
	uint16_t hr_num = 0;
	char name[10] = { 0 };
	char dm_id[11] = { 0 };
	uint32_t hd_num = 0;
	uint32_t sao_num = 0;
	uint32_t fk5_num = 0;
	char ir_source = 0;
	char ir_source_ref = 0;
	char multiple_code = 0;
	char ads_designation[5] = { 0 };
	char ads_comp[2] = { 0 };
	char variable_id[9] = { 0 };
	uint8_t ra_hours_1900 = 0;
	uint8_t ra_minutes_1900 = 0;
	float ra_seconds_1900 = 0;
	bool dec_negative_1900 = false;
	uint8_t dec_degrees_1900 = 0;
	uint8_t dec_minutes_1900 = 0;
	uint8_t dec_seconds_1900 = 0;
	uint8_t ra_hours_2000 = 0;
	uint8_t ra_minutes_2000 = 0;
	float ra_seconds_2000 = 0;
	bool dec_negative_2000 = false;
	uint8_t dec_degrees_2000 = 0;
	uint8_t dec_minutes_2000 = 0;
	uint8_t dec_seconds_2000 = 0;
	float gal_long = 0;
	float gal_lat = 0;
	float visual_mag = 0;
	char vmag_code = 0;
	char vmag_uncert_flag = 0;
	float bv = 0;
	char bv_uncert_flag = 0;
	float ub = 0;
	char ub_uncert_flag = 0;
	float ri = 0;
	char ri_code = 0;
	char spectral_type[20] = { 0 };
	char spectral_type_code = 0;
	float pm_ra = 0;
	float pm_dec = 0;
	char parallax_flag = 0;
	float parallax = 0;
	int32_t radial_velocity = 0;
	char radial_velocity_note[4] = { 0 };
	char rotational_velocity_limit[2] = { 0 };
	int32_t rotational_velocity = 0;
	char rotational_velocity_uncert = 0;
	float mag_diff = 0;
	float separation = 0;
	char comp_id[4] = { 0 };
	uint8_t comp_count = 0;
	char has_note = 0;
};
#pragma pack(pop)

inline std::vector<CTBSCEntry> readDat_BSC(std::string path)
{
	std::cout << "reading BSC catalog... " << std::endl;

	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cout << "failed!" << std::endl;
		return { };
	}

	std::string line;
	line.resize(256, '\0');
	size_t total_rows = 0;
	std::string last_modified;

	std::cout << "    reading entries..." << std::endl;

	std::vector<CTBSCEntry> entries;
	while (true)
	{
		memset((char*)line.c_str(), 0, line.size());
		file.getline((char*)line.c_str(), line.size(), '\n');
		if (strlen(line.c_str()) == 0)
			break;

		CTBSCEntry entry;
		size_t field_start = 0;
		DATA_INT(entry.hr_num, 4);
		DATA_STR(entry.name, 10);
		DATA_STR(entry.dm_id, 11);
		DATA_INT(entry.hd_num, 6);
		DATA_INT(entry.sao_num, 6);
		DATA_INT(entry.fk5_num, 4);
		DATA_CHAR(entry.ir_source);
		DATA_CHAR(entry.ir_source_ref);
		DATA_CHAR(entry.multiple_code);
		DATA_STR(entry.ads_designation, 5);
		DATA_STR(entry.ads_comp, 2);
		DATA_STR(entry.variable_id, 9);
		DATA_INT(entry.ra_hours_1900, 2);
		DATA_INT(entry.ra_minutes_1900, 2);
		DATA_FLOAT(entry.ra_seconds_1900, 4);
		DATA_EXISTS(entry.dec_negative_1900, '-');
		DATA_INT(entry.dec_degrees_1900, 2);
		DATA_INT(entry.dec_minutes_1900, 2);
		DATA_INT(entry.dec_seconds_1900, 2);
		DATA_INT(entry.ra_hours_2000, 2);
		DATA_INT(entry.ra_minutes_2000, 2);
		DATA_FLOAT(entry.ra_seconds_2000, 4);
		DATA_EXISTS(entry.dec_negative_2000, '-');
		DATA_INT(entry.dec_degrees_2000, 2);
		DATA_INT(entry.dec_minutes_2000, 2);
		DATA_INT(entry.dec_seconds_2000, 2);
		DATA_FLOAT(entry.gal_long, 6);
		DATA_FLOAT(entry.gal_lat, 6);
		DATA_FLOAT(entry.visual_mag, 5);
		DATA_CHAR(entry.vmag_code);
		DATA_CHAR(entry.vmag_uncert_flag);
		DATA_FLOAT(entry.bv, 5);
		DATA_CHAR(entry.bv_uncert_flag);
		DATA_FLOAT(entry.ub, 5);
		DATA_CHAR(entry.ub_uncert_flag);
		DATA_FLOAT(entry.ri, 5);
		DATA_CHAR(entry.ri_code);
		DATA_STR(entry.spectral_type, 20);
		DATA_CHAR(entry.spectral_type_code);
		DATA_FLOAT(entry.pm_ra, 6);
		DATA_FLOAT(entry.pm_dec, 6);
		DATA_CHAR(entry.parallax_flag);
		DATA_FLOAT(entry.parallax, 5);
		DATA_INT(entry.radial_velocity, 4);
		DATA_STR(entry.radial_velocity_note, 4);
		DATA_STR(entry.rotational_velocity_limit, 2);
		DATA_INT(entry.rotational_velocity, 3);
		DATA_CHAR(entry.rotational_velocity_uncert);
		DATA_FLOAT(entry.mag_diff, 4);
		DATA_FLOAT(entry.separation, 6);
		DATA_STR(entry.comp_id, 4);
		DATA_INT(entry.comp_count, 2);
		DATA_EXISTS(entry.has_note, '*');

		entries.push_back(entry);
	}

	file.close();

	std::cout << "done (read " << entries.size() << " entries)." << std::endl;
	return entries;
}

inline std::vector<CTCelestial> loadBSC(std::string base_path)
{
	std::cout << "loading BSC catalog..." << std::endl;

	std::vector<CTBSCEntry> data;
	readCache(data, base_path + "/cache/bsc.dat");
	if (data.empty())
	{
		data = readDat_BSC(base_path + "/bsc.dat");
		writeCache(data, base_path + "/cache/bsc.dat");
		std::cout << "wrote cache file to " << base_path << "/cache/bsc.dat" << std::endl;
	}
	else
		std::cout << "cache found, reading that instead." << std::endl;

	std::cout << "generating standardised data..." << std::endl;

	std::vector<CTCelestial> standardised_data;
	size_t search_start = 0;
	for (const auto& datum : data)
	{
		CTCelestial c;
		if (datum.hd_num == 0)
			continue;
		c.harvard_revised_number = datum.hr_num;
		c.henry_draper_number = datum.hd_num;
		c.sao_number = datum.sao_num;
		{
			std::string complex_name = datum.name;
			complex_name.resize(10, ' ');
			c.flamsteed_number = 0;
			try { c.flamsteed_number = std::stoi(complex_name.substr(0, 3)); }
			catch (std::exception _) { }
			if (c.flamsteed_number != 0)
				memcpy(c.flamsteed_constel, complex_name.substr(7, 3).c_str(), 3);
			if (complex_name.find_first_not_of(' ') != std::string::npos)
			{
				c.bayer_identifier = complex_name.substr(3, 3);
				if (*(c.bayer_identifier.end() - 1) != ' ')
				{
					c.bayer_identifier += ' ';
					c.bayer_identifier += complex_name.substr(6, 1);
					if (*(c.bayer_identifier.end() - 1) == ' ')
						c.bayer_identifier.pop_back();
					else
						c.bayer_identifier += ' ';
					c.bayer_identifier += complex_name.substr(7, 3);
				}
				else
					c.bayer_identifier = "";
			}
		}
		c.is_multiple = datum.multiple_code > ' ';
		{
			std::string var_id = datum.variable_id;
			var_id.resize(9, ' ');
			c.is_variable = (var_id.find_first_not_of(' ') != std::string::npos);
		}
		{
			double ra = ((double)datum.ra_hours_2000 * 15.0)
				+ ((double)datum.ra_minutes_2000 * 0.25)
				+ ((double)datum.ra_seconds_2000 * 0.004166666667);
			double dec = ((double)datum.dec_degrees_2000)
				+ ((double)datum.dec_minutes_2000 * 0.0166666666667)
				+ ((double)datum.dec_seconds_2000 * 0.0002777777778);
			if (datum.dec_negative_2000)
				dec = -dec;
			c.ra_dec = { ra, dec };
		}
		c.gal_lat_long = { datum.gal_lat, datum.gal_long };
		c.visual_magnitude = datum.visual_mag;
		c.proper_motion = { datum.pm_ra, datum.pm_dec };
		c.spectral_type = datum.spectral_type;
		c.spectral_type.resize(20);
		memcpy(c.constellation, c.flamsteed_constel, 3);
		for (int i = 0; i < 3; i++) c.constellation[i] = toupper(c.constellation[i]);
		c.object_type = STAR;

		standardised_data.push_back(c);
	}
	std::cout << "done." << std::endl;

	return standardised_data;
}

struct BSCComparator
{
	inline bool operator() (const CTCelestial& a, const CTCelestial& b)
	{
		if (a.henry_draper_number == 0)
			return false;
		if (b.henry_draper_number == 0)
			return true;
		return (a.henry_draper_number < b.henry_draper_number);
	}
};