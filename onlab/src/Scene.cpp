#include "Scene.h"
#include "GPUProgram.h"
#include "Geometry.h"
#include <GL/glew.h>
#include <iostream>

//#define DRAW_WIREFRAME

template <typename T> void appendVector(std::vector<T>& target, const std::vector<T>& donor) {
	target.insert(target.end(), donor.begin(), donor.end());
}

bool Scene::set() {
	seedRNG();

	int numOfLights = 8;	//TODO: move to a settings class

	camera = std::make_unique<Camera>();
	camera->translate(glm::vec3(0.0f, 8.0f, 20.0f));

	setMakeLights(numOfLights);

	if (!setMakeShaderPrograms()) return false;

	if (!setMakeFramebuffers()) return false;

	if (!setMakeOpaqueObjects()) return false;

	if (!setMakeTransparentObjects()) return false;

	return true;
}

void Scene::setMakeLights(int numOfLights) {
	if (numOfLights > maxNumOfLights) {
		numOfLights = maxNumOfLights;
		std::cerr << "numOfLights exceeds maximum, count truncated to bounds!" << std::endl;
	}
	std::shared_ptr<Light> l;

	std::uniform_real_distribution<float> l_pos_rand(-15.0f, 15.0f);
	std::uniform_real_distribution<float> l_L_rand(40.0f, 50.0f);

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
	auto fullscreenQuadVS = std::make_shared<Shader>(GL_VERTEX_SHADER);
	auto fullscreenQuadFS = std::make_shared<Shader>(GL_FRAGMENT_SHADER);
	auto vertexShader = std::make_shared<Shader>(GL_VERTEX_SHADER);
	auto phongShader = std::make_shared<Shader>(GL_FRAGMENT_SHADER);
	auto depthPeelShader = std::make_shared<Shader>(GL_FRAGMENT_SHADER);
	auto depthPeelCompositor = std::make_shared<Shader>(GL_FRAGMENT_SHADER);

	depthPeelShader->addDefine("#define DEPTH_PEEL_ENABLED");

	success = fullscreenQuadVS->create("./shaders/vs_FSTQ.glsl");
	success = fullscreenQuadFS->create("./shaders/fs_FSTQ.glsl");
	success = vertexShader->create("./shaders/vs_transform.glsl");
	success = phongShader->create("./shaders/fs_maxblinn.glsl");
	success = depthPeelShader->create("./shaders/fs_maxblinn.glsl");
	success = depthPeelCompositor->create("./shaders/fs_depthpeel_compositor.glsl");

	if (!success) return false;

	auto alphaBlendProgram = std::make_shared<GPUProgram>();
	alphaBlendProgram->addShader(vertexShader);
	alphaBlendProgram->addShader(phongShader);
	success = alphaBlendProgram->create("fragColor");

	auto depthPeelProgram = std::make_shared<GPUProgram>();
	depthPeelProgram->addShader(vertexShader);
	depthPeelProgram->addShader(depthPeelShader);
	success = depthPeelProgram->create("fragColor");

	auto depthPeelCompositeProgram = std::make_shared<GPUProgram>();
	depthPeelCompositeProgram->addShader(fullscreenQuadVS);
	depthPeelCompositeProgram->addShader(depthPeelCompositor);
	success = depthPeelCompositeProgram->create("fragColor");

	auto FSTQProgram = std::make_shared<GPUProgram>();
	FSTQProgram->addShader(fullscreenQuadVS);
	FSTQProgram->addShader(fullscreenQuadFS);
	success = FSTQProgram->create("fragColor");
	if (!success) return false;

	success = shaderPrograms.emplace("FSTQ", FSTQProgram).second;
	success = shaderPrograms.emplace("phongBlinn", alphaBlendProgram).second;	//https://en.cppreference.com/w/cpp/container/unordered_map/emplace#Return_value
	success = shaderPrograms.emplace("depthPeel", depthPeelProgram).second;
	success = shaderPrograms.emplace("depthPeelComposite", depthPeelCompositeProgram).second;
	return success;
}

bool Scene::setCreateFramebuffer(std::string n, int tc, int w, int h) {
	auto fbuf = std::make_shared<Framebuffer>();
	return (fbuf->create(tc, w, h) ? framebuffers.emplace(n, fbuf).second : false);
}

bool Scene::setMakeFramebuffers() {
	successToggle success(true);
	/*TODO: settings.renderResolution.w & settings.renderResolution.h*/
	success = setCreateFramebuffer("finalOutput", 1, 1920, 1080);
	success = setCreateFramebuffer("depthPeelPass1", 1, 1920, 1080);
	success = setCreateFramebuffer("depthPeelPass2", 1, 1920, 1080);
	success = setCreateFramebuffer("depthPeelPass3", 1, 1920, 1080);
	success = setCreateFramebuffer("depthPeelPass4", 1, 1920, 1080);
	return success;
}

bool Scene::setMakeOpaqueObjects() {
	successToggle success(true);

	auto plane = std::make_shared<Plane>();
	success = plane->create();

	if (!success) return false;

	fullscreenTexturedQuad = std::make_unique<Object>(plane);

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

	auto sphere = std::make_shared<Sphere>(64, 32);
	success = sphere->create();

	if (!success) return false;

	std::vector<std::shared_ptr<Object>> balls;
	
	float maxScale = 2;
	std::uniform_real_distribution<float> size_rand(1.0f, maxScale);
	std::uniform_real_distribution<float> pos_rand(-10.0f, 10.0f);
	std::uniform_real_distribution<float> _0_1_rand(0.0f, 1.0f);
	for (int i = 0; i < 20; i++)
	{
		auto ballMaterial = std::make_shared<Material>();

		auto color = glm::vec3(_0_1_rand(rng), _0_1_rand(rng), _0_1_rand(rng));
		ballMaterial->kd = color;
		ballMaterial->ka = color;
		ballMaterial->alpha = _0_1_rand(rng) + 0.3f / 2.0f;
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
	auto FSTQprogram = shaderPrograms["FSTQ"];
	auto compositeProgram = shaderPrograms["depthPeelComposite"];

	switch (mode) {
	default:
	case TransparencyMethod::alphaBlend:
		renderAlphaBlend();
		break;
	case TransparencyMethod::depthPeel:
		renderDepthPeeling();
		compositeProgram->activate();
		for (int i = 0; i < 4/*settings.depthPeelCount*/; i++) {
			auto layer = framebuffers["depthPeelPass" + std::to_string(i + 1)]->getColorTarget(0);
			compositeProgram->setUniform("colorSampler" + std::to_string(i + 1), layer, i + 1);
		}
		
		framebuffers["finalOutput"]->bind();
		glClearColor(1.0f, 3.0f, 2.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		fullscreenTexturedQuad->draw();
		break;
	case TransparencyMethod::moment:
		renderMBOIT();
		break;
	case TransparencyMethod::wavelet:
		renderWavelet();
		break;
	}

	FSTQprogram->activate();
	framebuffers["finalOutput"]->bindUniforms(FSTQprogram);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	fullscreenTexturedQuad->draw();
}

void Scene::renderAlphaBlend() {
#ifdef DRAW_WIREFRAME
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
	framebuffers["finalOutput"]->bind();

	glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto program = shaderPrograms["phongBlinn"];
	program->activate();
	
	camera->bindUniforms(program);

	program->setUniform("nLights", (int)lights.size());
	for (const auto& l : lights) {
		l->bindUniforms(program);
	}
	
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	for (const auto& o : opaqueObjects) {
		o->bindUniforms(program);
		o->draw();
	}
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (const auto& t : transparentObjects) {
		t->bindUniforms(program);
		t->draw();
	}
	glDisable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#ifdef DRAW_WIREFRAME
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
}
void Scene::renderDepthPeeling() {
#ifdef DRAW_WIREFRAME
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
	auto program = shaderPrograms["phongBlinn"];
	program->activate();

	camera->bindUniforms(program);

	program->setUniform("nLights", (int)lights.size());
	for (const auto& l : lights) {
		l->bindUniforms(program);
	}
	
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	auto framebuffer = framebuffers["depthPeelPass1"];
	framebuffer->bind();

	glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (const auto& o : opaqueObjects) {
		o->bindUniforms(program);
		o->draw();
	}
	for (const auto& t : transparentObjects) {
		t->bindUniforms(program);
		t->draw();
	}
	program = shaderPrograms["depthPeel"];
	program->activate();

	camera->bindUniforms(program);

	program->setUniform("nLights", (int)lights.size());
	for (const auto& l : lights) {
		l->bindUniforms(program);
	}
	for (int i = 1; i < 4/*settings.depthPeelCount*/; i++) {
		framebuffer->bindUniforms(program);
	
		framebuffer = framebuffers["depthPeelPass" + std::to_string(i + 1)];
		framebuffer->bind();
	
		glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		for (const auto& o : opaqueObjects) {
			o->bindUniforms(program);
			o->draw();
		}
		for (const auto& t : transparentObjects) {
			t->bindUniforms(program);
			t->draw();
		}
	}
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#ifdef DRAW_WIREFRAME
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
}
void Scene::renderMBOIT() {
	//TODO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Scene::renderWavelet() {
	//TODO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::animate(float dt) {
	if (cameraOrbitLockoutTimer > 0) {
		cameraOrbitLockoutTimer -= 500.0f * dt;
		return;
	}
	glm::vec3 xyzOmega(0.0f, 210.0f, 0.0f);
	glm::vec3 lookAtPoint(0.0f, 0.0f, 0.0f);
	camera->lookAt(lookAtPoint);
	camera->orbit(lookAtPoint, xyzOmega*100.0f*dt);
}

void Scene::seedRNG() {
	std::random_device seed;
	rng = std::mt19937(seed());
}

void Scene::notifyResize(int w, int h) {
	camera->setParams(73.0f, (float)w / (float)h);
	for (auto& fb : framebuffers) {
		fb.second->resize(w, h);
	}
}

void Scene::changeCameraAttitudeNDC(double dNDCy, double dNDCx) {
	camera->rotateNDC(dNDCy, dNDCx);
	cameraOrbitLockoutTimer = 1.0f;
}

void Scene::changeCameraPosition(glm::vec3 dpos) {
	camera->translate(dpos);
	cameraOrbitLockoutTimer = 1.0f;
}