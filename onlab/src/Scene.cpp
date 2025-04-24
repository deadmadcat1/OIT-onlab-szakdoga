#include "Scene.h"
#include "GPUProgram.h"
#include "Geometry.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#define	_USE_MATH_DEFINES
#include <math.h>
#include <random>

template <typename T> void appendVector(std::vector<T>& target, const std::vector<T>& donor) {
	target.insert(target.end(), donor.begin(), donor.end());
}

bool Scene::set() {
	struct successToggle{
		bool value, changed;
		successToggle(bool init) { value = init; changed = false; }
		void operator=(bool right) {
			if (changed) return;
			if (value != right) {
				value = right;
				changed = true;
			}
		}
		explicit operator bool() const{
			return value;
		}
	};

	std::random_device seed;
	std::mt19937 gen(seed());

	successToggle success = true;
	auto vertexShader = std::make_shared<Shader>(GL_VERTEX_SHADER);
	auto phongShader = std::make_shared<Shader>(GL_FRAGMENT_SHADER);
	success = vertexShader.get()->create("./shaders/barebones_vs.glsl");
	success = phongShader.get()->create("./shaders/phong_fs.glsl");

	if (!success) return false;

	auto simpleProgram = std::make_shared<GPUProgram>();
	simpleProgram.get()->addShader(vertexShader);
	simpleProgram.get()->addShader(phongShader);
	success = simpleProgram.get()->create("fragColor");

	if (!success) return false;

	auto simpleMaterial = std::make_shared<Material>(simpleProgram);
	simpleMaterial.get()->ka = glm::vec3(0.1f, 0.1f, 0.1f);
	simpleMaterial.get()->kd = glm::vec3(0.2f, 0.2f, 0.2f);
	simpleMaterial.get()->ks = glm::vec3(0.3f, 0.3f, 0.3f);
	simpleMaterial.get()->shine = 0.9f;

	auto plane = std::make_shared<Plane>();
	auto sphere = std::make_shared<Sphere>(32, 32);
	success = plane.get()->create();
	success = sphere.get()->create();

	if (!success) return false;

	std::vector<std::shared_ptr<Object>> box;
	for (int i = 0; i < 6; i++)
	{
		auto wall = std::make_unique<Object>(simpleMaterial, plane);
		wall.get()->scale(glm::vec3(10, 10, 1));
		box.push_back(std::move(wall));
	}
	box[0]->rotate(glm::vec3(M_PI_2, 0, 0));
	box[1]->rotate(glm::vec3(-M_PI_2, 0, 0));
	box[2]->rotate(glm::vec3(0, M_PI_2, 0));
	box[3]->rotate(glm::vec3(0, -M_PI_2, 0));
	box[5]->rotate(glm::vec3(0, M_PI, 0));

	box[0]->translate(glm::vec3(0, 10, 0));
	box[1]->translate(glm::vec3(0, -10, 0));
	box[2]->translate(glm::vec3(10, 0, 0));
	box[3]->translate(glm::vec3(-10, 0, 0));
	box[4]->translate(glm::vec3(0, 0, -10));
	box[5]->translate(glm::vec3(0, 0, 10));

	appendVector(opaqueObjects, box);

	std::vector<std::shared_ptr<Object>> subject;

	std::uniform_int_distribution<int> size_rand(1, 5);
	std::uniform_real_distribution<float> pos_rand(-10.0f, 10.0f);
	for (int i = 0; i < 20; i++)
	{
		auto ball = std::make_unique<Object>(simpleMaterial, sphere);
		ball.get()->scale(glm::vec3(size_rand(gen), size_rand(gen), size_rand(gen)));
		ball.get()->translate(glm::vec3(pos_rand(gen), pos_rand(gen), pos_rand(gen)));
		subject.push_back(std::move(ball));
	}

	appendVector(transparentObjects, subject);

	return true;
}

void Scene::render(){}

void Scene::animate(double dt) {}