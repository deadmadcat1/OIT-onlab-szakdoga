#pragma once
#include "GPUProgram.h"
#include "Camera.h"
#include "Light.h"
#include "Geometry.h"
#include "Object.h"
#include <vector>
#include <memory>

class Scene {
	std::unique_ptr<Camera> camera;
	std::vector<std::shared_ptr<Light>> lights;
	std::vector<std::shared_ptr<Object>> objects;
public:
	bool set();

	void render();

	void animate(double dt);
};