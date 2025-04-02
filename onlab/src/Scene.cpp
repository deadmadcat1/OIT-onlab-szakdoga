#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include "Scene.h"

bool Scene::set() {
	struct boolToggle{
		bool value;
		boolToggle(bool init) : value(init) {}
		void operator=(bool right) {
			value = (!value) ? right : value;
		}
		bool operator!() {
			return !value;
		}
	};

	boolToggle success = true;
	std::unique_ptr<Shader> vertexShader = std::make_unique<Shader>(GL_VERTEX_SHADER);
	std::unique_ptr<Shader> phongShader = std::make_unique<Shader>(GL_FRAGMENT_SHADER);
	success = vertexShader.get()->create("/shaders/barebones_vs.glsl");
	success = phongShader.get()->create("/shaders/phong_fs.glsl");

	if (!success) return false;

	std::unique_ptr<GPUProgram> simpleProgram = std::make_unique<GPUProgram>();
	success = simpleProgram.get()->create("fragColor");

	if (!success) return false;

	simpleProgram.get()->addShader(vertexShader);
	simpleProgram.get()->addShader(phongShader);

	std::unique_ptr<Material> simpleMaterial = std::make_unique<Material>(simpleProgram);
	simpleMaterial.get()->ka = glm::vec3(0.1f, 0.1f, 0.1f);
	simpleMaterial.get()->kd = glm::vec3(0.2f, 0.2f, 0.2f);
	simpleMaterial.get()->ks = glm::vec3(0.3f, 0.3f, 0.3f);
	simpleMaterial.get()->shine = 0.9f;

	std::unique_ptr<Plane> plane = std::make_unique<Plane>();
	success = plane.get()->create();

	std::unique_ptr<Object> fullscreenQuad = std::make_unique<Object>(simpleMaterial, plane);
	fullscreenQuad.get().


	return true;
}

void Scene::render(){}

void Scene::animate(double dt) {}