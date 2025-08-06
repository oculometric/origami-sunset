#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include "../inc/constellation.h"
#include "henry_draper.h"
#include "hipparcos.h"
#include "messier.h"
#include "ngc2000.h"
#include "constel.h"
#include "flamsteed.h"
#include "bsc.h"

using namespace std;

#pragma pack(1)
// FIXME: check the merging algorithms!!
std::vector<CTCelestial> mergeHD_HIP(const std::vector<CTCelestial>& hd, const std::vector<CTCelestial>& hip)
{
	std::vector<CTCelestial> merged;

	std::vector<bool> consumed;
	std::vector<CTCelestial> hip_sorted;
	hip_sorted.insert(hip_sorted.begin(), hip.begin(), hip.end());
	sort(hip_sorted.begin(), hip_sorted.end(), HIPComparator());
	std::vector<CTCelestial> hd_sorted;
	hd_sorted.insert(hd_sorted.begin(), hd.begin(), hd.end());
	sort(hd_sorted.begin(), hd_sorted.end(), HIPComparator());

	size_t remaining = hip.size();
	consumed.insert(consumed.begin(), remaining, false);

	size_t classi_mismatches = 0;
	size_t spect_mismatches = 0;

	size_t complete_percent = 0;
	size_t hd_complete = 0;
	size_t hip_complete = 0;
	size_t hip_search_start = 0;
	std::cout << "    " << "0% complete";
	for (CTCelestial hd_s : hd_sorted)
	{
		if (remaining != 0)
		{
			for (int i = hip_search_start; i < hip_sorted.size(); i++)
			{
				if (consumed[i])
					continue;
				if (hip_sorted[i].henry_draper_number > hd_s.henry_draper_number)
				{
					hip_search_start = i;
					break;
				}
				if (hip_sorted[i].henry_draper_number == hd_s.henry_draper_number)
				{
					hd_s.names.insert(hd_s.names.end(), hip_sorted[i].names.begin(), hip_sorted[i].names.end());
					hd_s.ra_dec = hip_sorted[i].ra_dec;
					hd_s.gal_lat_long = hip_sorted[i].gal_lat_long;
					hd_s.hipparcos_number = hip_sorted[i].hipparcos_number;
					if (hd_s.classification != hip_sorted[i].classification)
						classi_mismatches++;
					if (hd_s.spectral_type != hip_sorted[i].spectral_type)
						spect_mismatches++;
					if (hip_sorted[i].classification != 9999)
					{
						hd_s.classification = hip_sorted[i].classification;
						hd_s.spectral_type = hip_sorted[i].spectral_type;
					}
					// FIXME: magnitudes
					hd_s.proper_motion = hip_sorted[i].proper_motion;
					
					consumed[i] = true;
					hip_complete++;
					remaining--;
					hip_search_start = i + 1;
					break;
				}
			}
		}
		hd_complete++;
		merged.push_back(hd_s);

		size_t tmp = complete_percent;
		complete_percent = (hd_complete + hip_complete) * 100 / (hd.size() + hip.size());
		if (complete_percent > tmp)
			std::cout << "\r    " << complete_percent << "% complete";
	}
	if (remaining > 0)
	{
		for (int i = 0; i < hip_sorted.size(); i++)
		{
			if (remaining == 0)
				break;
			if (consumed[i])
				continue;
			merged.push_back(hip_sorted[i]);
			consumed[i] = true;
			hip_complete++;
			remaining--;

			size_t tmp = complete_percent;
			complete_percent = (hd_complete + hip_complete) * 100 / (hd.size() + hip.size());
			if (complete_percent > tmp)
				std::cout << "\r    " << complete_percent << "% complete";
		}
	}
	std::cout << std::endl;

	return merged;
}

std::vector<CTCelestial> mergeFlam_BSC(const std::vector<CTCelestial>& flam, const std::vector<CTCelestial>& bsc)
{
	std::vector<CTCelestial> merged;

	std::vector<bool> consumed;
	std::vector<CTCelestial> flam_sorted;
	flam_sorted.insert(flam_sorted.begin(), flam.begin(), flam.end());
	sort(flam_sorted.begin(), flam_sorted.end(), BSCComparator());
	std::vector<CTCelestial> bsc_sorted;
	bsc_sorted.insert(bsc_sorted.begin(), bsc.begin(), bsc.end());
	sort(bsc_sorted.begin(), bsc_sorted.end(), BSCComparator());

	size_t remaining = bsc.size();
	consumed.insert(consumed.begin(), remaining, false);

	size_t hr_mismatches = 0;
	size_t flamsteed_mismatches = 0;

	size_t complete_percent = 0;
	size_t flam_complete = 0;
	size_t bsc_complete = 0;
	size_t bsc_search_start = 0;
	std::cout << "    " << "0% complete";
	for (CTCelestial flam_s : flam_sorted)
	{
		if (remaining != 0)
		{
			for (int i = bsc_search_start; i < bsc_sorted.size(); i++)
			{
				if (consumed[i])
					continue;
				if (bsc_sorted[i].henry_draper_number > flam_s.henry_draper_number)
				{
					bsc_search_start = i;
					break;
				}
				if (bsc_sorted[i].henry_draper_number == flam_s.henry_draper_number)
				{
					flam_s.names.insert(flam_s.names.end(), bsc_sorted[i].names.begin(), bsc_sorted[i].names.end());
					flam_s.ra_dec = bsc_sorted[i].ra_dec;
					flam_s.gal_lat_long = bsc_sorted[i].gal_lat_long;
					flam_s.sao_number = bsc_sorted[i].sao_number;
					if (flam_s.harvard_revised_number != bsc_sorted[i].harvard_revised_number
						&& flam_s.harvard_revised_number != 0)
						hr_mismatches++;
					flam_s.harvard_revised_number = bsc_sorted[i].harvard_revised_number;
					if (bsc_sorted[i].constellation[0] != ' '
						&& bsc_sorted[i].constellation[0] != 0)
						memcpy(flam_s.constellation, bsc_sorted[i].constellation, 3);
					flam_s.is_multiple = bsc_sorted[i].is_multiple;
					flam_s.is_variable = bsc_sorted[i].is_variable;
					flam_s.visual_magnitude = bsc_sorted[i].visual_magnitude;
					flam_s.spectral_type = bsc_sorted[i].spectral_type;
					flam_s.proper_motion = bsc_sorted[i].proper_motion;
					flam_s.bayer_identifier = bsc_sorted[i].bayer_identifier;
					if (strncmp(flam_s.flamsteed_constel, bsc_sorted[i].flamsteed_constel, 3) != 0
						&& bsc_sorted[i].flamsteed_constel[0] != 0
						&& flam_s.flamsteed_constel[0] != 0)
						flamsteed_mismatches++;
					else if (flam_s.flamsteed_number != bsc_sorted[i].flamsteed_number
						&& bsc_sorted[i].flamsteed_number != 0
						&& flam_s.flamsteed_number != 0)
						flamsteed_mismatches++;

					consumed[i] = true;
					bsc_complete++;
					remaining--;
					bsc_search_start = i + 1;
					break;
				}
			}
		}
		flam_complete++;
		if (flam_s.bayer_identifier.find_first_not_of(' ') != std::string::npos)
			flam_s.names.push_back(flam_s.bayer_identifier);
		merged.push_back(flam_s);

		size_t tmp = complete_percent;
		complete_percent = (flam_complete + bsc_complete) * 100 / (flam.size() + bsc.size());
		if (complete_percent > tmp)
			std::cout << "\r    " << complete_percent << "% complete";
	}
	if (remaining > 0)
	{
		for (int i = 0; i < bsc_sorted.size(); i++)
		{
			if (remaining == 0)
				break;
			if (consumed[i])
				continue;
			auto copy = bsc_sorted[i];
			if (copy.bayer_identifier.find_first_not_of(' ') != std::string::npos)
				copy.names.push_back(copy.bayer_identifier);
			merged.push_back(copy);
			consumed[i] = true;
			bsc_complete++;
			remaining--;

			size_t tmp = complete_percent;
			complete_percent = (flam_complete + bsc_complete) * 100 / (flam.size() + bsc.size());
			if (complete_percent > tmp)
				std::cout << "\r    " << complete_percent << "% complete";
		}
	}
	std::cout << std::endl;

	return merged;
}

int main()
{
	ofstream generated_file("../inc/constellation_data.generated.h");

	generated_file << "// this file was generated by the origami-sunset star catalog analyis tool." << endl;

	auto hd_data = loadHD("../catalog");
	auto hip_data = loadHIP("../catalog");
	auto flam_data = loadFlam("../catalog");
	auto bsc_data = loadBSC("../catalog");
	//// TODO: read SAO
	auto mes_data = loadMes("../catalog");
	auto ngc_data = loadNGC("../catalog");

	auto hd_hip_merged = mergeHD_HIP(hd_data, hip_data);
	auto flam_bsc_merged = mergeFlam_BSC(flam_data, bsc_data);
	//// TODO: merge Flam with BSC
	//// TODO: merge HD/HIP with Flam/BSC
	//// TODO: properly decode object types in NGC/Mes
	//// TODO: merge NGC with Mes
	//// TODO: merge HD/HIP/BSC/Flam with NGC/Mes
	//// TODO: assign constellations for objects which do not have them
	//// TODO: generate output file
	//
	//auto constellations = loadConstellations("../catalog");

	generated_file.close();

	return 0;
}
