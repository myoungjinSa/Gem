#ifndef _SYSTEM_H
#define _SYSTEM_H

#include "UI/ImGUI/imgui.h"
#include "UI/ImGUI/imgui_impl_glfw.h"
#include "UI/ImGUI/imgui_impl_opengl3.h"

class GraphicsDevice;
class Window;
class System
{
public:
	System();
	~System();

	bool Initialize();
	void Run();
	void Shutdown();

private:
	GraphicsDevice* pDevice;
	Window* pMainWindow;
};


#endif

