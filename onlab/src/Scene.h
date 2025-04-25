#pragma once
#include "Camera.h"
#include "Light.h"
#include "Object.h"
#include "GPUProgram.h"
#include "TransparencyMode.h"
#include <vector>
#include <memory>

class Scene {
	std::unique_ptr<Camera> camera;
	std::vector<std::shared_ptr<Light>> lights;
	std::vector<std::shared_ptr<GPUProgram>> shaderPrograms;
	std::vector<std::shared_ptr<Object>> opaqueObjects;
	std::vector<std::shared_ptr<Object>> transparentObjects;
public:
	bool set();

	void render(TransparencyMode mode);

	void animate(double dt);
};