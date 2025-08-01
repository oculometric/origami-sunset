#pragma once

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

struct CTConstellation
{
    std::string name;
    std::string identifier;
    std::pair<double, double> center;
    std::vector<std::pair<double, double>> boundary;
};

inline std::map<std::string, std::vector<std::pair<double, double>>> readBounds(std::string path)
{
	std::cout << "reading boundary database... " << std::endl;

	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cout << "failed!" << std::endl;
		return { };
	}

	std::map<std::string, std::vector<std::pair<double, double>>> ret;

	std::string line;
	line.resize(64, '\0');
	while (file.getline((char*)line.c_str(), 64, '\n'))
	{
		size_t first_space = line.find(' ', 2);
		size_t second_space = line.find(' ', first_space + 2);
        double ra = stod(line.substr(0, first_space));
        double dec = stod(line.substr(first_space, second_space - first_space));
		std::string id = line.substr(second_space + 1, 4);
		if (id[id.size() - 1] == '\0') id.pop_back();

		ret[id].push_back({ ra, dec });
	}

	std::cout << "done." << std::endl;
	return ret;
}

inline std::map<std::string, std::pair<double, double>> readCenters(std::string path)
{
	std::cout << "reading center database... " << std::endl;

	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cout << "failed!" << std::endl;
		return { };
	}

	std::map<std::string, std::pair<double, double>> ret;

	std::string line;
	line.resize(64, '\0');
	while (file.getline((char*)line.c_str(), 64, '\n'))
	{
		size_t first_space = line.find(' ', 2);
		size_t second_space = line.find(' ', first_space + 2);
        double ra = stod(line.substr(0, first_space));
        double dec = stod(line.substr(first_space, second_space - first_space));
		second_space = line.find(' ', second_space + 10);
		std::string id = line.substr(second_space + 1, 4);
		if (id[id.size() - 1] == '\0') id.pop_back();

		ret[id] = { ra, dec };
	}

	std::cout << "done." << std::endl;
	return ret;
}

static inline uint8_t getQuarter(double g)
{
    if (abs(g) > 0.5f)
    {
        if (g > 0.0f)
            return 0;
        else
            return 1;
    }
    else
    {
        if (g > 0.0f)
            return 2;
        else
            return 3;
    }
}

inline std::vector<CTConstellation> loadConstellations(std::string base_path)
{
    auto boundary_data = readBounds(base_path + "/bound_in_20.txt");
    auto center_data = readCenters(base_path + "/centers_20.txt");

    // TODO: compute bounding box to accelerate containment checks

    if (boundary_data.empty() || center_data.empty())
        return { };

    center_data.erase("SER");

    std::vector<CTConstellation> constels;
    for (const auto& pair : boundary_data)
    {
        CTConstellation constel;
        constel.identifier = pair.first;
        constel.boundary = boundary_data[pair.first];
        for (auto& p : constel.boundary)
            p.first = p.first * (360.0f / 24.0f);
        constel.center = center_data[pair.first];
        constel.center.first = constel.center.first * (360.0f / 24.0f);

        constels.push_back(constel);
    }

    return constels;
}

inline bool checkContains(std::pair<double, double> point, const CTConstellation& constellation)
{
    // based on this https://stackoverflow.com/a/3838357/7332101
    std::pair<double, double> i2 = { std::min(point.first, (double)constellation.center.first), std::max(point.first, (double)constellation.center.first)};
    double a2 = (point.second - (double)constellation.center.second) / (point.first - (double)constellation.center.first);
    double b2 = point.second - (a2 * point.first);

    size_t num_intersections = 0;
    std::vector<std::pair<double, double>> intersections;
    std::vector<uint8_t> intersection_gradients;
    for (int i = 0; i < constellation.boundary.size(); i++)
    {
        auto p1 = constellation.boundary[i];
        auto p2 = constellation.boundary[(i + 1) % constellation.boundary.size()];
        if (abs(p1.first - p2.first) > 180.0f)
        {
            if (p2.first > p1.first)
                p2.first -= 360.0f;
            else
                p1.first -= 360.0f;
        }

        std::pair<double, double> i1 = { std::min(p1.first, p2.first), std::max(p1.first, p2.first) };
        std::pair<double, double> ia = { std::max(i1.first, i2.first), std::min(i1.second, i2.second) };

        if (i1.second < i2.first)
            continue;

        double a1 = (p1.second - p2.second) / (p1.first - p2.first);
        double b1 = p1.second - (a1 * p1.first);

        if (a1 == a2)
            continue;

        double xa = (b2 - b1) / (a1 - a2);

        if ((xa < ia.first) || xa > ia.second)
            continue;

        double ya = ((a1 * xa) + b1);

        bool duplicate_intersection = false;
        for (int j = 0; j < intersections.size(); j++)
        {
            auto p = intersections[j];
            uint8_t g = intersection_gradients[j];
            if (abs(p.first - xa) < 0.0001f
                && abs(p.second - ya) < 0.0001f
                && g == getQuarter(a1))
            {
                duplicate_intersection = true;
                break;
            }
        }
        if (!duplicate_intersection)
        {
            num_intersections++;
            intersections.push_back({ xa, ya });
            intersection_gradients.push_back(getQuarter(a1));
        }
    }

	return (num_intersections % 2) == 0;
}
