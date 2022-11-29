#include "Camera.h"
#include "Input.h"
#include <glm/gtc/matrix_transform.hpp>


//returns the forward matrix of the camera
glm::mat4 Camera::getViewMatrix()
{
	float thetaR = glm::radians(m_theta);
	float phiR = glm::radians(m_phi);
	glm::vec3 forward(cos(phiR) * cos(thetaR), sin(phiR), cos(phiR) * sin(thetaR));
	return glm::lookAt(m_position, m_position + forward, glm::vec3(0, 1, 0));
}


//returns the perspective matrix of the camera
glm::mat4 Camera::getProjectionMatrix(float w, float h)
{
	return glm::perspective(glm::pi<float>() * 0.25f, w / h, 0.1f, 1000.f);
}


//updates the camera position based on mouse and keyboard input
void Camera::update(float deltaTime)
{
	aie::Input* input = aie::Input::getInstance();
	float thetaR = glm::radians(m_theta);
	float phiR = glm::radians(m_phi);

	//calculate the forwards and right axes and up axis for the camera 
	glm::vec3 forward(cos(phiR) * cos(thetaR), sin(phiR), cos(phiR) * sin(thetaR));
	glm::vec3 right(-sin(thetaR), 0, cos(thetaR));
	glm::vec3 up(0, 1, 0);

	float movementSpeed = m_movementSpeed;

	//hold shift to increase camera speed
	if (input->isKeyDown(aie::INPUT_KEY_LEFT_SHIFT))
		movementSpeed *= 5.0f;

	// use WASD, ZX keys to move camera around 
	if (input->isKeyDown(aie::INPUT_KEY_W))
		m_position += movementSpeed * forward * deltaTime;
	if (input->isKeyDown(aie::INPUT_KEY_S))
		m_position += movementSpeed * -forward * deltaTime;
	if (input->isKeyDown(aie::INPUT_KEY_A))
		m_position += movementSpeed * -right * deltaTime;
	if (input->isKeyDown(aie::INPUT_KEY_D))
		m_position += movementSpeed * right * deltaTime;
	if (input->isKeyDown(aie::INPUT_KEY_Z))
		m_position += movementSpeed * -up * deltaTime;
	if (input->isKeyDown(aie::INPUT_KEY_X))
		m_position += movementSpeed * up * deltaTime;

	// get the current mouse coordinates 
	float mx = (float)input->getMouseX();
	float my = (float)input->getMouseY();

	// if the right button is down, increment theta and phi 
	if (input->isMouseButtonDown(aie::INPUT_MOUSE_BUTTON_RIGHT))
	{
		m_theta += m_invertMouseX * glm::clamp(m_turnSpeed * (mx - m_lastMouseX), -m_maxCameraAngle, m_maxCameraAngle);
		m_phi += m_invertMouseY * glm::clamp(m_turnSpeed * (my - m_lastMouseY), -m_maxCameraAngle, m_maxCameraAngle);
	}

	// store this frames values for next frame 
	m_lastMouseX = mx;
	m_lastMouseY = my;
}


void Camera::setTheta(float theta)
{
	m_theta = glm::clamp(theta, -m_maxCameraAngle, m_maxCameraAngle);
}



void Camera::setPhi(float phi)
{
	m_phi = glm::clamp(phi, -m_maxCameraAngle, m_maxCameraAngle);
}
