#ifndef _GRAPHICS_DEVICE_H
#define _GRAPHICS_DEVICE_H
#include <string>

struct GLFWwindow;

class GraphicsDevice {
public:
	GraphicsDevice();
	virtual ~GraphicsDevice();

	const char* GetShaderVersion() const { return shaderVersion; }

	virtual bool Initialize() = 0;
	virtual void Shutdown() = 0;

	//virtual HWND CreateHWNDWindow(int width, int height);
	virtual GLFWwindow* CreateGLFWWindow(int width, int height, std::string Title) { return nullptr; }
	virtual void ClearWindow(GLFWwindow& window, float r, float g, float b, float a) {}
	virtual void SwapBuffers(GLFWwindow& window) {}
	virtual void Shutdown(GLFWwindow& window) {}

protected:
	const char* shaderVersion;
};

class OpenGL : public GraphicsDevice {
public:
	OpenGL();
	~OpenGL() override;

	bool Initialize() override;
	void Shutdown() override;

	GLFWwindow* CreateGLFWWindow(int width, int height, std::string Title)override;

	void ClearWindow(GLFWwindow& window, float r, float g, float b, float a) override;
	void SwapBuffers(GLFWwindow& window) override;
	void Shutdown(GLFWwindow& window) override;
};

#endif