#include "Window.h"

// TODO: use GLAD

// TODO: font texture generation
// TODO: font rendering from text buffer
// TODO: text buffer window control

int main()
{
	ORIWindow window;

	while(true)
	{
		window.makeActiveContext();

		auto tmp = glfwGetCurrentContext();

		glViewport(0, 0, window.getSize()[0], window.getSize()[1]);
		glClearColor(0.004f, 0.510f, 0.506f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(0.8f, 0.8f, 0.8f);
		glLineWidth(2.0f);
		glBegin(GL_LINES);

		glVertex2f(0.0f, 0.0f);
		glVertex2f(1.0f, 1.0f);
		glVertex2f(1.0f, 1.0f);
		glVertex2f(-1.0f, 0.5f);

		

		glEnd();

		window.swapBuffers();
		glfwPollEvents();
	}

	return 0;
}