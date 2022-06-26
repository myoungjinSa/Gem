#include <GLFW/glfw3.h>
#include "Window.h"
#include "GraphicsDevice.h"
#include "String/String.h"
#include <stdio.h>

void ErrorCallback(int error, const char* description) {
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

GraphicsDevice::GraphicsDevice() 
	: shaderVersion(nullptr)
{

}
GraphicsDevice::~GraphicsDevice() {

}

bool OpenGL::GLFWInitializer::Init() {
	glfwSetErrorCallback(ErrorCallback);
	if (!glfwInit())
		return false;

	return true;
}

void OpenGL::GLFWInitializer::SetShaderVersion() {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
}

OpenGL::OpenGL()
	:GraphicsDevice()
	, pGLFWInitializer(nullptr)
	, bUseGLFW(false)
{
}

OpenGL::OpenGL(bool _bUseGLFW) 
	: GraphicsDevice()
	, pGLFWInitializer(nullptr)
{
	bUseGLFW = _bUseGLFW;
}

OpenGL::~OpenGL() {

}

bool OpenGL::Initialize() {
	shaderVersion = "#version 130";
	if (bUseGLFW) {
		GLFWInitializer* pGLFWInitializer = new GLFWInitializer();
		if (!pGLFWInitializer->Init())
			return false;

		pGLFWInitializer->SetShaderVersion();
		delete pGLFWInitializer;
		return true;
	}

	return false;
}

void OpenGL::Shutdown(Window& window) {
	window.Close();
}

Window* OpenGL::CreateWindow(int width, int height, std::string title) {
	Window* pWindow = new OpenGLWindow;
	if (!pWindow->Create(*pWindow, width, height, title))
		return nullptr;

	return pWindow;
}

void OpenGL::SetCurrentContext(Window& window) {
	window.SetCurrentContext();
	window.EnableVSync(true);
}
void OpenGL::ClearWindow(Window& window, float r, float g, float b, float a) {
	int width, height;
	window.Resize(width, height);
	glViewport(0, 0, width, height);
	glClearColor(r * a, g * a, b * a, a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGL::SwapBuffers(Window& window) {
	window.SwapBuffers();
}

