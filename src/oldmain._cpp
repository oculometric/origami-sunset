#include <string>
#include <cstring>
#include <map>

#include "../Window.h"
#include "../bitmap.h"

#define STUI_IMPLEMENTATION
#include "../stui/inc/stui.h"

using namespace std;

map<uint32_t, char> high_char_map
{
	// TODO: this will allow us to convert select UTF-8 characters to our limited ASCII codepage
	{ }
};

// TODO: use GLAD

// TODO: text buffer window control
// TODO: font convert to inline bitmap buffer

int main()
{
	ORIWindow window("origami", 320, 170);
	window.setResizeable(false);

	string demo_text = "The quick brown fox jumps over the lazy dog";

	auto size = window.getSize();
	char* buffer = new char[size[0] * size[1] * 4];
	
	int32_t font_width; int32_t font_height;
	char* font_bitmap;
	readRGBABitmap("font8x16.bmp", font_bitmap, font_width, font_height);
	size_t char_width = 8;
	size_t char_height = 16;
	size_t char_spacing = 1;

	size_t letter_spacing = 0;
	size_t line_height = 16;

	stui::Banner b("welcome STUI, you've been sorely missed");
	stui::BorderedBox bb(&b, "");

	while(true)
	{
		window.makeActiveContext();

		auto tmp = glfwGetCurrentContext();

		glViewport(0, 0, size[0], size[1]);

		stui::Tixel* charbuf = nullptr;
		stui::Coordinate buf_size = stui::Coordinate{ (int)(size[0] / (char_width + letter_spacing)), (int)(size[1] / line_height) };
		stui::Renderer::renderToBuffer(&bb, buf_size, charbuf);

		for (size_t i = 0; i < size[1] * size[0] * 4; i += 4)
		{
			((uint32_t*)(buffer + i))[0] = 0xFF808000;
		}

		size_t x_pos = 0;
		size_t y_pos = 0;
		for (size_t c_ind = 0; c_ind < buf_size.x * buf_size.y; c_ind++)
		{
			uint32_t hc = charbuf[c_ind].character;
			char c = hc > 255 ? 255 : hc;

			x_pos = (c_ind % buf_size.x) * (char_width + letter_spacing);
			y_pos = (c_ind / buf_size.x) * line_height;
			size_t buffer_offset = (x_pos + ((size[1] - y_pos - 1) * size[0])) * 4;

			if (hc > 255)
			{
				for (size_t j = 0; j < char_height; j++)
				{
					for (size_t i = 0; i < char_width; i++)
					{
						((uint32_t*)(buffer + (buffer_offset + (4 * i))))[0] = 0xFF151515;
					}
					buffer_offset -= size[0] * 4;
				}
				continue;
			}

			size_t font_x = (c % 32) * (char_width + (2 * char_spacing)) + char_spacing;
			size_t font_y = (c / 32) * (char_height + (2 * char_spacing)) + char_spacing;
			size_t font_offset = (font_x + ((font_height - font_y - 1) * font_width)) * 4;

			for (size_t j = 0; j < char_height; j++)
			{
				for (size_t i = 0; i < char_width; i++)
				{
					if (font_bitmap[font_offset + (4 * i)] != 0x00)
						((uint32_t*)(buffer + (buffer_offset + (4 * i))))[0] = 0xFF151515;
				}
				buffer_offset -= size[0] * 4;
				font_offset -= font_width * 4;
			}

			// x_pos += char_width + letter_spacing;
		}

		glRasterPos2f(-1.0f, -1.0f);
		glDrawPixels(size[0], size[1], GL_RGBA, GL_UNSIGNED_BYTE, buffer);
		glEnd();

		delete[] charbuf;
		charbuf = nullptr;

		window.swapBuffers();
		glfwPollEvents();
	}

	return 0;
}