#pragma once

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

inline std::map<std::string, std::vector<std::pair<float, float>>> readBounds(std::string path)
{
	std::cout << "reading boundary database... " << std::endl;

	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cout << "failed!" << std::endl;
		return { };
	}

	std::map<std::string, std::vector<std::pair<float, float>>> ret;

	std::string line;
	line.resize(64, '\0');
	while (file.getline((char*)line.c_str(), 64, '\n'))
	{
		size_t first_space = line.find(' ', 2);
		size_t second_space = line.find(' ', first_space + 2);
		float ra = stof(line.substr(0, first_space));
		float dec = stof(line.substr(first_space, second_space - first_space));
		std::string id = line.substr(second_space + 1, 4);
		if (id[id.size() - 1] == '\0') id.pop_back();

		ret[id].push_back({ ra, dec });
	}

	std::cout << "done." << std::endl;
	return ret;
}

inline std::map<std::string, std::pair<float, float>> readCenters(std::string path)
{
	std::cout << "reading center database... " << std::endl;

	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cout << "failed!" << std::endl;
		return { };
	}

	std::map<std::string, std::pair<float, float>> ret;

	std::string line;
	line.resize(64, '\0');
	while (file.getline((char*)line.c_str(), 64, '\n'))
	{
		size_t first_space = line.find(' ', 2);
		size_t second_space = line.find(' ', first_space + 2);
		float ra = stof(line.substr(0, first_space));
		float dec = stof(line.substr(first_space, second_space - first_space));
		second_space = line.find(' ', second_space + 10);
		std::string id = line.substr(second_space + 1, 4);
		if (id[id.size() - 1] == '\0') id.pop_back();

		ret[id] = { ra, dec };
	}

	std::cout << "done." << std::endl;
	return ret;
}

inline bool testStarInConstellation(std::pair<float, float> star, std::pair<float, float> center, const std::vector<std::pair<float, float>>& boundary)
{
	std::pair<float, float> alt_star = star;
	if (abs(alt_star.first - center.first) > 12.0f)
	{
		if (alt_star.first > center.first)
			alt_star.first -= 24.0f;
		else
			center.first -= 24.0f;
	}
	//if (abs((alt_star.first - 24.0f) - center.first) < abs(alt_star.first - center.first))
	//	alt_star.first -= 24.0f;

	// based on this https://stackoverflow.com/a/3838357/7332101
	std::pair<float, float> i2 = { std::min(alt_star.first, center.first), std::max(alt_star.first, center.first) };
	float a2 = (alt_star.second - center.second) / (alt_star.first - center.first);
	float b2 = alt_star.second - (a2 * alt_star.first);
	//std::pair<float, float> dif = { alt_star.first - center.first, alt_star.second - center.second };
	//float x3y4 = (alt_star.first * center.second) - (alt_star.second * center.first);
	//float l_dir = sqrt((dir.first * dir.first) + (dir.second * dir.second));
	//dir.first /= l_dir; dir.second /= l_dir;
	// 

	size_t num_intersections = 0;
	for (int i = 0; i < boundary.size(); i++)
	{
		auto p1 = boundary[i];
		auto p2 = boundary[(i + 1) % boundary.size()];
		if (abs(p1.first - p2.first) > 12.0f)
		{
			if (p2.first > p1.first)
				p2.first -= 24.0f;
			else
				p1.first -= 24.0f;
		}

		std::pair<float, float> i1 = { std::min(p1.first, p2.first), std::max(p1.first, p2.first) };
		std::pair<float, float> ia = { std::max(i1.first, i2.first), std::min(i1.second, i2.second) };

		if (i1.second < i2.first)
			continue;

		float a1 = (p1.second - p2.second) / (p1.first - p2.first);
		float b1 = p1.second - (a1 * p1.first);

		if (a1 == a2)
			continue;

		float xa = (b2 - b1) / (a1 - a2);

		if ((xa < ia.first) || xa > ia.second)
			continue;

		num_intersections++;

		/*float denom = ((p1.first - p2.first) * dif.second)
					- ((p1.second - p2.second) * dif.first);
		if (denom == 0.0f) continue;
		float x1y2 = (p1.first * p2.second) - (p1.second * p2.first);
		float ix = (x1y2 * dif.first) - ((p1.first - p2.first) * x3y4);
		float iy = */
	}
	// TODO: here
	return (num_intersections % 2) == 0;
}
