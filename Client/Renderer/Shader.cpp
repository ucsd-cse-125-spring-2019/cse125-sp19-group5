#include "Shader.h"
#include <fstream>
#include <string>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

// File extensions for the vertex and fragment shader source code.
constexpr auto EXT_VERT = ".vert";
constexpr auto EXT_FRAG = ".frag";

// Buffer size for errors.
constexpr auto LOG_BUF_SIZE = 256;

void Shader::loadShader(GLuint &shader, const std::string& path) {
	// Read the file corresponding to path.
	std::ifstream file(path);
	std::string source(
		(std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>()
	);
	auto sourceStr = source.c_str();

	// Then compile the source code that was read.
	glShaderSource(shader, 1, &sourceStr, nullptr);
	glCompileShader(shader);

	// Report errors if any.
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		char buf[LOG_BUF_SIZE];
		glGetShaderInfoLog(shader, sizeof(buf), nullptr, buf);
		std::cerr << "Failed to compile shader " << path << std::endl
			<< buf << std::endl;
	}
}

void Shader::loadProgram(const std::string& path) {
	if (vert == 0 || frag == 0) {
		std::cerr << "Shader::loadProgram called before shaders were loaded"
			<< std::endl;
		return;
	}

	// Create a shader program and link it with the loaded vertex/frag shaders.
	program = glCreateProgram();
	glAttachShader(program, vert);
	glAttachShader(program, frag);
	glBindFragDataLocation(program, 0, "fragColor");
	glLinkProgram(program);

	// Report errors if any.
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		char buf[LOG_BUF_SIZE];
		glGetProgramInfoLog(program, sizeof(buf), nullptr, buf);
		std::cerr << "Failed to link program " << path << std::endl
			<< buf << std::endl;
	}

	// Clean up the vertex/frag shader data since we don't need it anymore.
	glDeleteShader(vert); vert = 0;
	glDeleteShader(frag); frag = 0;
}

Shader::Shader(const std::string& path) {
	vert = glCreateShader(GL_VERTEX_SHADER);
	frag = glCreateShader(GL_FRAGMENT_SHADER);

	loadShader(vert, path + EXT_VERT);
	loadShader(frag, path + EXT_FRAG);
	loadProgram(path);
}

Shader::~Shader() {
	if (program) {
		glDeleteProgram(program);
		program = 0;
	}
}

GLint Shader::getLocation(const char *name) {
	GLint location;
	auto cached = locations.find(name);
	if (cached == locations.end()) {
		location = glGetUniformLocation(program, name);
		locations[name] = location;
	} else {
		location = cached->second;
	}
	return location;
}

void Shader::setUniform(const char *name, GLint v0) {
	glUniform1i(getLocation(name), v0);
}

void Shader::setUniform(const char *name, GLfloat v0) {
	glUniform1f(getLocation(name), v0);
}

void Shader::setUniform(const char *name, const vec3 &v) {
	glUniform3fv(getLocation(name), 1, glm::value_ptr(v));
}

void Shader::setUniform(const char *name, const vec4 &v) {
	glUniform4fv(getLocation(name), 1, glm::value_ptr(v));
}

void Shader::setUniform(const char *name, const mat3 &m) {
	glUniformMatrix3fv(getLocation(name), 1, GL_FALSE, glm::value_ptr(m));
}

void Shader::setUniform(const char *name, const mat4 &m) {
	glUniformMatrix4fv(getLocation(name), 1, GL_FALSE, glm::value_ptr(m));
}

void Shader::use() const {
	glUseProgram(program);
}
