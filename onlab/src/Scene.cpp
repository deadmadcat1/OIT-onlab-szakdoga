#include "Scene.h"
#include "GPUProgram.h"
#include "Geometry.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#define	_USE_MATH_DEFINES
#include <math.h>

template <typename T> void appendVector(std::vector<T>& target, const std::vector<T>& donor) {
	target.insert(target.end(), donor.begin(), donor.end());
}

bool Scene::set() {
	seedRNG();

	int numOfLights = 4;	//TODO: move to a settings class

	camera = std::make_unique<Camera>();
	camera->translate(glm::vec3(12.0f, 12.0f, 12.0f));

	setMakeLights(numOfLights);

	if (!setMakeShaderPrograms()) return false;

	if (!setMakeOpaqueObjects()) return false;

	if (!setMakeTransparentObjects()) return false;

	return true;
}

void Scene::setMakeLights(int numOfLights) {
	if (numOfLights > maxNumOfLights) {
		numOfLights = maxNumOfLights;
		std::cerr << "numOfLights exceeds maximum, count truncated to bounds!" << std::endl;
	}
	int lightsMade = 0;
	auto l = std::make_shared<Light>(0);
	l->changeAmbientColor(glm::vec3(0.7f, 0.5f, 0.3f));
	l->changeEmissiveColor(glm::vec3(1));
	lights[lightsMade] = l;
	++lightsMade;

	std::uniform_real_distribution<float> l_pos_rand(-15.0f, 15.0f);
	std::uniform_real_distribution<float> l_L_rand(0.0f, 1.0f);

	for (int i = lightsMade; i < numOfLights; i++){
		l = std::make_shared<Light>(i);
		l->setPointLight();
		l->translate(glm::vec3(l_pos_rand(rng), l_pos_rand(rng), l_pos_rand(rng)));
		l->changeEmissiveColor(glm::vec3(l_L_rand(rng), l_L_rand(rng), l_L_rand(rng)));
		lights[i] = l;
	}
}

bool Scene::setMakeShaderPrograms() {
	successToggle success(true);
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

	success = shaderPrograms.emplace("phongBlinn", simpleProgram).second;	//https://en.cppreference.com/w/cpp/container/unordered_map/emplace#Return_value
	return success;
}

bool Scene::setMakeOpaqueObjects() {
	successToggle success(true);

	auto plane = std::make_shared<Plane>();
	success = plane->create();

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

	return true;
}

bool Scene::setMakeTransparentObjects() {
	successToggle success(true);

	auto sphere = std::make_shared<Sphere>(32, 32);
	success = sphere->create();

	if (!success) return false;

	auto ballMaterial = std::make_shared<Material>();
	ballMaterial->kd = glm::vec3(0.2f, 0.2f, 0.2f);
	ballMaterial->shine = 0.9f;

	std::vector<std::shared_ptr<Object>> balls;

	std::uniform_int_distribution<int> size_rand(1, 5);
	std::uniform_real_distribution<float> pos_rand(-10.0f, 10.0f);
	for (int i = 0; i < 20; i++)
	{
		auto ball = std::make_unique<Object>(ballMaterial, sphere);
		ball->scale(glm::vec3(size_rand(rng), size_rand(rng), size_rand(rng)));
		ball->translate(glm::vec3(pos_rand(rng), pos_rand(rng), pos_rand(rng)));
		balls.push_back(std::move(ball));
	}

	appendVector(transparentObjects, balls);

	return true;
}

void Scene::render(TransparencyMode mode){
	auto phongProgram = shaderPrograms["phongBlinn"];
	phongProgram->activate();

	camera->bindUniforms(phongProgram);
	for (const auto& l : lights) {
		l->bindUniforms(phongProgram);
	}
	for (const auto& o : opaqueObjects) {
		o->bindUniforms(phongProgram);
		o->draw();
	}
	switch (mode) {
	default:
	case TransparencyMode::alphaBlend:
		renderAlphaBlend();
		break;
	case TransparencyMode::depthPeel:
		renderDepthPeeling();
		break;
	case TransparencyMode::moment:
		renderMBOIT();
		break;
	case TransparencyMode::wavelet:
		renderWavelet();
		break;
	}
}

void Scene::renderAlphaBlend() {
	auto phongProgram = shaderPrograms["phongBlinn"];
	for (const auto& t : transparentObjects) {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		t->bindUniforms(phongProgram);
		t->draw();
	}
}
void Scene::renderDepthPeeling() {
	//TODO
}
void Scene::renderMBOIT() {
	//TODO
}
void Scene::renderWavelet() {
	//TODO
}

void Scene::animate(float dt) {
	glm::vec3 xyzOmega(0.0f, 200.f, 0.0f);
	camera->orbit(glm::vec3(0.0f), xyzOmega*dt);
}

void Scene::seedRNG() {
	std::random_device seed;
	rng = std::mt19937(seed());
}