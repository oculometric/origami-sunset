#pragma once

#include <GLFW/glfw3.h>
#include <string>
#include <array>

class ORIWindow
{
private:
	GLFWwindow* window;

public:
	ORIWindow(std::string title = "origami", size_t width = 640, size_t height = 480);

	void setResizeable(bool b);
	void setTitle(std::string t);
	void setWidth(size_t w);
	void setHeight(size_t h);
	void setSize(size_t w, size_t h);

	std::array<size_t, 2> getSize();
	std::string getTitle();

	// TODO: context and swapping funcs, drawing to buffer
	void swapBuffers();
	void makeActiveContext();

	~ORIWindow();
};