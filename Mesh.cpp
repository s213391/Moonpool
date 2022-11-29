#include "Mesh.h"
#include <vector>
#include <gl_core_4_4.h>

//uses openGL delete calls to clear the mesh data
Mesh::~Mesh() 
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
}


//create a quad mesh in the middle of world space
void Mesh::initialiseQuad(const unsigned int width, const unsigned int height) 
{
	// check that the mesh is not initialized already 
	assert(vao == 0);

	// generate buffers 
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);

	// bind vertex array aka a mesh wrapper 
	glBindVertexArray(vao);

	// bind vertex buffer 
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	//create a vector of vertices based on given height and width
	
	std::vector<Vertex> vertices(6 * width * height);

	float xOffset = (width - 1.0f) / 2.0f;
	float yOffset = (height - 1.0f) / 2.0f;

	//assign position, normal and texture coordinates of all vertices
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			//find centerpoint of this sub-quad in world space
			glm::vec4 worldQuadCenter = { j - xOffset, 0, i - yOffset, 1 };

			//set corner offsets
			glm::vec4 worldTopLeft      = glm::vec4(-0.5f, 0,  0.5f, 0);
			glm::vec4 worldTopRight     = glm::vec4( 0.5f, 0,  0.5f, 0);
			glm::vec4 worldBottomLeft   = glm::vec4(-0.5f, 0, -0.5f, 0);
			glm::vec4 worldBottomRight  = glm::vec4( 0.5f, 0, -0.5f, 0);
			
			//if odd indexed quad, split triangles top-right to bottom-left
			if ((i + j) % 2 == 1)
			{
				vertices[6 * (i * width + j) + 0].position = worldQuadCenter + worldTopLeft;
				vertices[6 * (i * width + j) + 1].position = worldQuadCenter + worldTopRight;
				vertices[6 * (i * width + j) + 2].position = worldQuadCenter + worldBottomLeft;

				vertices[6 * (i * width + j) + 3].position = worldQuadCenter + worldBottomLeft;
				vertices[6 * (i * width + j) + 4].position = worldQuadCenter + worldTopRight;
				vertices[6 * (i * width + j) + 5].position = worldQuadCenter + worldBottomRight;
			}
			//if even indexed quad, split triangles top-left to bottom-right
			else
			{
				vertices[6 * (i * width + j) + 0].position = worldQuadCenter + worldTopLeft;
				vertices[6 * (i * width + j) + 1].position = worldQuadCenter + worldTopRight;
				vertices[6 * (i * width + j) + 2].position = worldQuadCenter + worldBottomRight;

				vertices[6 * (i * width + j) + 3].position = worldQuadCenter + worldBottomRight;
				vertices[6 * (i * width + j) + 4].position = worldQuadCenter + worldBottomLeft;
				vertices[6 * (i * width + j) + 5].position = worldQuadCenter + worldTopLeft;
			}

			//add up-facing normals for the vertices
			vertices[6 * (i * width + j) + 0].normal = { 0, 1, 0, 0 };
			vertices[6 * (i * width + j) + 1].normal = { 0, 1, 0, 0 };
			vertices[6 * (i * width + j) + 2].normal = { 0, 1, 0, 0 };
			vertices[6 * (i * width + j) + 3].normal = { 0, 1, 0, 0 };
			vertices[6 * (i * width + j) + 4].normal = { 0, 1, 0, 0 };
			vertices[6 * (i * width + j) + 5].normal = { 0, 1, 0, 0 };

			//find top-left point of this quad in texture space
			glm::vec2 textureQuadCenter = { (float)j / (float)width, (float)i / (float)height };

			//set corner offsets
			glm::vec2 textureTopLeft     = glm::vec2(0.0f / width, 0.0f / height);
			glm::vec2 textureTopRight    = glm::vec2(1.0f / width, 0.0f / height);
			glm::vec2 textureBottomLeft  = glm::vec2(0.0f / width, 1.0f / height);
			glm::vec2 textureBottomRight = glm::vec2(1.0f / width, 1.0f / height);

			//if odd indexed quad, split triangles top-right to bottom-left
			if (i + j % 2 == 1)
			{
				vertices[6 * (i * width + j) + 0].texCoord = textureQuadCenter + textureTopLeft;
				vertices[6 * (i * width + j) + 1].texCoord = textureQuadCenter + textureTopRight;
				vertices[6 * (i * width + j) + 2].texCoord = textureQuadCenter + textureBottomLeft;

				vertices[6 * (i * width + j) + 3].texCoord = textureQuadCenter + textureBottomLeft;
				vertices[6 * (i * width + j) + 4].texCoord = textureQuadCenter + textureTopRight;
				vertices[6 * (i * width + j) + 5].texCoord = textureQuadCenter + textureBottomRight;
			}
			//if even indexed quad, split triangles top-left to bottom-right
			else
			{
				vertices[6 * (i * width + j) + 0].texCoord = textureQuadCenter + textureTopLeft;
				vertices[6 * (i * width + j) + 1].texCoord = textureQuadCenter + textureTopRight;
				vertices[6 * (i * width + j) + 2].texCoord = textureQuadCenter + textureBottomRight;

				vertices[6 * (i * width + j) + 3].texCoord = textureQuadCenter + textureBottomRight;
				vertices[6 * (i * width + j) + 4].texCoord = textureQuadCenter + textureBottomLeft;
				vertices[6 * (i * width + j) + 5].texCoord = textureQuadCenter + textureTopLeft;
			}
		}
	}

	// fill vertex buffer 
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
		vertices.data(), GL_STATIC_DRAW);

	// enable first element as position 
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), 0);

	// enable second element as normal 
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE,
		sizeof(Vertex), (void*)16);

	// enable third element as texture 
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), (void*)32);

	// unbind buffers 
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// quad has 2 triangles 
	triCount = 2 * height * width;
}


//create a full-screen quad mesh
void Mesh::initialiseFullscreenQuad() {
	assert(vao == 0);

	// generate buffers 
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);

	// bind vertex array aka a mesh wrapper 
	glBindVertexArray(vao);

	// bind vertex buffer 
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// define vertices 
	float vertices[] = {
	 -1,1, // left top 
	 -1,-1, // left bottom 
	 1,1, // right top 
	 -1,-1, // left bottom 
	 1,-1, // right bottom 
	 1, 1 // right top 
	};

	// fill vertex buffer 
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), vertices,
		GL_STATIC_DRAW);

	// enable first element as position 
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8, 0);

	// unbind buffers 
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// quad has 2 triangles 
	triCount = 2;
}


//set the vertex data of the quad using the given vertices, optional use of indices instead
void Mesh::initialise(unsigned int vertexCount,	const Vertex* vertices,	unsigned int indexCount /* = 0 */, unsigned int* indices /* = nullptr*/) 
{
	assert(vao == 0);

	// generate buffers 
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);

	// bind vertex array aka a mesh wrapper 
	glBindVertexArray(vao);

	// bind vertex buffer 
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// fill vertex buffer 
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex),
		vertices, GL_STATIC_DRAW);

	// enable first element as position 
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), 0);

	// enable second element as normal 
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE,
		sizeof(Vertex), (void*)16);

	// bind indices if there are any 
	if (indexCount != 0) 
	{
		glGenBuffers(1, &ibo);

		// bind vertex buffer 
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		// fill vertex buffer 
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

		triCount = indexCount / 3;
	}
	else 
	{
		triCount = vertexCount / 3;
	}

	// unbind buffers 
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void Mesh::draw() 
{
	glBindVertexArray(vao);

	// using indices or just vertices? 
	if (ibo != 0)
		glDrawElements(GL_TRIANGLES, 3 * triCount,
			GL_UNSIGNED_INT, 0);
	else
		glDrawArrays(GL_TRIANGLES, 0, 3 * triCount);
}