#pragma once
#include <GL/glew.h>
#include <string>

using TextureParams = struct TextureParams {
  GLenum _min_filter;
  GLenum _mag_filter;
  GLenum _wrap_s;
  GLenum _wrap_t;
  GLenum _wrap_r;

  TextureParams(GLenum min_filter = GL_LINEAR_MIPMAP_LINEAR,
                GLenum mag_filter = GL_LINEAR, GLenum wrap_s = GL_REPEAT,
                GLenum wrap_t = GL_REPEAT, GLenum wrap_r = GL_REPEAT)
      : _min_filter(min_filter), _mag_filter(mag_filter), _wrap_s(wrap_s),
        _wrap_t(wrap_t), _wrap_r(wrap_r) {}
};

class Texture {
protected:
  unsigned int textureID = 0;
public:
  unsigned int getID();
  unsigned int getID() const;
  bool create(const std::string &path, const TextureParams opt);
  bool create(GLenum target, unsigned int mipmapLevels, GLenum internalFormat, unsigned int width, unsigned int height, unsigned int depth, const TextureParams opt = {});
  ~Texture();
};
