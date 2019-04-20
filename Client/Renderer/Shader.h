#pragma once

#include <GL/glew.h>
#include <string>
#include <unordered_map>
#include <Shared/Common.h>

// The Shader class is a shader program. It handles the loading and usage of/
// shader programs.
class Shader {
	private:
	GLuint vert = 0;    // ID for the vertex shader
	GLuint frag = 0;    // ID for the fragment shader
	GLuint program = 0; // ID for the shader program
	std::unordered_map<std::string, GLint> locations; // Uniform location cache

	// Loads a specific shader given a shader type and path for the source.
	// This is used to load the vertex + fragment shaders.
	void loadShader(GLuint &shader, const std::string &path);

	// After shaders are loaded, sets up a shader program.
	void loadProgram(const std::string &path);

	// A memoized version of glGetUniformLocation.
	GLint getLocation(const char *name);

	public:
	Shader(const std::string &path);
	~Shader();

	// Various ways to set uniforms.
	void setUniform(const char *name, GLint v0);
	void setUniform(const char *name, GLfloat v0);
	void setUniform(const char *name, const vec3 &v);
	void setUniform(const char *name, const vec4 &v);
	void setUniform(const char *name, const mat3 &m);
	void setUniform(const char *name, const mat4 &m);
	void setUniform(const char *name, bool v);

	// Sets the current shader program to the one this class represents.
	void use() const;
};
