#pragma once
#include <glm/glm.hpp>

class Mesh
{
public:

	Mesh() : triCount(0), vao(0), vbo(0), ibo(0) {}
	virtual ~Mesh(); //uses openGL delete calls to clear the mesh data

	struct Vertex {
		glm::vec4 position;
		glm::vec4 normal;
		glm::vec2 texCoord;
	};

	//create a quad mesh in the middle of world space
	void initialiseQuad(const unsigned int width, const unsigned int height);
	//create a full-screen quad mesh
	void initialiseFullscreenQuad();

	//set the vertex data of the quad using the given vertices, optional use of indices instead
	void initialise(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount = 0, unsigned int* indices = nullptr);

	virtual void draw();

protected:

	unsigned int triCount;
	unsigned int vao, vbo, ibo;
};

