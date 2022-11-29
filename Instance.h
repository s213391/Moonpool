#pragma once
#include <glm/glm.hpp>

namespace aie
{
	class OBJMesh;
	class ShaderProgram;
	class Texture;
	class RenderTarget;
}
class Scene;
class Mesh;
struct Light;


class Instance
{
public:
	//constructors for OBJMeshes
	Instance(glm::mat4 transform, aie::OBJMesh* OBJmesh, aie::ShaderProgram* shader, aie::Texture* texture = nullptr, aie::RenderTarget* renderTarget1 = nullptr, aie::RenderTarget* renderTarget2 = nullptr);
	Instance(glm::vec3 position, glm::vec3 eulerAngles, glm::vec3 scale, aie::OBJMesh* OBJmesh, aie::ShaderProgram* shader, aie::Texture* texture = nullptr, aie::RenderTarget* renderTarget1 = nullptr, aie::RenderTarget* renderTarget2 = nullptr);
	//constructors for Meshes
	Instance(glm::mat4 transform, Mesh* mesh, aie::ShaderProgram* shader, aie::Texture* texture = nullptr, aie::RenderTarget* renderTarget1 = nullptr, aie::RenderTarget* renderTarget2 = nullptr);
	Instance(glm::vec3 position, glm::vec3 eulerAngles, glm::vec3 scale, Mesh* mesh, aie::ShaderProgram* shader, aie::Texture* texture = nullptr, aie::RenderTarget* renderTarget1 = nullptr, aie::RenderTarget* renderTarget2 = nullptr);

	//draws the instanced object with the given shader and lighting
	void draw(Scene* scene, aie::ShaderProgram* tempShader = nullptr);
	//draws the instanced object without binding only the pvm
	void drawRaw(Scene* scene, aie::ShaderProgram* tempShader = nullptr);
	//creates a mat4 transform from given values
	glm::mat4 makeTransform(glm::vec3 position, glm::vec3 eulerAngles, glm::vec3 scale);

	//swaps the attached shader
	void swapShader(aie::ShaderProgram* newShader) { m_shader = newShader; }

	//adds material lighting data
	void addMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float specularPower);
	//adds parameters for shaders to use
	void setDimensions(int dimensions);

	glm::vec3 m_ambient = glm::vec3(0);
	glm::vec3 m_diffuse = glm::vec3(0);
	glm::vec3 m_specular = glm::vec3(0);
	float m_specularPower = 0.0f;

protected:
	glm::mat4 m_transform;
	Mesh* m_mesh;
	aie::OBJMesh* m_OBJmesh;
	aie::ShaderProgram* m_shader;
	aie::Texture* m_texture;
	aie::RenderTarget* m_renderTarget1;
	aie::RenderTarget* m_renderTarget2;

	
	bool m_materialManualLoad = false;

	int m_dimensions = 1;
};

