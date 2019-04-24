#pragma once

#include <GL/glew.h>
#include <Shared/Common.h>
#include "Shader.h"
#include "Texture2d.h"

// Contains functions for 2D drawing.
namespace Draw {
	extern GLuint quadVAO, quadVBO;
	extern mat4 projection;
	extern Shader *texturedQuadShader;
	extern Texture2d *whiteTexture;
	extern vec4 color;
		
	void init();
	void cleanUp();
	void setColor(const vec4 &newColor);
	void setColor(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
	void rect(float x, float y, float w, float h, Texture &texture);
	void rect(float x, float y, float w, float h);
}