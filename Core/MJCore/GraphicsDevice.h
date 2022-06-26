#ifndef _GRAPHICS_DEVICE_H
#define _GRAPHICS_DEVICE_H
#include <string>
#include "glew.h"


class Window;
class GraphicsDevice {
public:
	GraphicsDevice();
	virtual ~GraphicsDevice();

	const char* GetShaderVersion() const { return shaderVersion; }

	virtual bool Initialize() = 0;
	virtual Window* CreateWindow(int width, int height, std::string Title) { return nullptr; }
	virtual void Shutdown(Window& window) {}
	virtual bool ShouldClose(Window& window) { return false; }
	
	virtual void SetCurrentContext(Window& window){}
	virtual void ClearWindow(Window& window, float r, float g, float b, float a) {}
	virtual void ClearWindow(float r, float g, float b, float a);
	virtual void SwapBuffers(Window& window) {}
	virtual void SwapBuffers() {}

	virtual char* GetShaderLog(unsigned int programID) { return nullptr; }
	virtual char* GetLinkerLog(unsigned int programID) { return nullptr; }

	
protected:
	const char* shaderVersion;
};



class OpenGL : public GraphicsDevice {
public:
	class GLFWInitializer {
	public:
		bool Init();
		void SetShaderVersion();
	};
	OpenGL();
	OpenGL(bool bUseGLFW);
	~OpenGL() override;

	bool Initialize() override;

	//Window 관련 함수
	Window* CreateWindow(int width, int height, std::string Title)override;
	void SetCurrentContext(Window& window)override;
	void ClearWindow(Window& window, float r, float g, float b, float a) override;
	void ClearWindow(float r, float g, float b, float a) override;
	void SwapBuffers(Window& window) override;
	void SwapBuffers() override;
	void Shutdown(Window& window) override;

	char* GetShaderLog(unsigned int shaderID) override;
	char* GetLinkerLog(unsigned int programID) override;

	static void BindBuffer(void* pBuffer, unsigned int vertexBufferId, unsigned int vertexCount, unsigned int sizeofVertex);
protected:
	GLFWInitializer* pGLFWInitializer;
	bool bUseGLFW;

	Window* pWindow;
};

#endif