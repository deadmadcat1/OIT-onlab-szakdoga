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

	camera = std::make_unique<Camera>();

	auto l = std::make_shared<Light>(0);
	l->changeAmbientColor(glm::vec3(0.7f, 0.5f, 0.3f));
	l->changeEmissiveColor(glm::vec3(1));
	lights.push_back(l);

	std::uniform_real_distribution<float> l_pos_rand(-15.0f, 15.0f);
	std::uniform_real_distribution<float> l_L_rand(0.0f, 1.0f);
	int numOfLights = 4; //TODO: somehow make consistent with how many lights a shader can handle;
	for (int i = 1; i < numOfLights; i++) {
		l = std::make_shared<Light>(i);
		l->setPointLight();
		l->translate(glm::vec3(l_pos_rand(gen), l_pos_rand(gen), l_pos_rand(gen)));
		l->changeEmissiveColor(glm::vec3(l_L_rand(gen), l_L_rand(gen), l_L_rand(gen)));
		lights.push_back(l);
	}

	if (!success) return false;

	auto vertexShader = std::make_shared<Shader>(GL_VERTEX_SHADER);
	auto phongShader = std::make_shared<Shader>(GL_FRAGMENT_SHADER);
	success = vertexShader->create("./shaders/barebones_vs.glsl");
	success = phongShader->create("./shaders/phong_fs.glsl");

	if (!success) return false;

	auto simpleProgram = std::make_shared<GPUProgram>();
	simpleProgram->addShader(vertexShader);
	simpleProgram->addShader(phongShader);
	success = simpleProgram->create("fragColor");

	if (!success) return false;

	shaderPrograms.push_back(simpleProgram);

	auto plane = std::make_shared<Plane>();
	auto sphere = std::make_shared<Sphere>(32, 32);
	success = plane->create();
	success = sphere->create();

	if (!success) return false;

	auto boxMaterial = std::make_shared<Material>();
	boxMaterial->kd = glm::vec3(0.2f, 0.2f, 0.2f);
	boxMaterial->shine = 0.9f;

	std::vector<std::shared_ptr<Object>> box;
	for (int i = 0; i < 6; i++)
	{
		auto wall = std::make_unique<Object>(boxMaterial, plane);
		wall->scale(glm::vec3(10, 10, 1));
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

	auto ballMaterial = std::make_shared<Material>();
	ballMaterial->kd = glm::vec3(0.2f, 0.2f, 0.2f);
	ballMaterial->shine = 0.9f;


	std::vector<std::shared_ptr<Object>> balls;

	std::uniform_int_distribution<int> size_rand(1, 5);
	std::uniform_real_distribution<float> pos_rand(-10.0f, 10.0f);
	for (int i = 0; i < 20; i++)
	{
		auto ball = std::make_unique<Object>(ballMaterial, sphere);
		ball->scale(glm::vec3(size_rand(gen), size_rand(gen), size_rand(gen)));
		ball->translate(glm::vec3(pos_rand(gen), pos_rand(gen), pos_rand(gen)));
		balls.push_back(std::move(ball));
	}

	appendVector(transparentObjects, balls);

	return true;
}

void Scene::render(TransparencyMode mode){
	//TODO 3gre_105[43]
}

void Scene::animate(double dt) {}