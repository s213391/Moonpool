#pragma once
#include <glm/glm.hpp>

class Camera
{
public:
	Camera(glm::vec3 position, float theta, float phi) : m_position(position), m_theta(theta), m_phi(phi) {};
	~Camera() {};

	//returns the forward matrix of the camera
	glm::mat4 getViewMatrix();
	//returns the perspective matrix of the camera
	glm::mat4 getProjectionMatrix(float w, float h);
	//returns the cameras position
	glm::vec3 getPosition() { return m_position; };

	//updates the camera position based on mouse and keyboard input
	void update(float deltaTime);

	float getTheta() { return m_theta; }
	float getPhi() { return m_phi; }

	void setTheta(float theta);
	void setPhi(float phi);

	void setPosition(glm::vec3 position) { m_position = position; }

private:
	float m_theta;
	float m_phi;
	glm::vec3 m_position;

	float m_lastMouseX = 0.0f;
	float m_lastMouseY = 0.0f;

	int m_invertMouseX = -1;
	int m_invertMouseY = -1;

	const float m_maxCameraAngle = 70.0f;
	const float m_turnSpeed = 0.1f;
	const float m_movementSpeed = 1.0f;
};