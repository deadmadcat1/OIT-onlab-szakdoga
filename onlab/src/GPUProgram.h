#pragma once
#include "Shader.h"
#include "Texture.h"
#include <glm/glm.hpp>
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

	void setUniform(int i, const std::string& name);

	void setUniform(float f, const std::string& name);

	void setUniform(const glm::vec2* const v, const std::string& name);

	void setUniform(const glm::vec3* const v, const std::string& name);

	void setUniform(const glm::vec4* const v, const std::string& name);

	void setUniform(const glm::mat4* const mat, const std::string& name);

	void setUniform(const Texture* texture,
					const std::string& samplerName,
					unsigned int textureUnit = 0);

	~GPUProgram();
};