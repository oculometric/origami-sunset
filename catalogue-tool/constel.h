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

		ret[id] = { ra, dec };
	}

	std::cout << "done." << std::endl;
	return ret;
}
