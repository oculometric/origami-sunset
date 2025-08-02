#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include "field_helpers.h"

#pragma pack(push)
#pragma pack(1)
struct CTHIPEntry
{
	char name[11] = { 0 };
	double ra = 0;
	double dec = 0;
	double lii = 0;
	double bii = 0;
	uint32_t hip_number = 0;
	char prox_10asec = 0;
	double vmag = 0;
	uint8_t var_flag = 0;
	char vmag_source = 0;
	char ra_deg[12] = { 0 };
	char dec_deg[12] = { 0 };
	char astrom_ref_dbl = 0;
	double parallax = 0;
	double pm_ra = 0;
	double pm_dec = 0;
	double ra_error = 0;
	double dec_error = 0;
	double parallax_error = 0;
	double pm_ra_error = 0;
	double pm_dec_error = 0;
	double crl_dec_ra = 0;
	double crl_plx_ra = 0;
	double crl_plx_dec = 0;
	double crl_pmra_ra = 0;
	double crl_pmra_dec = 0;
	double crl_pmra_plx = 0;
	double crl_pmdec_ra = 0;
	double crl_pmdec_dec = 0;
	double crl_pmdec_plx = 0;
	double crl_pmdec_pmra = 0;
	uint8_t reject_percent = 0;
	double quality_fit = 0;
	double bt_mag = 0;
	double bt_mag_error = 0;
	double vt_mag = 0;
	double vt_mag_error = 0;
	char bt_mag_ref_dbl = 0;
	double bv_color = 0;
	double bv_color_error = 0;
	char bv_mag_source = 0;
	double vi_color = 0;
	double vi_color_error = 0;
	char vi_color_source = 0;
	char mag_ref_dbl = 0;
	double hip_mag = 0;
	double hip_mag_error = 0;
	double scat_hip_mag = 0;
	uint16_t n_obs_hip_mag = 0;
	char hip_mag_ref_dbl = 0;
	double hip_mag_max = 0;
	double hip_mag_min = 0;
	double var_period = 0;
	char hip_var_type = 0;
	char var_data_annex = 0;
	char var_curv_annex = 0;
	char ccdm_id[10] = { 0 };
	char ccdm_history = 0;
	uint8_t ccdm_n_entries = 0;
	uint8_t ccdm_n_comp = 0;
	char dbl_mult_annex = 0;
	char astrom_mult_source = 0;
	char dbl_soln_qual = 0;
	char dbl_ref_id[2] = { 0 };
	uint16_t dbl_theta = 0;
	double dbl_rho = 0;
	double rho_error = 0;
	double diff_hip_mag = 0;
	double dhip_mag_error = 0;
	char survey_star = 0;
	char id_chart = 0;
	char notes = 0;
	uint32_t hd_id = 0;
	char bd_id[10] = { 0 };
	char cod_id[10] = { 0 };
	char cpd_id[10] = { 0 };
	double vi_color_reduct = 0;
	char spect_type[12] = { 0 };
	char spect_type_source = 0;
	uint16_t classi = 0;
};
#pragma pack(pop)

inline std::vector<CTHIPEntry> readTDat_HIP(std::string path)
{
	std::cout << "reading hipparcos catalog..." << std::endl;

	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cout << "failed!" << std::endl;
		return { };
	}

	std::string line;
	line.resize(1024, '\0');
	size_t total_rows = 0;
	std::string last_modified;
	if (!findTDatDataStart(file, line, total_rows, last_modified))
		return { };

	std::cout << "    detected " << total_rows << " total rows" << std::endl;
	std::cout << "    last modified " << last_modified << std::endl;

	std::cout << "    reading entries..." << std::endl;

	std::vector<CTHIPEntry> entries;
	size_t complete_percent = 0;
	while (true)
	{
		file.getline((char*)line.c_str(), 1024, '\n');
		if (strcmp(line.c_str(), "<END>") == 0 || strlen(line.c_str()) == 0)
			break;

		CTHIPEntry entry;
		size_t field_start = 0;
		size_t field_end = line.find('|', field_start);
		FIELD_STR(entry.name);
		FIELD_FLOAT8(entry.ra);
		FIELD_FLOAT8(entry.dec);
		FIELD_FLOAT8(entry.lii);
		FIELD_FLOAT8(entry.bii);
		FIELD_INT(entry.hip_number);
		FIELD_CHAR(entry.prox_10asec);
		FIELD_FLOAT8(entry.vmag);
		FIELD_INT(entry.var_flag);
		FIELD_CHAR(entry.vmag_source);
		FIELD_STR(entry.ra_deg);
		FIELD_STR(entry.dec_deg);
		FIELD_CHAR(entry.astrom_ref_dbl);
		FIELD_FLOAT8(entry.parallax);
		FIELD_FLOAT8(entry.pm_ra);
		FIELD_FLOAT8(entry.pm_dec);
		FIELD_FLOAT8(entry.ra_error);
		FIELD_FLOAT8(entry.dec_error);
		FIELD_FLOAT8(entry.parallax_error);
		FIELD_FLOAT8(entry.pm_ra_error);
		FIELD_FLOAT8(entry.pm_dec_error);
		FIELD_FLOAT8(entry.crl_dec_ra);
		FIELD_FLOAT8(entry.crl_plx_ra);
		FIELD_FLOAT8(entry.crl_plx_dec);
		FIELD_FLOAT8(entry.crl_pmra_ra);
		FIELD_FLOAT8(entry.crl_pmra_dec);
		FIELD_FLOAT8(entry.crl_pmra_plx);
		FIELD_FLOAT8(entry.crl_pmdec_ra);
		FIELD_FLOAT8(entry.crl_pmdec_dec);
		FIELD_FLOAT8(entry.crl_pmdec_plx);
		FIELD_FLOAT8(entry.crl_pmdec_pmra);
		FIELD_INT(entry.reject_percent);
		FIELD_FLOAT8(entry.quality_fit);
		FIELD_FLOAT8(entry.bt_mag);
		FIELD_FLOAT8(entry.bt_mag_error);
		FIELD_FLOAT8(entry.vt_mag);
		FIELD_FLOAT8(entry.vt_mag_error);
		FIELD_CHAR(entry.bt_mag_ref_dbl);
		FIELD_FLOAT8(entry.bv_color);
		FIELD_FLOAT8(entry.bv_color_error);
		FIELD_CHAR(entry.bv_mag_source);
		FIELD_FLOAT8(entry.vi_color);
		FIELD_FLOAT8(entry.vi_color_error);
		FIELD_CHAR(entry.vi_color_source);
		FIELD_CHAR(entry.mag_ref_dbl);
		FIELD_FLOAT8(entry.hip_mag);
		FIELD_FLOAT8(entry.hip_mag_error);
		FIELD_FLOAT8(entry.scat_hip_mag);
		FIELD_INT(entry.n_obs_hip_mag);
		FIELD_CHAR(entry.hip_mag_ref_dbl);
		FIELD_FLOAT8(entry.hip_mag_max);
		FIELD_FLOAT8(entry.hip_mag_min);
		FIELD_FLOAT8(entry.var_period);
		FIELD_CHAR(entry.hip_var_type);
		FIELD_CHAR(entry.var_data_annex);
		FIELD_CHAR(entry.var_curv_annex);
		FIELD_STR(entry.ccdm_id);
		FIELD_CHAR(entry.ccdm_history);
		FIELD_INT(entry.ccdm_n_entries);
		FIELD_INT(entry.ccdm_n_comp);
		FIELD_CHAR(entry.dbl_mult_annex);
		FIELD_CHAR(entry.astrom_mult_source);
		FIELD_CHAR(entry.dbl_soln_qual);
		FIELD_STR(entry.dbl_ref_id);
		FIELD_INT(entry.dbl_theta);
		FIELD_FLOAT8(entry.dbl_rho);
		FIELD_FLOAT8(entry.rho_error);
		FIELD_FLOAT8(entry.diff_hip_mag);
		FIELD_FLOAT8(entry.dhip_mag_error);
		FIELD_CHAR(entry.survey_star);
		FIELD_CHAR(entry.id_chart);
		FIELD_CHAR(entry.notes);
		FIELD_LONG(entry.hd_id);
		FIELD_STR(entry.bd_id);
		FIELD_STR(entry.cod_id);
		FIELD_STR(entry.cpd_id);
		FIELD_FLOAT8(entry.vi_color_reduct);
		FIELD_STR(entry.spect_type);
		FIELD_CHAR(entry.spect_type_source);
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

inline std::vector<CTHIPEntry> loadHIP(std::string base_path)
{
	std::cout << "loading hipparcos catalog..." << std::endl;

	std::vector<CTHIPEntry> data;
	readCache(data, base_path + "/cache/hip.dat");
	if (data.empty())
	{
		data = readTDat_HIP(base_path + "/heasarc_hipparcos.tdat");
		writeCache(data, base_path + "/cache/hip.dat");
		std::cout << "wrote cache file to " << base_path << "/cache/hip.dat" << std::endl;
	}
	else
		std::cout << "cache found, reading that instead." << std::endl;

	return data;
}
