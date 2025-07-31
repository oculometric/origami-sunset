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