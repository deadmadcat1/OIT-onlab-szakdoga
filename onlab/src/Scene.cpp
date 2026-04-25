#include "Scene.h"
#include "GPUProgram.h"
#include "Geometry.h"
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#include <math.h>
#endif
#include <GL/glew.h>
#include <iostream>

// #define DRAW_WIREFRAME

template <typename T>
void appendVector(std::vector<T> &target, const std::vector<T> &donor) {
  target.insert(target.end(), donor.begin(), donor.end());
}

bool Scene::set() {
  seedRNG();

  camera = std::make_unique<Camera>();
  camera->translate(_settings.camera.startingPos);

  setMakeLights(_settings.lights.maxAmount);

  if (!setMakeShaderPrograms())
    return false;

  if (!setMakeFramebuffers())
    return false;

  if (!setMakeOpaqueObjects())
    return false;

  if (!setMakeTransparentObjects())
    return false;

  return true;
}

void Scene::setMakeLights(unsigned int numOfLights) {
  if (numOfLights > _settings.lights.maxAmount) {
    numOfLights = _settings.lights.maxAmount;
    std::cerr << "numOfLights exceeds maximum, count truncated to bounds!"
              << std::endl;
  }
  std::shared_ptr<Light> l;

  std::uniform_real_distribution<float> l_pos_rand(_settings.lights.pos.min,
                                                   _settings.lights.pos.max);
  std::uniform_real_distribution<float> l_L_rand(
      _settings.lights.irradiance.min, _settings.lights.irradiance.max);

  for (size_t i = lights.size(); i < numOfLights; i++) {
    l = std::make_shared<Light>((int)i);
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
  auto depthPeel = std::make_shared<Shader>(GL_FRAGMENT_SHADER);
  auto depthPeelCompositor = std::make_shared<Shader>(GL_FRAGMENT_SHADER);
  auto MBOITMomentGen = std::make_shared<Shader>(GL_FRAGMENT_SHADER);
  auto MBOITTransparentPass = std::make_shared<Shader>(GL_FRAGMENT_SHADER);
  auto MBOITCompositor = std::make_shared<Shader>(GL_FRAGMENT_SHADER);

  depthPeel->addDefine("#define DEPTH_PEEL_ENABLED");

  success = fullscreenQuadVS->create("./shaders/vs_FSTQ.glsl");
  success = fullscreenQuadFS->create("./shaders/fs_FSTQ.glsl");
  success = vertexShader->create("./shaders/vs_transform.glsl");
  success = phongShader->create("./shaders/fs_maxblinn.glsl");
  success = depthPeel->create("./shaders/fs_maxblinn.glsl");
  success =
      depthPeelCompositor->create("./shaders/fs_depthpeel_compositor.glsl");
  success = MBOITMomentGen->create("./shaders/fs_mboit_momentgen.glsl");
  success = MBOITTransparentPass->create("./shaders/fs_mboit_transparent.glsl");
  success = MBOITCompositor->create("./shaders/fs_mboit_compositor.glsl");

  if (!success)
    return false;

  auto alphaBlendProgram = std::make_shared<GPUProgram>();
  alphaBlendProgram->addShader(vertexShader);
  alphaBlendProgram->addShader(phongShader);
  success = alphaBlendProgram->create({});

  auto depthPeelProgram = std::make_shared<GPUProgram>();
  depthPeelProgram->addShader(vertexShader);
  depthPeelProgram->addShader(depthPeel);
  success = depthPeelProgram->create({});

  auto depthPeelCompositorProgram = std::make_shared<GPUProgram>();
  depthPeelCompositorProgram->addShader(fullscreenQuadVS);
  depthPeelCompositorProgram->addShader(depthPeelCompositor);
  success =
      depthPeelCompositorProgram->create({"peel1", "peel2", "peel3", "peel4"});
  // TODO: use settings to define depth peel count

  auto MBOITMomentGenProgram = std::make_shared<GPUProgram>();
  MBOITMomentGenProgram->addShader(vertexShader);
  MBOITMomentGenProgram->addShader(MBOITMomentGen);
  success = MBOITMomentGenProgram->create({});

  auto MBOITTransparentPassProgram = std::make_shared<GPUProgram>();
  MBOITTransparentPassProgram->addShader(vertexShader);
  MBOITTransparentPassProgram->addShader(MBOITTransparentPass);
  success = MBOITTransparentPassProgram->create({"moment012", "moment3456"});

  auto MBOITCompositorProgram = std::make_shared<GPUProgram>();
  MBOITCompositorProgram->addShader(fullscreenQuadVS);
  MBOITCompositorProgram->addShader(MBOITCompositor);
  success = MBOITCompositorProgram->create(
      {"opaqueTarget", "transparentTarget", "totalTransmittance"});

  auto FSTQProgram = std::make_shared<GPUProgram>();
  FSTQProgram->addShader(fullscreenQuadVS);
  FSTQProgram->addShader(fullscreenQuadFS);
  success = FSTQProgram->create({"mainPassColor"});
  if (!success)
    return false;

  // https://en.cppreference.com/w/cpp/container/unordered_map/emplace#Return_value
  success = shaderPrograms.emplace("FSTQ", FSTQProgram).second;
  success = shaderPrograms.emplace("phongBlinn", alphaBlendProgram).second;
  success = shaderPrograms.emplace("depthPeel", depthPeelProgram).second;
  success =
      shaderPrograms.emplace("depthPeelCompositor", depthPeelCompositorProgram)
          .second;
  success =
      shaderPrograms.emplace("MBOITMomentGen", MBOITMomentGenProgram).second;
  success = shaderPrograms
                .emplace("MBOITTransparentPass", MBOITTransparentPassProgram)
                .second;
  success =
      shaderPrograms.emplace("MBOITCompositor", MBOITCompositorProgram).second;
  return success;
}

bool Scene::setCreateFramebuffer(
    const std::string &name,
    const std::unordered_map<std::string, TargetParams> &targetParams) {
  auto fbuf = std::make_shared<Framebuffer>();
  return (fbuf->create(targetParams) ? framebuffers.emplace(name, fbuf).second
                                     : false);
}

bool Scene::setMakeFramebuffers() {
  auto rgba =
      TargetParams(_settings.renderResolution.x, _settings.renderResolution.y,
                   GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  auto rgb =
      TargetParams(_settings.renderResolution.x, _settings.renderResolution.y,
                   GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
  auto r =
      TargetParams(_settings.renderResolution.x, _settings.renderResolution.y,
                   GL_RED, GL_RED, GL_UNSIGNED_BYTE, nullptr);
  auto d = TargetParams(_settings.renderResolution.x,
                        _settings.renderResolution.y, GL_DEPTH_COMPONENT,
                        GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
  using FramebufferParameters = struct {
    std::string name;
    std::unordered_map<std::string, TargetParams> params;
  };
	//TODO: ->settings
  auto fbufs = std::vector<FramebufferParameters>(
      {{"finalOutput", {{"mainPassColor", rgba}}},
       {"depthPeelPass1", {{"peel1", rgba}, {"depthBuffer", d}}},
       {"depthPeelPass2", {{"peel2", rgba}, {"depthBuffer", d}}},
       {"depthPeelPass3", {{"peel3", rgba}, {"depthBuffer", d}}},
       {"depthPeelPass4", {{"peel4", rgba}, {"depthBuffer", d}}},
       {"MBOITOpaque", {{"opaqueTarget", rgba}, {"depthBuffer", d}}},
       {"MBOITMoments", {{"moment012", rgb}, {"moment3456", rgba}}},
       {"MBOITTransparent",
        {{"transparentTarget", rgba}, {"totalTransmittance", r}}}});
  for (const FramebufferParameters &fbuf : fbufs) {
    if (!setCreateFramebuffer(fbuf.name, fbuf.params))
      return false;
  }
  return true;
}

bool Scene::setMakeOpaqueObjects() {
  successToggle success(true);

  auto plane = std::make_shared<Plane>();
  success = plane->create();

  if (!success)
    return false;

  fullscreenTexturedQuad = std::make_unique<Object>(plane);

  auto boxMaterial = std::make_shared<Material>();
	//TODO: ->settings
  boxMaterial->kd = glm::vec3(0.2f, 0.2f, 0.2f);
  boxMaterial->shine = 0.9f;

  float boxSize = 20.0f;

  std::vector<std::shared_ptr<Object>> box;
  for (int i = 0; i < 6; i++) {
    auto wall = std::make_unique<Object>(boxMaterial, plane);
	//TODO: ->settings
    wall->scale(glm::vec3(boxSize, boxSize, 1.0f));
    box.push_back(std::move(wall));
  }
  box[0]->rotate(glm::vec3(90.0f, 0.0f, 0.0f));
  box[1]->rotate(glm::vec3(-90.0f, 0.0f, 0.0f));
  box[2]->rotate(glm::vec3(0, -90.0f, 0));
  box[3]->rotate(glm::vec3(0, 90.0f, 0));
  box[5]->rotate(glm::vec3(0, 2.0f * 90.0f, 0));

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

	//TODO: ->settings
  auto sphere = std::make_shared<Sphere>(64, 32);
  success = sphere->create();

  if (!success)
    return false;

  std::vector<std::shared_ptr<Object>> balls;

	//TODO: ->settings
  float maxScale = 2;
  std::uniform_real_distribution<float> size_rand(1.0f, maxScale);
  std::uniform_real_distribution<float> pos_rand(-10.0f, 10.0f);
  std::uniform_real_distribution<float> _0_1_rand(0.0f, 1.0f);
  for (int i = 0; i < 20; i++) {
    auto ballMaterial = std::make_shared<Material>();

    auto color = glm::vec3(_0_1_rand(rng), _0_1_rand(rng), _0_1_rand(rng));
    ballMaterial->kd = color;
    ballMaterial->ka = color;
    ballMaterial->alpha = _0_1_rand(rng) + 0.3f / 2.0f;
    ballMaterial->ks =
        glm::vec3(_0_1_rand(rng), _0_1_rand(rng), _0_1_rand(rng));
    ballMaterial->shine = _0_1_rand(rng);

    auto ball = std::make_unique<Object>(ballMaterial, sphere);
    float scaleFactor = size_rand(rng);
    ball->scale(scaleFactor * glm::vec3(1.0f));
    ball->translate(glm::vec3(pos_rand(rng), pos_rand(rng), pos_rand(rng)));
    balls.push_back(std::move(ball));
  }

  appendVector(transparentObjects, balls);
  return true;
}

void Scene::render(TransparencyMethod mode) {
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
  glViewport(0, 0, _settings.viewportSize.x, _settings.viewportSize.y);
  auto FSTQprogram = shaderPrograms["FSTQ"];
  FSTQprogram->activate();
  framebuffers["finalOutput"]->bindUniforms(FSTQprogram);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  fullscreenTexturedQuad->draw();
}

void Scene::renderAlphaBlend() {
#ifdef DRAW_WIREFRAME
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
  framebuffers["finalOutput"]->bind();

  glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  auto program = shaderPrograms["phongBlinn"];
  program->activate();

  camera->bindUniforms(program);

  program->setUniform("nLights", (int)lights.size());
  for (const auto &l : lights) {
    l->bindUniforms(program);
  }

  glEnable(GL_DEPTH_TEST);
  for (const auto &o : opaqueObjects) {
    o->bindUniforms(program);
    o->draw();
  }
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  for (const auto &t : transparentObjects) {
    t->bindUniforms(program);
    t->draw();
  }
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ZERO);
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
  for (const auto &l : lights) {
    l->bindUniforms(program);
  }

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  auto framebuffer = framebuffers["depthPeelPass1"];
  framebuffer->bind();

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (const auto &o : opaqueObjects) {
    o->bindUniforms(program);
    o->draw();
  }
  for (const auto &t : transparentObjects) {
    t->bindUniforms(program);
    t->draw();
  }
  program = shaderPrograms["depthPeel"];
  program->activate();

  camera->bindUniforms(program);

  program->setUniform("nLights", (int)lights.size());
  for (const auto &l : lights) {
    l->bindUniforms(program);
  }
  for (int i = 1; i < 4 /*settings.depthPeelCount*/; i++) {
    framebuffer->bindUniforms(program);

    framebuffer = framebuffers["depthPeelPass" + std::to_string(i + 1)];
    framebuffer->bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto &o : opaqueObjects) {
      o->bindUniforms(program);
      o->draw();
    }
    for (const auto &t : transparentObjects) {
      t->bindUniforms(program);
      t->draw();
    }
  }
  glEnable(GL_BLEND);
#ifdef DRAW_WIREFRAME
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
  program = shaderPrograms["depthPeelCompositor"];
  program->activate();

  for (int i = 0; i < 4 /*settings.depthPeelCount*/; i++) {
    framebuffers["depthPeelPass" + std::to_string(i + 1)]->bindUniforms(
        program);
    // auto layer =
    //     framebuffers["depthPeelPass" + std::to_string(i +
    //     1)]->getColorTarget(
    //         0);
    // program->setUniform("colorSampler" + std::to_string(i + 1), layer, i +
    // 1);
  }

  framebuffers["finalOutput"]->bind();
  glClearColor(0.0f, 1.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  fullscreenTexturedQuad->draw();
}

void Scene::renderMBOIT() {
  // TODO
#ifdef DRAW_WIREFRAME
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  // Opaque pre-pass
  std::cout << "Beginning MBOIT Opaque pass" << std::endl;
  auto program = shaderPrograms["phongBlinn"];
  program->activate();

  camera->bindUniforms(program);

  program->setUniform("nLights", (int)lights.size());
  for (const auto &l : lights) {
    l->bindUniforms(program);
  }

  auto framebuffer = framebuffers["MBOITOpaque"];
  framebuffer->bind();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  for (const auto &o : opaqueObjects) {
    o->bindUniforms(program);
    o->draw();
  }
  glDisable(GL_DEPTH_TEST);
  // Calc necessary parameter for wrapping zone because of trig moments
  auto wrapping_zone_param = (float)M_PI * 0.95f;
  // Gather moments
  std::cout << "Beginning MBOIT MomentGen pass" << std::endl;
  program = shaderPrograms["MBOITMomentGen"];
  program->activate();

  camera->bindUniforms(program);

  program->setUniform("wrapping_zone_param", wrapping_zone_param);

  framebuffer->bindUniforms(program);
  framebuffer = framebuffers["MBOITMoments"];
  framebuffer->bind();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_BLEND);
  // additive blending enabled
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_ONE, GL_ONE);

  for (const auto &t : transparentObjects) {
    t->bindUniforms(program);
    t->draw();
  }
  // Produce image from moments
  // glBlendFunci(0, GL_ONE, GL_ONE);
  // glBlendFunci(1, GL_ONE, GL_ZERO);
  std::cout << "Beginning MBOIT Transparent pass" << std::endl;
  program = shaderPrograms["MBOITTransparentPass"];
  program->activate();

  camera->bindUniforms(program);

  program->setUniform("nLights", (int)lights.size());
  for (const auto &l : lights) {
    l->bindUniforms(program);
  }
  program->setUniform("wrapping_zone_param", wrapping_zone_param);

  framebuffer->bindUniforms(program);
  framebuffers["MBOITOpaque"]->bindUniforms(program);
  // TODO: only enable zbuffer on FBO's that need to draw to it, bindings are
  // getting confused.
  framebuffer = framebuffers["MBOITTransparent"];
  framebuffer->bind();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (const auto &t : transparentObjects) {
    t->bindUniforms(program);
    t->draw();
  }
  // glDisable(GL_DEPTH_TEST);
  // additive blending disabled
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_ONE, GL_ZERO);
  glDisable(GL_BLEND);
#ifdef DRAW_WIREFRAME
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
  std::cout << "Beginning MBOIT Composite pass" << std::endl;
  program = shaderPrograms["MBOITCompositor"];
  program->activate();
  framebuffers["MBOITOpaque"]->bindUniforms(program);
  framebuffers["MBOITTransparent"]->bindUniforms(program);
  framebuffers["finalOutput"]->bind();

  glClearColor(0.0f, 1.0f, 1.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  fullscreenTexturedQuad->draw();
  glDisable(GL_DEPTH_TEST);
}

void Scene::renderWavelet() {
  // TODO
  glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  fullscreenTexturedQuad->draw();
}

void Scene::animate(float dt) {
  if (cameraOrbitLockoutTimer > 0) {
    cameraOrbitLockoutTimer -= 500.0f * dt;
    return;
  }
  glm::vec3 xyzOmega(0.0f, 210.0f, 0.0f);
  glm::vec3 orbitRootPoint(0.0f, 0.0f, 0.0f);
  camera->orbit(orbitRootPoint, xyzOmega * 100.0f * dt);
  camera->lookAt(orbitRootPoint);
}

void Scene::seedRNG() {
  std::random_device seed;
  rng = std::mt19937(seed());
}

void Scene::notifyResize(glm::uvec2 newsize) {
  camera->setParams(73.0f, (float)newsize.x / (float)newsize.y);
  for (auto &fb : framebuffers) {
    fb.second->resize(newsize);
  }
}

void Scene::panCameraNDC(glm::vec2 dNDC) {
  camera->pan(dNDC);
  cameraOrbitLockoutTimer = _settings.camera.orbitLockout;
}

void Scene::changeCameraPosition(glm::vec3 dpos) {
  camera->translate(dpos);
  cameraOrbitLockoutTimer = _settings.camera.orbitLockout;
}
