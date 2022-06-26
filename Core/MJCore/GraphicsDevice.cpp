#include <GLFW/glfw3.h>
#include "GraphicsDevice.h"
#include "String/String.h"

GraphicsDevice::GraphicsDevice() 
	: shaderVersion(nullptr)
{

}
GraphicsDevice::~GraphicsDevice() {

}

OpenGL::OpenGL() 
	:GraphicsDevice()
{
}
OpenGL::~OpenGL() {

}

bool OpenGL::Initialize() {

	shaderVersion = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	return false;
}

void OpenGL::Shutdown() {

}

void OpenGL::Shutdown(GLFWwindow& window) {
	glfwDestroyWindow(&window);
	glfwTerminate();
}

GLFWwindow* OpenGL::CreateGLFWWindow(int width, int height, std::string title) {
	GLFWwindow* pWindow = nullptr;

	pWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	return pWindow;
}

void OpenGL::ClearWindow(GLFWwindow& window, float r, float g, float b, float a) {
	int width, height;
	glfwGetFramebufferSize(&window, &width, &height);
	glViewport(0, 0, width, height);
	glClearColor(r * a, g * a, b * a, a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGL::SwapBuffers(GLFWwindow& window) {
	glfwSwapBuffers(&window);
}

