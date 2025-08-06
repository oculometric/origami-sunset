#pragma once

#include <fstream>
#include <string>
#include <iostream>

#define FIELD_FLOAT4(d) if (field_start != field_end) d = std::stof(line.substr(field_start, field_start - field_end)); \
field_start = field_end + 1; field_end = line.find('|', field_start)
#define FIELD_FLOAT8(d) if (field_start != field_end) d = std::stod(line.substr(field_start, field_start - field_end)); \
field_start = field_end + 1; field_end = line.find('|', field_start)
#define FIELD_INT(d) if (field_start != field_end) d = std::stoi(line.substr(field_start, field_start - field_end)); \
field_start = field_end + 1; field_end = line.find('|', field_start)
#define FIELD_LONG(d) if (field_start != field_end) d = std::stol(line.substr(field_start, field_start - field_end)); \
field_start = field_end + 1; field_end = line.find('|', field_start)
#define FIELD_CHAR(d) if (field_start != field_end) d = line.substr(field_start, field_start - field_end)[0]; \
field_start = field_end + 1; field_end = line.find('|', field_start)
#define FIELD_STR(d) if (field_start != field_end) memcpy(d, line.substr(field_start, field_end - field_start).c_str(), std::min(sizeof(d), field_end - field_start)); \
field_start = field_end + 1; field_end = line.find('|', field_start)

#define DATA_NONE(n) field_start += n
#define DATA_INT(d, n) try { d = std::stoi(line.substr(field_start, n)); } catch (std::exception _) { d = 0; }; field_start += n
#define DATA_EXISTS(d, c) d = (line[field_start] == c); field_start++
#define DATA_STR(d, n) memcpy(d, line.substr(field_start, n).c_str(), std::min(sizeof(d), (size_t)n)); field_start += n
#define DATA_FLOAT(d, n) try { d = std::stof(line.substr(field_start, n)); } catch (std::exception _) { d = 0; }; field_start += n
#define DATA_CHAR(d) d = line[field_start]; field_start++


inline bool findTDatDataStart(std::ifstream& file, std::string& line, size_t& total_rows, std::string& last_modified)
{
	while (true)
	{
		if (!file.getline((char*)line.c_str(), line.size(), '\n'))
		{
			std::cout << "failed!" << std::endl;
			return false;
		}
		if (line[0] == '#')
		{
			size_t c = 1;
			while (line[c] == ' ' && c < line.size())
				c++;
			if (line[c] == '\0') continue;
			if (strncmp(line.c_str() + c, "TOTAL ROWS", sizeof("TOTAL ROWS") - 1) == 0)
				total_rows = stol(line.substr(c + sizeof("TOTAL ROWS: ") - 1));
			if (strncmp(line.c_str() + c, "LAST MODIFIED", sizeof("LAST MODIFIED") - 1) == 0)
			{
				size_t start = c + sizeof("LAST MODIFIED: ") - 1;
				last_modified = line.substr(start, line.find('\0', start) - start);
			}
		}
		else if (strcmp(line.c_str(), "<DATA>") == 0)
			break;
	}

	return true;
}

static inline std::string fixPath(std::string path)
{
	std::string str = path.substr(0, path.find_last_of('/'));
#if defined(_WIN32)
	for (int i = 0; i < str.size(); i++)
	{
		if (str[i] == '/')
		{
			str[i] = '\\';
			str.insert(i, 1, '\\');
			i++;
		}
	}
#endif
	return str;
}

template<typename T>
inline void writeCache(const std::vector<T>& data, std::string path)
{
	system(("mkdir " + fixPath(path)).c_str());
	std::ofstream file(path, std::ios::binary);
	if (!file.is_open())
		return;

	file.write((const char*)(data.data()), sizeof(T) * data.size());

	file.close();
}

template<typename T>
inline void readCache(std::vector<T>& data, std::string path)
{
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	if (!file.is_open())
		return;

	size_t length = file.tellg();
	file.seekg(std::ios::beg);
	data.resize(length / sizeof(T));

	file.read((char*)(data.data()), length);

	file.close();
}