#pragma once
#include "GPUProgram.h"
#include "Camera.h"
#include "Light.h"
#include "Geometry.h"
#include "Object.h"
#include <vector>

class Scene {
	Camera camera;
	std::vector<Light*> lights;
	std::vector<Object*> objects;
public:
	bool set();

	void render();

	void animate(double dt);
};