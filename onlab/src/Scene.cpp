#include "Scene.h"
#include "GPUProgram.h"
#include "Geometry.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#define TESTING

template <typename T> void appendVector(std::vector<T>& target, const std::vector<T>& donor) {
	target.insert(target.end(), donor.begin(), donor.end());
}

bool Scene::set() {
	seedRNG();

	int numOfLights = 4;	//TODO: move to a settings class

	camera = std::make_unique<Camera>();
	camera->translate(glm::vec3(0.0f, 15.0f, 15.0f));
	camera->setViewUp(glm::vec3(0, 1, 0));
	camera->setParams(120.0f);

	setMakeLights(numOfLights);

	if (!setMakeShaderPrograms()) return false;

	if (!setMakeOpaqueObjects()) return false;

	//if (!setMakeTransparentObjects()) return false;

	return true;
}

void Scene::setMakeLights(int numOfLights) {
	if (numOfLights > maxNumOfLights) {
		numOfLights = maxNumOfLights;
		std::cerr << "numOfLights exceeds maximum, count truncated to bounds!" << std::endl;
	}
	auto l = std::make_shared<Light>(0);
	l->translate(glm::vec3(1,2,3));
	l->changeAmbientColor(glm::vec3(0.7f, 0.5f, 0.2f));
	l->changeEmissiveColor(glm::vec3(1));
	lights.push_back(l);

	std::uniform_real_distribution<float> l_pos_rand(-15.0f, 15.0f);
	std::uniform_real_distribution<float> l_L_rand(0.0f, 1.0f);

	for (int i = lights.size(); i < numOfLights; i++){
		l = std::make_shared<Light>(i);
		l->setPointLight();
		l->translate(glm::vec3(l_pos_rand(rng), l_pos_rand(rng), l_pos_rand(rng)));
		l->changeEmissiveColor(glm::vec3(l_L_rand(rng), l_L_rand(rng), l_L_rand(rng)));
		lights.push_back(l);
	}
}

bool Scene::setMakeShaderPrograms() {
	successToggle success(true);
	auto testVS = std::make_shared<Shader>(GL_VERTEX_SHADER);
	auto testFS = std::make_shared<Shader>(GL_FRAGMENT_SHADER);
	auto vertexShader = std::make_shared<Shader>(GL_VERTEX_SHADER);
	auto phongShader = std::make_shared<Shader>(GL_FRAGMENT_SHADER);
	auto depthPeelShader = std::make_shared<Shader>(GL_FRAGMENT_SHADER);
	//auto depthPeelCompositor = std::make_shared<Shader>(GL_FRAGMENT_SHADER);

	success = testVS->create("./shaders/vs_simple.glsl");
	success = testFS->create("./shaders/fs_simple.glsl");
	success = vertexShader->create("./shaders/vs_transform.glsl");
	success = phongShader->create("./shaders/fs_maxblinn.glsl");
	success = depthPeelShader->create("./shaders/depthpeel/fs_maxblinn.glsl");
	//success = depthPeelCompositor->create("./shaders/depthpeel/fs_compositor.glsl");

	if (!success) return false;


	auto testProgram = std::make_shared<GPUProgram>();
	testProgram->addShader(testVS);
	testProgram->addShader(testFS);
	success = testProgram->create("fragColor");

	auto alphaBlendProgram = std::make_shared<GPUProgram>();
	alphaBlendProgram->addShader(vertexShader);
	alphaBlendProgram->addShader(phongShader);
	success = alphaBlendProgram->create("fragColor");

	auto depthPeelProgram = std::make_shared<GPUProgram>();
	depthPeelProgram->addShader(vertexShader);
	depthPeelProgram->addShader(depthPeelShader);
	//depthPeelProgram->addShader(depthPeelCompositor);
	success = depthPeelProgram->create("fragColor");
	if (!success) return false;

	success = shaderPrograms.emplace("test", testProgram).second;
	success = shaderPrograms.emplace("phongBlinn", alphaBlendProgram).second;	//https://en.cppreference.com/w/cpp/container/unordered_map/emplace#Return_value
	success = shaderPrograms.emplace("depthPeel", depthPeelProgram).second;
	return success;
}

bool Scene::setMakeOpaqueObjects() {
#ifndef TESTING
	successToggle success(true);

	auto plane = std::make_shared<Plane>();
	success = plane->create();

	if (!success) return false;

	auto boxMaterial = std::make_shared<Material>();
	boxMaterial->kd = glm::vec3(0.4f, 0.4f, 0.4f);
	boxMaterial->shine = 0.9f;

	auto fullscreenQuad = std::make_unique<Object>(boxMaterial, plane);
	//fullscreenQuad->scale(glm::vec3(2,2,2));
	fullscreenQuad->translate(glm::vec3(0,0,0));
	//fullscreenQuad->rotate(glm::vec3(glm::radians(-45.0f), glm::radians(45.0f), glm::radians(0.0f)));
	opaqueObjects.push_back(std::move(fullscreenQuad));

	//auto fullscreenQuad2 = std::make_unique<Object>(boxMaterial, plane);
	//fullscreenQuad2->scale(glm::vec3(2, 2, 2));
	//fullscreenQuad2->translate(glm::vec3(0, 0, -1));
	//fullscreenQuad2->rotate(glm::vec3(0.5f, 0.5f, 0.0f));
	//opaqueObjects.push_back(std::move(fullscreenQuad2));
	return true;
#else
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
	constexpr float pihalf = glm::radians(90.0f);
	box[0]->rotate(glm::vec3(-pihalf, 0, 0));
	box[1]->rotate(glm::vec3(pihalf, 0, 0));
	box[2]->rotate(glm::vec3(0, -pihalf, 0));
	box[3]->rotate(glm::vec3(0, pihalf, 0));
	box[5]->rotate(glm::vec3(0, 2.0f*pihalf, 0));

	box[0]->translate(glm::vec3(0, 10, 0));
	box[1]->translate(glm::vec3(0, -10, 0));
	box[2]->translate(glm::vec3(10, 0, 0));
	box[3]->translate(glm::vec3(-10, 0, 0));
	box[4]->translate(glm::vec3(0, 0, -10));
	box[5]->translate(glm::vec3(0, 0, 10));

	appendVector(opaqueObjects, box);

	return true;
#endif
}

bool Scene::setMakeTransparentObjects() {
#ifdef TESTING
	successToggle success(true);

	auto sphere = std::make_shared<Sphere>(32, 32);
	success = sphere->create();

	if (!success) return false;

	auto ballMaterial = std::make_shared<Material>();
	ballMaterial->kd = glm::vec3(0.2f, 0.2f, 0.2f);
	ballMaterial->shine = 0.9f;
	auto ball = std::make_unique<Object>(ballMaterial, sphere);
	transparentObjects.push_back(std::move(ball));
	return true;
#else
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
#endif
}

void Scene::render(TransparencyMode mode){
#ifndef TESTING
	auto phongProgram = shaderPrograms["test"];
#else
	auto phongProgram = shaderPrograms["phongBlinn"];
#endif
	phongProgram->activate();

	camera->bindUniforms(phongProgram);
	phongProgram->setUniform("nLights", (int)lights.size());
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
#ifdef TESTING
	auto program = shaderPrograms["test"];
#else
	auto program = shaderPrograms["phongBlinn"];
#endif
	program->activate();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (const auto& t : transparentObjects) {
		t->bindUniforms(program);
		t->draw();
	}
	glBlendFunc(GL_ONE, GL_ZERO);
	glDisable(GL_BLEND);
}
void Scene::renderDepthPeeling() {
	auto program = shaderPrograms["depthPeel"];
	program->activate();
	glDepthFunc(GL_ALWAYS);
	for (const auto& t : transparentObjects) {
		t->bindUniforms(program);
		t->draw();
	}
}
void Scene::renderMBOIT() {
	//TODO
}
void Scene::renderWavelet() {
	//TODO
}

void Scene::animate(float dt) {
	glm::vec3 xyzOmega(0.0f, 1000.0f, 0.0f);
	glm::vec3 lookAtPoint(0.0f, 10.0f, 0.0f);
	camera->orbit(lookAtPoint, xyzOmega*dt);
	camera->lookAt(lookAtPoint);
}

void Scene::seedRNG() {
	std::random_device seed;
	rng = std::mt19937(seed());
}