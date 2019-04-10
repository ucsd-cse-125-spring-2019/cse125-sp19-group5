#pragma once

#include <GL/glew.h>
#include <vector>
#include "Common.h"
#include "Shader.h"

// Type of number used for indices in the element buffer.
typedef unsigned int ElementIndex;

// Container for the position vector and normal vector of a vertex.
struct Vertex {
	vec3 pos;
	vec3 normal;
	vec2 texCoords;
};

// The Mesh class is used to draw a collection of vertices using triangles in the
// order given by indices.
class Mesh {
	private:
	GLuint VAO, VBO, EBO;
	GLsizei numVertices;

	public:
	Mesh(
		const std::vector<Vertex> &vertices,
		const std::vector<ElementIndex> indices
	);
	~Mesh();

	void draw(const Shader &shader) const;
};

