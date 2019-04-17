#include "Mesh.h"

Mesh::Mesh(
	const std::vector<Vertex>& vertices,
	const std::vector<ElementIndex> indices
) {
	VAO = 0;
	numVertices = static_cast<GLsizei>(indices.size());

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	// Set up the VBO for the mesh
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(
		GL_ARRAY_BUFFER,
		vertices.size() * sizeof(Vertex),
		vertices.data(),
		GL_STATIC_DRAW
	);

	// Set up the EBO for the mesh
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(ElementIndex),
		indices.data(),
		GL_STATIC_DRAW
	);

	// Location 0: Vertex Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	glEnableVertexAttribArray(0);

	// Location 1: Vertex Normal
	auto offset = (const GLvoid*)offsetof(Vertex, normal);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offset);
	glEnableVertexAttribArray(1);

	// Location 2: Texture Coordinates
	offset = (const GLvoid*)offsetof(Vertex, texCoords);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offset);
	glEnableVertexAttribArray(2);

	// Location 3: Bone Indices for Weights
	offset = (const GLvoid*)offsetof(Vertex, bones);
	glVertexAttribIPointer();

	// Clean up
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::draw(const Shader &shader) const {
	if (VAO) {
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}
}

Mesh::~Mesh() {
	if (VAO) {
		glDeleteBuffers(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		VAO = 0;
	}
}
