#include <string>
#include <iostream>
#include <fstream>

using namespace std;

#pragma pack(push)
#pragma pack(1)
struct PTBitmapHeader
{
	uint8_t sig_0 = 'B';
	uint8_t sig_1 = 'M';
	uint32_t size_bytes = 0;
	uint16_t res_0 = 'P';
	uint16_t res_1 = 'T';
	uint32_t data_offset = 0;
};

struct PTBitmapInfoHeader
{
	uint32_t header_size = 40;
	int32_t bitmap_width = 0;
	int32_t bitmap_height = 0;
	uint16_t planes = 1;
	uint16_t bits_per_pixel = 32;
	uint32_t compresion = 0;
	uint32_t data_length = 0;
	int32_t ppm_horizontal = 144;
	int32_t ppm_vertical = 144;
	uint32_t colour_palette_count = 0;
	uint32_t important_colours = 0;
};
#pragma pack(pop)


bool writeRGBABitmap(string path, char* data, int32_t width, int32_t height)
{
    // if the image params are nonsense, don't do anything
    if (width <= 0 || height <= 0 || data == nullptr)
        return false;

    // if we can't open the file, don't do anything
    ofstream file(path, ios::binary);
    if (!file.is_open())
        return false;

    // create a bitmap header
    PTBitmapHeader header;
    header.data_offset = sizeof(PTBitmapHeader) + sizeof(PTBitmapInfoHeader);
    header.size_bytes = header.data_offset + (width * height * 4);

    // create bitmapinfo header
    PTBitmapInfoHeader info_header;
    info_header.bitmap_width = width;
    info_header.bitmap_height = height;
    info_header.bits_per_pixel = 32;
    info_header.compresion = 0;
    info_header.data_length = (width * height * 4);

    // write header, followed by info header, followed by data
    file.write((char*)&header, sizeof(PTBitmapHeader));
    file.write((char*)&info_header, sizeof(PTBitmapInfoHeader));

    // reorganise data RGBA -> BGRA
    char* other_buf = new char[width * height * 4];
    size_t length = width * height;
    for (size_t i = 0; i < length; i++)
    {
        other_buf[(i * 4) + 0] = data[(i * 4) + 2];
        other_buf[(i * 4) + 1] = data[(i * 4) + 1];
        other_buf[(i * 4) + 2] = data[(i * 4) + 0];
        other_buf[(i * 4) + 3] = data[(i * 4) + 3];
    }

    file.write((char*)other_buf, info_header.data_length);
    delete[] other_buf;

    file.close();

    return true;
}

bool readRGBABitmap(string path, char*& data, int32_t& width, int32_t& height)
{
    // if we can't open the file, don't do anything
    ifstream file(path, ios::binary | ios::ate);
    if (!file.is_open())
        return false;

    // grab the file size
    size_t size = file.tellg();
    file.seekg(0);

    if (size < sizeof(PTBitmapHeader) + sizeof(PTBitmapInfoHeader))
    {
        file.close();
        return false;
    }

    // grab the bitmap header
    PTBitmapHeader header;
    file.read((char*)(&header), sizeof(PTBitmapHeader));

    // check the validity of the header
    PTBitmapHeader sample;
    if (header.sig_0 != sample.sig_0 || header.sig_1 != sample.sig_1)
    {
        file.close();
        return false;
    }

    if (size < header.size_bytes || header.data_offset < (sizeof(PTBitmapHeader) + sizeof(PTBitmapInfoHeader)))
    {
        file.close();
        return false;
    }

    // grab the bitmap info header
    PTBitmapInfoHeader info_header;
    file.read((char*)(&info_header), sizeof(PTBitmapInfoHeader));

    // check the validity of the info header
    PTBitmapInfoHeader info_sample;
    if (info_header.header_size != info_sample.header_size)
    {
        file.close();
        return false;
    }

    if (size < info_header.data_length + header.data_offset)
    {
        file.close();
        return false;
    }

    if (info_header.compresion != 0)
    {
        file.close();
        return false;
    }

    char* buffer = new char[info_header.data_length];
    file.read(buffer, info_header.data_length);

    file.close();

    width = info_header.bitmap_width;
    height = info_header.bitmap_height;
    if (info_header.bits_per_pixel == 32)
    {
        data = buffer;
        return true;
    }
    else if (info_header.bits_per_pixel == 24)
    {
        char* other_buf = new char[width * height * 4];
        size_t length = width * height;
        for (size_t i = 0; i < length; i++)
        {
            other_buf[(i * 4) + 0] = buffer[(i * 3) + 2];
            other_buf[(i * 4) + 1] = buffer[(i * 3) + 1];
            other_buf[(i * 4) + 2] = buffer[(i * 3) + 0];
            other_buf[(i * 4) + 3] = 255;
        }

        data = other_buf;
        delete[] buffer;

        return true;
    }
    else return false;
}

int main()
{
	int32_t font_width; int32_t font_height;
	char* font_bitmap;
	readRGBABitmap("font8x16.bmp", font_bitmap, font_width, font_height);
	size_t char_width = 8;
	size_t char_height = 16;
	size_t char_spacing = 1;

	size_t letter_spacing = 0;
	size_t line_height = 16;

	for (size_t c = 0; c < 256; c++)
	{
		size_t font_x = (c % 32) * (char_width + (2 * char_spacing)) + char_spacing;
		size_t font_y = (c / 32) * (char_height + (2 * char_spacing)) + char_spacing;
		size_t font_offset = (font_x + ((font_height - font_y - 1) * font_width)) * 4;

		bool is_null_char = true;
		std::string s = "";
		for (size_t j = 0; j < char_height; j++)
		{
			s += "0b";
			for (size_t i = 0; i < char_width; i++)
			{
				if (font_bitmap[font_offset + (4 * i)] != 0x00)
				{
					s += '1';
					is_null_char = false;
				}
				else
					s += '0';
			}
			s += ",\n";
			font_offset -= font_width * 4;
		}
		if (is_null_char)
		{
			for (size_t j = 0; j < char_height; j++)
			{
				s[(j * 12) + 11] = ' ';
			}
		}
		std::cout << "// 0x" << std::hex << c << ", aka '" << (char)c << "'" << std::endl;
		std::cout << s << std::endl << std::endl;
	}

	return 0;
}