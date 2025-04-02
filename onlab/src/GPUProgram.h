#pragma once
#include <glm/glm.hpp>
#include "Shader.h"
#include "Texture.h"
#include <vector>
#include <memory>

class GPUProgram {
	unsigned int progID;
	std::vector<std::unique_ptr<Shader>> shaders;
	void getErrorInfo(unsigned int handle);
	bool checkLinking(unsigned int program);
	int getLocation(const char* const name);
public:
	void addShader(std::unique_ptr<Shader>& shader);

	bool create(const char* const fragOut);

	void activate();

	void setUniform(int i, const char* const name);

	void setUniform(float f, const char* const name);

	void setUniform(const glm::vec2* const v, const char* const name);

	void setUniform(const glm::vec3* const v, const char* const name);

	void setUniform(const glm::vec4* const v, const char* const name);

	void setUniform(const glm::mat4* const mat, const char* const name);

	void setUniform(const Texture* texture,
					const char* const samplerName,
					unsigned int textureUnit = 0);

	~GPUProgram();
};