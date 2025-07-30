#include <fstream>
#include <string>
#include <vector>

#pragma pack(1)
struct CTHDEntry
{
	char name[20];
	double ra;
	double dec;
	float bii;
	float lii;
	uint16_t classi;
	char spectral_type[4];
	float vmag;
	float pgmag;
	uint32_t hd_number;
	double dircos1;
	double dircos2;
	double dircos3;
	uint16_t variability_flag;
	uint16_t multiplicity_flag;
};

inline std::vector<CTHDEntry> readTDat_HD(std::string path)
{
	std::ifstream file(path, std::ios::binary);
	if (!file.is_open())
		return {};

	std::string line;
	line.resize(512, '\0');
	while (true)
	{
		file.getline((char*)line.c_str(), 512, '\n');
		if (strcmp(line.c_str(), "<DATA>") == 0)
			break;
	}
	
	std::vector<CTHDEntry> entries;
	while (true)
	{
		file.getline((char*)line.c_str(), 512, '\n');
		if (strcmp(line.c_str(), "<END>") == 0 || strlen(line.c_str()) == 0)
			break;

		CTHDEntry entry;
		size_t field_start = 0;
		size_t field_end = line.find('|', field_start);
		entry.bii = std::stof(line.substr(field_start, field_start - field_end));
		field_start = field_end + 1; field_end = line.find('|', field_start);
		entry.classi = std::stoi(line.substr(field_start, field_start - field_end));
		field_start = field_end + 1; field_end = line.find('|', field_start);
		entry.dec = std::stod(line.substr(field_start, field_start - field_end));
		field_start = field_end + 1; field_end = line.find('|', field_start);
		entry.dircos1 = std::stod(line.substr(field_start, field_start - field_end));
		field_start = field_end + 1; field_end = line.find('|', field_start);
		entry.dircos2 = std::stod(line.substr(field_start, field_start - field_end));
		field_start = field_end + 1; field_end = line.find('|', field_start);
		entry.dircos3 = std::stod(line.substr(field_start, field_start - field_end));
		field_start = field_end + 1; field_end = line.find('|', field_start);
		entry.hd_number = std::stoi(line.substr(field_start, field_start - field_end));
		field_start = field_end + 1; field_end = line.find('|', field_start);
		entry.lii = std::stof(line.substr(field_start, field_start - field_end));
		field_start = field_end + 1; field_end = line.find('|', field_start);
		entry.multiplicity_flag = std::stoi(line.substr(field_start, field_start - field_end));
		field_start = field_end + 1; field_end = line.find('|', field_start);
		memcpy(entry.name, line.substr(field_start, field_end - field_start).c_str(), 20);
		field_start = field_end + 1; field_end = line.find('|', field_start);
		entry.pgmag = std::stof(line.substr(field_start, field_start - field_end));
		field_start = field_end + 1; field_end = line.find('|', field_start);
		entry.ra = std::stof(line.substr(field_start, field_start - field_end));
		field_start = field_end + 1; field_end = line.find('|', field_start);
		memcpy(entry.spectral_type, line.substr(field_start, field_end - field_start).c_str(), 4);
		field_start = field_end + 1; field_end = line.find('|', field_start);
		entry.variability_flag = std::stoi(line.substr(field_start, field_start - field_end));
		field_start = field_end + 1; field_end = line.find('|', field_start);
		entry.vmag = std::stof(line.substr(field_start, field_start - field_end));

		entries.push_back(entry);
	}

	file.close();
	return entries;
}
