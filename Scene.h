#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <list>

#define MAX_LIGHTS 4

namespace aie
{
	class RenderTarget;
	class ShaderProgram;
}

class Camera;
class Instance;

struct Light
{
	glm::vec3 direction;
	glm::vec3 colour;

	Light() : direction(glm::vec3(0)), colour(glm::vec3(0)) {}
	
	Light(glm::vec3 pos, glm::vec3 col, float intensity) : direction(pos), colour(col* intensity) {}
};

class Scene
{
public:
	Scene(Camera* camera, glm::vec2 windowSize, Light& light, glm::vec3	ambientLight);
	~Scene();

	//adds an instance to the instance lists that are above or below the water level
	void AddInstance(Instance* instance, int ySign);
	void draw(int ySign, aie::ShaderProgram* tempShader = nullptr);
	void drawRaw(int ySign, aie::ShaderProgram* tempShader = nullptr);

	Camera* getCamera() { return m_camera; }
	glm::vec2 getWindowSize() { return m_windowSize; }
	Light& getLight() { return m_sunlight; }
	glm::vec3 getAmbientLight() { return m_ambientLight; }

	void setCamera(Camera* camera) { m_camera = camera; }

	void setTime(float time) { m_time = time; }
	float getTime() { return m_time; }

	int getNumLights() { return (int)m_pointLights.size(); }
	glm::vec3* getPointlightPositions() { return &m_pointLightPositions[0]; }
	glm::vec3* getPointlightColours() { return &m_pointLightColours[0]; }
	std::vector<Light>& getPointLights() { return m_pointLights; }
	glm::mat4 getLightMatrix() { return m_lightMatrix; }
	glm::mat4 getOffsetLightMatrix() { return m_offsetLightMatrix; }
	glm::vec2 getShadowBias() { return glm::vec2(m_shadowBiasMin, m_shadowBiasMax); }

	void setShadowTarget(aie::RenderTarget* shadowTarget) { m_shadowTarget = shadowTarget; }
	aie::RenderTarget* getShadowTarget() { return m_shadowTarget; }
	
	void setWireFrame(bool active) { m_wireFrameActive = active; }

protected:
	Camera* m_camera;
	glm::vec2 m_windowSize;

	Light m_sunlight;
	glm::vec3 m_ambientLight;
	std::vector<Light> m_pointLights;

	glm::mat4 m_lightMatrix = glm::mat4(0);
	glm::mat4 m_offsetLightMatrix = glm::mat4(0);

	float m_time = 0.0f;
	bool m_wireFrameActive = false;

	std::list<Instance*> m_instances;
	std::list<Instance*> m_aboveWater;
	std::list<Instance*> m_underWater;
	std::list<Instance*> m_notWater;

	glm::vec3 m_pointLightPositions[MAX_LIGHTS];
	glm::vec3 m_pointLightColours[MAX_LIGHTS];

	aie::RenderTarget* m_shadowTarget = nullptr;
	const float m_shadowBiasMin = 0.001f;
	const float m_shadowBiasMax = 0.01f;
};

