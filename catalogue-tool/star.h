#pragma once

#include <vector>
#include <string>

enum CTCelesetialType
{
	STAR,
	GALAXY,
	NEBULA,
	STAR_CLUSTER,

};

struct CTCelestial
{
	std::vector<std::string> names;
	char constellation[3] = { 0 };
	std::pair<double, double> ra_dec;
	std::pair<double, double> gal_lat_long;
	std::pair<double, double> proper_motion;
	std::string spectral_type;
	uint16_t classification;
	CTCelesetialType object_type;
	float distance = 0;
	float visual_magnitude = 0;
	float absolute_magnitude = 0;
	uint32_t henry_draper_number = 0;
	uint32_t hipparcos_number = 0;
	uint16_t ngc2000_number = 0;
	uint8_t messier_number = 0;
	uint16_t ic_number = 0;
	std::string bayer_identifier;
	uint16_t flamsteed_number = 0;
	bool is_multiple = 0;
	uint8_t is_variable = 0;
	std::vector<std::string> descriptions;
};