#include "Scene.h"
#include "GPUProgram.h"
#include "Geometry.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>

template <typename T> void appendVector(std::vector<T>& target, const std::vector<T>& donor) {
	target.insert(target.end(), donor.begin(), donor.end());
}

bool Scene::set() {
	seedRNG();

	int numOfLights = 4;	//TODO: move to a settings class

	camera = std::make_unique<Camera>();
	camera->translate(glm::vec3(0.0f, 3.0f, 3.0f));

	setMakeLights(numOfLights);

	if (!setMakeShaderPrograms()) return false;

	//if (!setMakeOpaqueObjects()) return false;

	if (!setMakeTransparentObjects()) return false;

	return true;
}

void Scene::setMakeLights(int numOfLights) {
	if (numOfLights > maxNumOfLights) {
		numOfLights = maxNumOfLights;
		std::cerr << "numOfLights exceeds maximum, count truncated to bounds!" << std::endl;
	}
	auto l = std::make_shared<Light>(0);
	l->translate(glm::vec3(1.0f, 2.0f, 3.0f));
	l->setEmissiveColor(glm::vec3(1));
	lights.push_back(l);

	std::uniform_real_distribution<float> l_pos_rand(-15.0f, 15.0f);
	std::uniform_real_distribution<float> l_L_rand(100.0f, 200.0f);

	for (int i = lights.size(); i < numOfLights; i++){
		l = std::make_shared<Light>(i);
		l->setPointLight();
		l->translate(glm::vec3(l_pos_rand(rng), l_pos_rand(rng), l_pos_rand(rng)));
		l->setEmissiveColor(glm::vec3(l_L_rand(rng), l_L_rand(rng), l_L_rand(rng)));
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

	//depthPeelShader->addDefine("#define DEPTH_PEEL_ENABLED");

	success = testVS->create("./shaders/vs_simple.glsl");
	success = testFS->create("./shaders/fs_simple.glsl");
	success = vertexShader->create("./shaders/vs_transform.glsl");
	success = phongShader->create("./shaders/fs_maxblinn.glsl");
	success = depthPeelShader->create("./shaders/fs_maxblinn.glsl");
	//success = depthPeelCompositor->create("./shaders/fs_depthpeel_compositor.glsl");

	if (!success) return false;

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

	success = shaderPrograms.emplace("phongBlinn", alphaBlendProgram).second;	//https://en.cppreference.com/w/cpp/container/unordered_map/emplace#Return_value
	success = shaderPrograms.emplace("depthPeel", depthPeelProgram).second;
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

	float boxSize = 20.0f;

	std::vector<std::shared_ptr<Object>> box;
	for (int i = 0; i < 6; i++)
	{
		auto wall = std::make_unique<Object>(boxMaterial, plane);
		wall->scale(glm::vec3(boxSize, boxSize, 1.0f));
		box.push_back(std::move(wall));
	}
	box[0]->rotate(glm::vec3(90.0f, 0.0f, 0.0f));
	box[1]->rotate(glm::vec3(-90.0f, 0.0f, 0.0f));
	box[2]->rotate(glm::vec3(0, -90.0f, 0));
	box[3]->rotate(glm::vec3(0, 90.0f, 0));
	box[5]->rotate(glm::vec3(0, 2.0f*90.0f, 0));
	
	box[0]->translate(boxSize * glm::vec3(0, 1, 0));
	box[1]->translate(boxSize * glm::vec3(0, -1, 0));
	box[2]->translate(boxSize * glm::vec3(1, 0, 0));
	box[3]->translate(boxSize * glm::vec3(-1, 0, 0));
	box[4]->translate(boxSize * glm::vec3(0, 0, -1));
	box[5]->translate(boxSize * glm::vec3(0, 0, 1));

	appendVector(opaqueObjects, box);

	return true;
}

bool Scene::setMakeTransparentObjects() {
	successToggle success(true);

	auto sphere = std::make_shared<Sphere>(32, 32);
	success = sphere->create();

	if (!success) return false;

	auto ballMaterial = std::make_shared<Material>();

	std::vector<std::shared_ptr<Object>> balls;

	float maxScale = 3;
	std::uniform_real_distribution<float> size_rand(1.0f, maxScale);
	std::uniform_real_distribution<float> pos_rand(-10.0f, 10.0f);
	std::uniform_real_distribution<float> _0_1_rand(0.0f, 1.0f);
	for (int i = 0; i < 20; i++)
	{
		ballMaterial->kd = glm::vec3(_0_1_rand(rng), _0_1_rand(rng), _0_1_rand(rng));
		ballMaterial->alpha = _0_1_rand(rng);
		ballMaterial->ks = glm::vec3(_0_1_rand(rng), _0_1_rand(rng), _0_1_rand(rng));
		ballMaterial->shine = _0_1_rand(rng);

		auto ball = std::make_unique<Object>(ballMaterial, sphere);
		float scaleFactor = size_rand(rng);
		ball->scale(scaleFactor*glm::vec3(1.0f));
		ball->translate(glm::vec3(pos_rand(rng), pos_rand(rng), pos_rand(rng)));
		balls.push_back(std::move(ball));
	}

	appendVector(transparentObjects, balls);

	return true;
}

void Scene::render(TransparencyMethod mode){
	switch (mode) {
	default:
	case TransparencyMethod::alphaBlend:
		renderAlphaBlend();
		break;
	case TransparencyMethod::depthPeel:
		renderDepthPeeling();
		break;
	case TransparencyMethod::moment:
		renderMBOIT();
		break;
	case TransparencyMethod::wavelet:
		renderWavelet();
		break;
	}
}

void Scene::renderAlphaBlend() {
	auto program = shaderPrograms["phongBlinn"];
	program->activate();
	
	camera->bindUniforms(program);

	program->setUniform("nLights", (int)lights.size());
	for (const auto& l : lights) {
		l->bindUniforms(program);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (const auto& o : opaqueObjects) {
		o->bindUniforms(program);
		o->draw();
	}
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

	camera->bindUniforms(program);

	program->setUniform("nLights", (int)lights.size());
	for (const auto& l : lights) {
		l->bindUniforms(program);
	}

	for (const auto& o : opaqueObjects) {
		o->bindUniforms(program);
		o->draw();
	}
	glDepthFunc(GL_ALWAYS);
	for (const auto& t : transparentObjects) {
		t->bindUniforms(program);
		t->draw();
	}
	glDepthFunc(GL_LESS);
}
void Scene::renderMBOIT() {
	//TODO
}
void Scene::renderWavelet() {
	//TODO
}

void Scene::animate(float dt) {
	glm::vec3 xyzOmega(0.0f, 360.0f, 0.0f);
	glm::vec3 lookAtPoint(0.0f, 0.0f, 0.0f);
	camera->lookAt(lookAtPoint);
	camera->orbit(lookAtPoint, xyzOmega*100.0f*dt);
}

void Scene::seedRNG() {
	std::random_device seed;
	rng = std::mt19937(seed());
}