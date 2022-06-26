#ifndef _GRAPHICS_DEVICE_H
#define _GRAPHICS_DEVICE_H
#include <string>

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
	virtual void SwapBuffers(Window& window) {}

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
	void SwapBuffers(Window& window) override;
	void Shutdown(Window& window) override;

protected:
	GLFWInitializer* pGLFWInitializer;
	bool bUseGLFW;
};

#endif