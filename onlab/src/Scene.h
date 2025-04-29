#pragma once
#include <vector>
#include <unordered_map>
#include <memory>
#include <random>
#include "Camera.h"
#include "Light.h"
#include "Object.h"
#include "GPUProgram.h"
#include "TransparencyMode.h"

class Scene {
	class successToggle {
		bool value;
		bool changed = false;
	public:
		explicit successToggle(bool init) : value(init) {}
		void operator=(bool right) {
			if (changed) return;
			if (value != right) {
				value = right;
				changed = true;
			}
		}
		operator bool() const {
			return value;
		}
	};

	static const size_t maxNumOfLights = 8;
	std::unique_ptr<Camera> camera;
	std::vector<std::shared_ptr<Light>> lights;
	std::unordered_map<std::string, std::shared_ptr<GPUProgram>> shaderPrograms;
	std::vector<std::shared_ptr<Object>> opaqueObjects;
	std::vector<std::shared_ptr<Object>> transparentObjects;

	std::mt19937 rng;

	void seedRNG();

	void setMakeLights(int numOfLights);
	bool setMakeShaderPrograms();
	bool setMakeOpaqueObjects();
	bool setMakeTransparentObjects();

	void renderAlphaBlend();
	void renderDepthPeeling();
	void renderMBOIT();
	void renderWavelet();

public:
	bool set();

	void render(TransparencyMethod mode);

	void animate(float dt);
};