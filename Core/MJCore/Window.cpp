#include <GLFW/glfw3.h>
#include "Window.h"
#include <assert.h>


OpenGLWindow::OpenGLWindow() 
	: pWindow(nullptr)
{

}
OpenGLWindow::~OpenGLWindow() {
	
}

bool OpenGLWindow::Create(Window& outWindow, int width, int height, std::string title) {
	if (IsCreated())
		return false;

	pWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!pWindow)
		return false;

	outWindow = *this;
}

void OpenGLWindow::Close() {
	glfwDestroyWindow(pWindow);
	glfwTerminate();
}
bool OpenGLWindow::ShouldClose() const{
	if (!IsCreated())
		return false;
	return glfwWindowShouldClose(pWindow) == false ? false : true;
}

void OpenGLWindow::PollEvents() {
	// Poll and handle events (inputs, window resize, etc.)
	// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
	// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
	// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
	// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.

	glfwPollEvents();
}

void OpenGLWindow::SetCurrentContext() {
	if (!IsCreated()) {
		assert(0);
		return;
	}
	glfwMakeContextCurrent(pWindow);
}


void OpenGLWindow::EnableVSync(bool bEnable) {
	glfwSwapInterval(bEnable ? 1 : 0); // Enable Sync
}

void OpenGLWindow::Resize(int& width, int& height) {
	if (!IsCreated())
		return;

	glfwGetFramebufferSize(pWindow, &width, &height);
}

void OpenGLWindow::SwapBuffers() {
	if (!IsCreated())
		return;

	glfwSwapBuffers(pWindow);
}
