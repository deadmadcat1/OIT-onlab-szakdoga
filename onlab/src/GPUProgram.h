#pragma once
#include "Shader.h"
#include "Texture.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
#include <memory>

class GPUProgram {
	unsigned int progID = 0;
	std::vector<std::shared_ptr<Shader>> shaders;
	void getErrorInfo();
	bool checkLinking();
	int getLocation(const std::string& name);
public:
	unsigned int getID();

	void addShader(std::shared_ptr<Shader> shader);

	bool create(const std::string& fragOut);

	void activate();

	void setUniform(const std::string& name, int i);

	void setUniform(const std::string& name, float f);

	void setUniform(const std::string& name, const glm::vec2* const v);

	void setUniform(const std::string& name, const glm::vec3* const v);

	void setUniform(const std::string& name, const glm::vec4* const v);

	void setUniform(const std::string& name, const glm::mat4* const mat);

	void setUniform(const std::string& samplerName,
					const Texture* texture,
					unsigned int textureUnit);

	~GPUProgram();
};