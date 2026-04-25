#pragma once
#include <vector>
#include <unordered_map>
#include <memory>
#include <random>
#include <glm/vec3.hpp>
#include "Camera.h"
#include "Light.h"
#include "Object.h"
#include "Framebuffer.h"
#include "GPUProgram.h"
#include "TransparencyMode.h"
#include "Settings.h"

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

	Settings _settings;
	std::unique_ptr<Camera> camera;
	float cameraOrbitLockoutTimer = 0.0f;
	std::vector<std::shared_ptr<Light>> lights;
	std::unordered_map<std::string, std::shared_ptr<GPUProgram>> shaderPrograms;
	std::unordered_map<std::string, std::shared_ptr<Framebuffer>> framebuffers;
	std::vector<std::shared_ptr<Object>> opaqueObjects;
	std::vector<std::shared_ptr<Object>> transparentObjects;

	std::mt19937 rng;

	std::unique_ptr<Object> fullscreenTexturedQuad;

	void seedRNG();

	void setMakeLights(unsigned int numOfLights);
	bool setMakeShaderPrograms();
	bool setMakeFramebuffers();
	bool setCreateFramebuffer(const std::string& name, const std::unordered_map<std::string, TargetParams>& targetParams);
	bool setMakeOpaqueObjects();
	bool setMakeTransparentObjects();

	void renderAlphaBlend();
	void renderDepthPeeling();
	void renderMBOIT();
	void renderWavelet();

public:
	bool set();

	void notifyResize(glm::uvec2 newsize);

	void render(TransparencyMethod mode);

	void animate(float dt);

	void panCameraNDC(glm::vec2 dNDC);

	void changeCameraPosition(glm::vec3 dpos);
	
	Scene(Settings settings) : _settings(settings) {}
};
