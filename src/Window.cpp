#include "Window.h"

using namespace std;

ORIWindow::ORIWindow(string title, size_t width, size_t height)
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}

void ORIWindow::setResizeable(bool b)
{
    glfwSetWindowAttrib(window, GLFW_RESIZABLE, b);
}

void ORIWindow::setTitle(string t)
{
    glfwSetWindowTitle(window, t.c_str());
}

void ORIWindow::setWidth(size_t w)
{
    glfwSetWindowSize(window, w, getSize()[1]);
}

void ORIWindow::setHeight(size_t h)
{
    glfwSetWindowSize(window, getSize()[0], h);
}

void ORIWindow::setSize(size_t w, size_t h)
{
    glfwSetWindowSize(window, w, h);
}

array<size_t, 2> ORIWindow::getSize()
{
    int w; int h;
    glfwGetWindowSize(window, &w, &h);
    return array<size_t, 2>({ static_cast<size_t>(w), static_cast<size_t>(h) });
}

void ORIWindow::swapBuffers()
{
    glfwSwapBuffers(window);
    glfwSwapInterval(1);
}

void ORIWindow::makeActiveContext()
{
    glfwMakeContextCurrent(window);
}

ORIWindow::~ORIWindow()
{
    glfwDestroyWindow(window);
}
