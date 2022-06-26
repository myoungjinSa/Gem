#ifndef _SCENE_H
#define _SCENE_H

#include "..\..\Core\MJCore\Object.h"

class Scene final : public Object {
public:
	Scene();
	~Scene();

	bool BuildObject(Renderer& renderer);
	void Update(double elapsedTime);
	bool Render(Renderer& renderer);
	void Shutdown(Renderer& renderer);


};

#endif

