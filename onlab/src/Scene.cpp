#include <GL/glew.h>
#include <glm/glm.hpp>
#define	_USE_MATH_DEFINES
#include <math.h>
#include "Scene.h"

template <typename T> void appendVector(std::vector<T> target, std::vector<T> donor) {
	target.insert(target.end(), donor.begin(), donor.end());
}

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
	auto vertexShader = std::make_shared<Shader>(GL_VERTEX_SHADER);
	auto phongShader = std::make_shared<Shader>(GL_FRAGMENT_SHADER);
	success = vertexShader.get()->create("/shaders/barebones_vs.glsl");
	success = phongShader.get()->create("/shaders/phong_fs.glsl");

	if (!success) return false;

	auto simpleProgram = std::make_shared<GPUProgram>();
	success = simpleProgram.get()->create("fragColor");

	if (!success) return false;

	simpleProgram.get()->addShader(vertexShader);
	simpleProgram.get()->addShader(phongShader);

	auto simpleMaterial = std::make_shared<Material>(simpleProgram);
	simpleMaterial.get()->ka = glm::vec3(0.1f, 0.1f, 0.1f);
	simpleMaterial.get()->kd = glm::vec3(0.2f, 0.2f, 0.2f);
	simpleMaterial.get()->ks = glm::vec3(0.3f, 0.3f, 0.3f);
	simpleMaterial.get()->shine = 0.9f;

	auto plane = std::make_shared<Plane>();
	success = plane.get()->create();

	if (!success) return false;

	std::vector<std::shared_ptr<Object>> sceneBox;
	for (int i = 0; i < 6; i++)
	{
		auto wall = std::make_unique<Object>(simpleMaterial, plane);
		wall.get()->scale(glm::vec3(10, 10, 1));
		sceneBox.push_back(std::move(wall));
	}
	sceneBox[0]->rotate(glm::vec3(M_PI_2, 0, 0));
	sceneBox[1]->rotate(glm::vec3(-M_PI_2, 0, 0));
	sceneBox[2]->rotate(glm::vec3(0, M_PI_2, 0));
	sceneBox[3]->rotate(glm::vec3(0, -M_PI_2, 0));
	sceneBox[5]->rotate(glm::vec3(0, M_PI, 0));

	sceneBox[0]->translate(glm::vec3(0, 10, 0));
	sceneBox[1]->translate(glm::vec3(0, -10, 0));
	sceneBox[2]->translate(glm::vec3(10, 0, 0));
	sceneBox[3]->translate(glm::vec3(-10, 0, 0));
	sceneBox[4]->translate(glm::vec3(0, 0, -10));
	sceneBox[5]->translate(glm::vec3(0, 0, 10));

	appendVector(objects, sceneBox);

	return true;
}

void Scene::render(){}

void Scene::animate(double dt) {}