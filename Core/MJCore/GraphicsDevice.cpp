#include "Window.h"
#include "GraphicsDevice.h"

#include "String/String.h"
#include <stdio.h>
#include "Mesh.h"
#include <GLFW/glfw3.h>

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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
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
	shaderVersion = "#version 400";
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

void OpenGL::BindBuffer(void* pBuffer, unsigned int vertexBufferId, unsigned int vertexCount, unsigned int sizeofVertex) {
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), pBuffer, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // Vertex Position;
	glEnableVertexAttribArray(1); // Color
	glEnableVertexAttribArray(2); // Texture coordinates;
	glEnableVertexAttribArray(3); // Normal
	//glEnableVertexAttribArray(4); // Tangent

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeofVertex, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, false, sizeofVertex, (unsigned char*)NULL + (3 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeofVertex, (unsigned char*)NULL + (3 * sizeof(float)) + (4 * sizeof(unsigned char)));

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glVertexAttribPointer(3, 3, GL_FLOAT, false, sizeofVertex, (unsigned char*)NULL + (5 * sizeof(float)) + (4 * sizeof(unsigned char)));

	//glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	//glVertexAttribPointer(4, 3, GL_FLOAT, false, sizeofVertex, (unsigned char*)NULL + (8 * sizeof(float)) + (4 * sizeof(unsigned char)));
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

	pWindow = &window;
}

void OpenGL::ClearWindow(Window& window, float r, float g, float b, float a) {
	int width, height;
	window.Resize(width, height);
	glViewport(0, 0, width, height);
	glClearColor(r * a, g * a, b * a, a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGL::ClearWindow(float r, float g, float b, float a) {
	if (!pWindow)
		return;

	ClearWindow(*pWindow, r, g, b, a);
}

void OpenGL::SwapBuffers(Window& window) {
	window.SwapBuffers();
}
void OpenGL::SwapBuffers() {
	if (!pWindow)
		return;

	SwapBuffers(*pWindow);
}


char* OpenGL::GetShaderLog(unsigned int shaderID) {
	int logSize = 0;
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logSize);

	//Increment the size by one to handle also the null terminator.
	logSize++;
	char* pInfoLog = new char[logSize];
	if (!pInfoLog)
		return nullptr;

	glGetShaderInfoLog(shaderID, logSize, NULL, pInfoLog);

	return pInfoLog;
}

char* OpenGL::GetLinkerLog(unsigned int programID) {
	int logSize = 0;
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logSize);

	//Increment the size by one to handle also the null terminator.
	logSize++;
	char* pInfoLog = new char[logSize];
	if (!pInfoLog)
		return nullptr;

	glGetProgramInfoLog(programID, logSize, NULL, pInfoLog);

	return pInfoLog;
}