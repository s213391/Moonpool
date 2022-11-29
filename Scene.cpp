#include "Scene.h"
#include "Instance.h"
#include "gl_core_4_4.h"
#include <glm/gtc/matrix_transform.hpp>

Scene::Scene(Camera* camera, glm::vec2 windowSize, Light& light, glm::vec3 ambientLight)
{
	m_camera = camera;
	m_windowSize = windowSize;
	m_sunlight = light;
	m_ambientLight = ambientLight;
}


Scene::~Scene()
{
	for (auto it = m_instances.begin(); it != m_instances.end(); it++)
		delete* it;
}


void Scene::AddInstance(Instance* instance, int ySign)
{
	m_instances.push_back(instance);

	if (ySign == 1 || ySign == 2)
		m_aboveWater.push_back(instance);
	if (ySign == -1 || ySign == 2)
		m_underWater.push_back(instance);
	if (ySign != 0)
		m_notWater.push_back(instance);
}


void Scene::draw(int ySign, aie::ShaderProgram* tempShader)
{
	//enable gl wire frame rendering
	if (m_wireFrameActive)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//get the pvm for the sunlight
	glm::vec3 lightDirection = glm::normalize(glm::vec3(getLight().direction * -1.0f));
	glm::mat4 lightProjection = glm::ortho<float>(-10, 10, -10, 10, -10, 10);
	glm::mat4 lightView = glm::lookAt(lightDirection, glm::vec3(0), glm::vec3(0, 1, 0));
	m_lightMatrix = lightProjection * lightView;

	glm::mat4 textureSpaceOffset(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	);

	m_offsetLightMatrix = textureSpaceOffset * m_lightMatrix;

	//alway draw all lights
	for (int i = 0; i < MAX_LIGHTS && i < m_pointLights.size(); i++)
	{
		m_pointLightPositions[i] = m_pointLights[i].direction;
		m_pointLightColours[i] = m_pointLights[i].colour;
	}
	
	//draw everything
	if (ySign == 0)
	{
		for (auto it = m_instances.begin(); it != m_instances.end(); it++)
		{
			Instance* instance = *it;
			instance->draw(this, tempShader);
		}
	}
	//draw everything that is above water
	else if (ySign == 1)
	{
		for (auto it = m_aboveWater.begin(); it != m_aboveWater.end(); it++)
		{
			Instance* instance = *it;
			instance->draw(this, tempShader);
		}
	}
	//draw everything that is under water
	else if (ySign == -1)
	{
		for (auto it = m_underWater.begin(); it != m_underWater.end(); it++)
		{
			Instance* instance = *it;
			instance->draw(this, tempShader);
		}
	}
	//draw everything that is not water
	else if (ySign == 2)
	{
		for (auto it = m_notWater.begin(); it != m_notWater.end(); it++)
		{
			Instance* instance = *it;
			instance->draw(this, tempShader);
		}
	}

	//disble gl wire frame rendering before rendering the UI
	if (m_wireFrameActive)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


void Scene::drawRaw(int ySign, aie::ShaderProgram* tempShader)
{
	//draw everything
	if (ySign == 0)
	{
		for (auto it = m_instances.begin(); it != m_instances.end(); it++)
		{
			Instance* instance = *it;
			instance->drawRaw(this, tempShader);
		}
	}
	//draw everything that is above water
	else if (ySign == 1)
	{
		for (auto it = m_aboveWater.begin(); it != m_aboveWater.end(); it++)
		{
			Instance* instance = *it;
			instance->drawRaw(this, tempShader);
		}
	}
	//draw everything that is under water
	else if (ySign == -1)
	{
		for (auto it = m_underWater.begin(); it != m_underWater.end(); it++)
		{
			Instance* instance = *it;
			instance->drawRaw(this, tempShader);
		}
	}
	//draw everything that is not water
	else if (ySign == 2)
	{
		for (auto it = m_notWater.begin(); it != m_notWater.end(); it++)
		{
			Instance* instance = *it;
			instance->drawRaw(this, tempShader);
		}
	}
}