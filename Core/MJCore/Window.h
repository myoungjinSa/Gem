#ifndef _WINDOW_H
#define _WINDOW_H
#include <string>

struct GLFWwindow;

class Window {
public:
	virtual ~Window() {};
	
	virtual bool Create(Window& outWindow, int width, int height, std::string title) { return false; }
	virtual bool IsCreated() const { return false; }
	virtual void Close() = 0;
	virtual bool ShouldClose()const { return false; }
	virtual void PollEvents(){}
	virtual void SetCurrentContext(){}
	virtual void EnableVSync(bool bEnable){}

	virtual void Resize(int& width, int& height){}
	virtual void SwapBuffers(){}
};


class OpenGLWindow : public Window {
public:
	OpenGLWindow();
	~OpenGLWindow() override;

	bool Create(Window& outWindow, int width, int height, std::string title);
	bool IsCreated() const override { return pWindow ? true : false; }
	void Close()override;
	bool ShouldClose() const override;
	void PollEvents() override;

	void SetCurrentContext() override;
	void EnableVSync(bool bEnable) override;

	void Resize(int& width, int& height) override;
	void SwapBuffers() override;

	GLFWwindow* GetWindowHandle() const { return pWindow; }

	GLFWwindow* pWindow;
};



#endif