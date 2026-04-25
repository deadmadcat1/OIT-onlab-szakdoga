#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Settings {
	glm::uvec2 viewportSize = glm::uvec2(1920, 1080);
  glm::uvec2 renderResolution = glm::uvec2(1920, 1080);
  unsigned int depthPeel_layerCount = 4;
	struct {
		unsigned int layerCount = 4;
	} depthPeeling;
	struct {
		glm::vec3 startingPos = glm::vec3(0.0f, 8.0f, 16.0f);
		float orbitLockout = 1.0f;
	} camera;
	struct {
		const unsigned int maxAmount = 8;
		struct {
			float min = -15.0f;
			float max = 15.0f;
		} pos;
		struct {
			float min = 40.0f;
			float max = 50.0f;
		} irradiance;
	} lights;
};
