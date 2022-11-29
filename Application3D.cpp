#include "Application3D.h"
#include "Gizmos.h"
#include "Input.h"
#include "Camera.h"
#include "Instance.h"
#include "Scene.h"
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <gl_core_4_4.h>
#include <iostream>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

Application3D::Application3D() {
	m_camera = nullptr;
	m_scene = nullptr;
}


Application3D::~Application3D() {

}


bool Application3D::startup() {
	
	setBackgroundColour(0.2f, 0.2f, 0.2f);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	//create the virtual camera
	m_camera = new Camera(glm::vec3(10, 10, 10), -135.0f, -34.0f);

	//create the scene lighting
	Light sunlight = Light(vec3(0.25f, -0.25f, 0.25f), vec3(0.5f, 0.5f, 0.5f), 1);

	//create the scene
	m_scene = new Scene(m_camera, glm::vec2(getWindowWidth(), getWindowHeight()), sunlight, glm::vec3(0.25f, 0.25f, 0.25f));

	//coloured lights on all four corners 
	m_scene->getPointLights().push_back(Light(vec3(+4.7, 0.5, +4.7), vec3(1, 1, 0), 15));
	m_scene->getPointLights().push_back(Light(vec3(+4.7, 0.5, -4.7), vec3(1, 0, 0), 15));
	m_scene->getPointLights().push_back(Light(vec3(-4.7, 0.5, +4.7), vec3(0, 1, 0), 15));
	m_scene->getPointLights().push_back(Light(vec3(-4.7, 0.5, -4.7), vec3(0, 0, 1), 15));

	//create a render target for reflection
	if (m_reflectionTarget.initialise(1, getWindowWidth(), getWindowHeight()) == false) {
		printf("Reflection Target Error!\n");
		return false;
	}

	//create a render target for refraction
	if (m_refractionTarget.initialise(1, getWindowWidth(), getWindowHeight()) == false) {
		printf("Refraction Target Error!\n");
		return false;
	}

	//create a render target for post processing
	if (m_postTarget.initialise(1, getWindowWidth(), getWindowHeight()) == false) {
		printf("Post Target Error!\n");
		return false;
	}

	//create a render target for shadow generation
	if (m_shadowTarget.initialise(1, 2048, 2048, true) == false) {
		printf("Shadow Target Error!\n");
		return false;
	}
	m_scene->setShadowTarget(&m_shadowTarget);

	//load simple shader
	m_simpleShader.loadShader(aie::eShaderStage::VERTEX,
		"./shaders/simple.vert");
	m_simpleShader.loadShader(aie::eShaderStage::FRAGMENT,
		"./shaders/simple.frag");
	if (m_simpleShader.link() == false) {
		printf("Simple Shader Error: %s\n", m_simpleShader.getLastError());
		return false;
	}

	//load phong shader
	m_phongShader.loadShader(aie::eShaderStage::VERTEX,
		"./shaders/phong.vert");
	m_phongShader.loadShader(aie::eShaderStage::FRAGMENT,
		"./shaders/phong.frag");
	if (m_phongShader.link() == false) {
		printf("Phong Shader Error: %s\n", m_phongShader.getLastError());
		return false;
	}

	//load textured shader
	m_texturedShader.loadShader(aie::eShaderStage::VERTEX,
		"./shaders/textured.vert");
	m_texturedShader.loadShader(aie::eShaderStage::FRAGMENT,
		"./shaders/textured.frag");
	if (m_texturedShader.link() == false) {
		printf("Textured Shader Error: %s\n", m_texturedShader.getLastError());
		return false;
	}

	//load normal map shader
	m_normalMapShader.loadShader(aie::eShaderStage::VERTEX,
		"./shaders/normalmap.vert");
	m_normalMapShader.loadShader(aie::eShaderStage::FRAGMENT,
		"./shaders/normalmap.frag");
	if (m_normalMapShader.link() == false) {
		printf("Normal Map Shader Error: %s\n", m_normalMapShader.getLastError());
		return false;
	}

	//load screen space shader
	m_screenSpaceShader.loadShader(aie::eShaderStage::VERTEX,
		"./shaders/screenSpace.vert");
	m_screenSpaceShader.loadShader(aie::eShaderStage::FRAGMENT,
		"./shaders/screenSpace.frag");
	if (m_screenSpaceShader.link() == false) {
		printf("Screen Space Shader Error: %s\n", m_screenSpaceShader.getLastError());
		return false;
	}

	//load post shader
	m_postShader.loadShader(aie::eShaderStage::VERTEX,
		"./shaders/post.vert");
	m_postShader.loadShader(aie::eShaderStage::FRAGMENT,
		"./shaders/post.frag");
	if (m_postShader.link() == false) {
		printf("Post Shader Error: %s\n", m_postShader.getLastError());
		return false;
	}

	//load box blur post processing shader
	m_boxBlurShader.loadShader(aie::eShaderStage::VERTEX,
		"./shaders/post.vert");
	m_boxBlurShader.loadShader(aie::eShaderStage::FRAGMENT,
		"./shaders/boxBlur.frag");
	if (m_boxBlurShader.link() == false) {
		printf("Box Blur Post Shader Error: %s\n", m_boxBlurShader.getLastError());
		return false;
	}

	//load sharpen post processing shader
	m_sharpenShader.loadShader(aie::eShaderStage::VERTEX,
		"./shaders/post.vert");
	m_sharpenShader.loadShader(aie::eShaderStage::FRAGMENT,
		"./shaders/sharpen.frag");
	if (m_sharpenShader.link() == false) {
		printf("Sharpen Post Shader Error: %s\n", m_sharpenShader.getLastError());
		return false;
	}

	//load chromatic aberration post processing shader
	m_chromaticShader.loadShader(aie::eShaderStage::VERTEX,
		"./shaders/post.vert");
	m_chromaticShader.loadShader(aie::eShaderStage::FRAGMENT,
		"./shaders/chromatic.frag");
	if (m_chromaticShader.link() == false) {
		printf("Chromatic Post Shader Error: %s\n", m_chromaticShader.getLastError());
		return false;
	}

	//load depth buffer shader
	m_depthShader.loadShader(aie::eShaderStage::VERTEX,
		"./shaders/post.vert");
	m_depthShader.loadShader(aie::eShaderStage::FRAGMENT,
		"./shaders/depthBuffer.frag");
	if (m_depthShader.link() == false) {
		printf("Depth Shader Error: %s\n", m_depthShader.getLastError());
		return false;
	}

	//load water shader
	m_waterShader.loadShader(aie::eShaderStage::VERTEX,
		"./shaders/reflectiveWater.vert");
	m_waterShader.loadShader(aie::eShaderStage::FRAGMENT,
		"./shaders/reflectiveWater.frag");
	if (m_waterShader.link() == false) {
		printf("Reflective Water Shader Error: %s\n", m_waterShader.getLastError());
		return false;
	}

	//load shadow gen shader
	m_shadowGenShader.loadShader(aie::eShaderStage::VERTEX,
		"./shaders/shadowGen.vert");
	m_shadowGenShader.loadShader(aie::eShaderStage::FRAGMENT,
		"./shaders/shadowGen.frag");
	if (m_shadowGenShader.link() == false) {
		printf("Shadow Generation Shader Error: %s\n", m_shadowGenShader.getLastError());
		return false;
	}

	//load shadow use shader
	m_shadowUseShader.loadShader(aie::eShaderStage::VERTEX,
		"./shaders/shadowUse.vert");
	m_shadowUseShader.loadShader(aie::eShaderStage::FRAGMENT,
		"./shaders/shadowUse.frag");
	if (m_shadowUseShader.link() == false) {
		printf("Shadow Usage Shader Error: %s\n", m_shadowUseShader.getLastError());
		return false;
	}


	if (m_loadMirror)
	{
		const unsigned int dimensions = 200;
		
		m_mirrorMesh.initialiseQuad(dimensions, dimensions);

		glm::vec3 position = { 0,0,0 };
		glm::vec3 eulerAngles = { 0,0,0 };
		glm::vec3 scale = { 10.0f / dimensions, 1.0f, 10.0f / dimensions };

		//create instance with a render target
		m_waterInstance = new Instance(position, eulerAngles, scale, &m_mirrorMesh, &m_waterShader, nullptr, &m_reflectionTarget, &m_refractionTarget);
		m_scene->AddInstance(m_waterInstance, 0);

		glm::vec3 ambient  = glm::vec3(0.000000f, 0.000000f, 0.000000f);
		glm::vec3 diffuse  = glm::vec3(0.000000f, 0.000000f, 0.000000f);
		glm::vec3 specular = glm::vec3(1.000000f, 1.000000f, 1.000000f);
		float specularPower = 128.0f; 
			
		m_waterInstance->addMaterial(ambient, diffuse, specular, specularPower);
		m_waterInstance->setDimensions(dimensions);
	}

	if (m_loadQuad)
	{
		const unsigned int dimensions = 1;

		m_quadMesh.initialiseQuad(dimensions, dimensions);

		glm::vec3 position = { 0,0,0 };
		glm::vec3 eulerAngles = { 0,0,0 };
		glm::vec3 scale = { 20.0f / dimensions, 1.0f, 20.0f / dimensions };

		//create instance
		m_scene->AddInstance(new Instance(position, eulerAngles, scale, &m_quadMesh, &m_shadowUseShader), 1);
		eulerAngles = { 180, 0,0 };
		m_scene->AddInstance(new Instance(position, eulerAngles, scale, &m_quadMesh, &m_shadowUseShader), 1);
	}

	if (m_loadBunny)
	{
		//load a bunny model
		if (m_bunnyMesh.load("./stanford/bunny.obj") == false) {
			printf("Bunny Mesh Error!\n");
			return false;
		}

		glm::vec3 position = { 2.5f,-0.1f,-2.5f };
		glm::vec3 eulerAngles = { 0,45,0 };
		glm::vec3 scale = { 0.3,0.3,0.3 };

		m_scene->AddInstance(new Instance(position, eulerAngles, scale, &m_bunnyMesh, &m_phongShader), 1);
	}

	if (m_loadBuddha)
	{
		//load a buddha model
		if (m_buddhaMesh.load("./stanford/buddha.obj") == false) {
			printf("Buddha Mesh Error!\n");
			return false;
		}

		glm::vec3 position = { -2.5f,-0.1f,2.5f };
		glm::vec3 eulerAngles = { 0,45,0 };
		glm::vec3 scale = { 0.3,0.3,0.3 };

		m_scene->AddInstance(new Instance(position, eulerAngles, scale, &m_buddhaMesh, &m_phongShader), 1);
	}

	if (m_loadDragon)
	{
		//load a dragon model
		if (m_dragonMesh.load("./stanford/dragon.obj") == false) {
			printf("Dragon Mesh Error!\n");
			return false;
		}

		glm::vec3 position = { -2.5f,-0.1f,-2.5f };
		glm::vec3 eulerAngles = { 0,45,0 };
		glm::vec3 scale = { 0.3,0.3,0.3 };

		m_scene->AddInstance(new Instance(position, eulerAngles, scale, &m_dragonMesh, &m_phongShader), 1);
	}

	if (m_loadSpear)
	{
		//load soul spear
		if (m_spearMesh.load("./soulspear/soulspear.obj",
			true, true) == false) {
			printf("Soulspear Mesh Error!\n");
			return false;
		}

		glm::vec3 position = { 2.5f,0,2.5f };
		glm::vec3 eulerAngles = { 0,45,0 };
		glm::vec3 scale = { 0.6,0.6,0.6 };

		m_scene->AddInstance(new Instance(position, eulerAngles, scale, &m_spearMesh, &m_normalMapShader), 1);
	}

	if (m_loadWalls)
	{
		//load tile texture
		if (m_tileTexture.load("./textures/Tiles.jpg") == false) {
			printf("Failed to load texture!\n");
			return false;
		}
		
		const unsigned int dimensions = 1;
		const float size = 5.0f;

		m_tileMesh.initialiseQuad(dimensions, dimensions);

		glm::vec3 position1 = { -size,-size/2+0.5f,-size/2 };
		glm::vec3 position2 = { -size,-size/2+0.5f,+size/2 };
		glm::vec3 position3 = { +size,-size/2+0.5f,-size/2 };
		glm::vec3 position4 = { +size,-size/2+0.5f,+size/2 };
		glm::vec3 eulerAngles1 = { 0,0,90 };
		glm::vec3 eulerAngles2 = { 0,0,270 };
		glm::vec3 position5 = { -size/2,-size/2+0.5f,+size };
		glm::vec3 position6 = { +size/2,-size/2+0.5f,+size };
		glm::vec3 position7 = { -size/2,-size/2+0.5f,-size };
		glm::vec3 position8 = { +size/2,-size/2+0.5f,-size };
		glm::vec3 eulerAngles3 = { 90,0,0 };
		glm::vec3 eulerAngles4 = { 270,0,0 };
		glm::vec3 position9  = { +size/2, -size+0.5f, +size/2 };
		glm::vec3 position10 = { +size/2, -size+0.5f, -size/2 };
		glm::vec3 position11 = { -size/2, -size+0.5f, +size/2 };
		glm::vec3 position12 = { -size/2, -size+0.5f, -size/2 };
		glm::vec3 eulerAngles5 = { 0,0,0 };
		glm::vec3 eulerAngles6 = { 180,0,0 };

		glm::vec3 scale = { size / dimensions, 1.0f, size / dimensions };

		//create instances for each of the 16 textured wall segments
		Instance* tileInstance1 = new Instance(position1, eulerAngles1, scale, &m_tileMesh, &m_texturedShader, &m_tileTexture);
		m_scene->AddInstance(tileInstance1, -2);
		Instance* tileInstance2 = new Instance(position1, eulerAngles2, scale, &m_tileMesh, &m_texturedShader, &m_tileTexture);
		m_scene->AddInstance(tileInstance2, 2);
		Instance* tileInstance3 = new Instance(position2, eulerAngles1, scale, &m_tileMesh, &m_texturedShader, &m_tileTexture);
		m_scene->AddInstance(tileInstance3, -2);
		Instance* tileInstance4 = new Instance(position2, eulerAngles2, scale, &m_tileMesh, &m_texturedShader, &m_tileTexture);
		m_scene->AddInstance(tileInstance4, 2);
		Instance* tileInstance5 = new Instance(position3, eulerAngles1, scale, &m_tileMesh, &m_texturedShader, &m_tileTexture);
		m_scene->AddInstance(tileInstance5, 2);
		Instance* tileInstance6 = new Instance(position3, eulerAngles2, scale, &m_tileMesh, &m_texturedShader, &m_tileTexture);
		m_scene->AddInstance(tileInstance6, -2);
		Instance* tileInstance7 = new Instance(position4, eulerAngles1, scale, &m_tileMesh, &m_texturedShader, &m_tileTexture);
		m_scene->AddInstance(tileInstance7, 2);
		Instance* tileInstance8 = new Instance(position4, eulerAngles2, scale, &m_tileMesh, &m_texturedShader, &m_tileTexture);
		m_scene->AddInstance(tileInstance8, -2);
		Instance* tileInstance9 = new Instance(position5, eulerAngles3, scale, &m_tileMesh, &m_texturedShader, &m_tileTexture);
		m_scene->AddInstance(tileInstance9, -2);
		Instance* tileInstance10 = new Instance(position5, eulerAngles4, scale, &m_tileMesh, &m_texturedShader, &m_tileTexture);
		m_scene->AddInstance(tileInstance10, 2);
		Instance* tileInstance11 = new Instance(position6, eulerAngles3, scale, &m_tileMesh, &m_texturedShader, &m_tileTexture);
		m_scene->AddInstance(tileInstance11, -2);
		Instance* tileInstance12 = new Instance(position6, eulerAngles4, scale, &m_tileMesh, &m_texturedShader, &m_tileTexture);
		m_scene->AddInstance(tileInstance12, 2);
		Instance* tileInstance13 = new Instance(position7, eulerAngles3, scale, &m_tileMesh, &m_texturedShader, &m_tileTexture);
		m_scene->AddInstance(tileInstance13, 2);
		Instance* tileInstance14 = new Instance(position7, eulerAngles4, scale, &m_tileMesh, &m_texturedShader, &m_tileTexture);
		m_scene->AddInstance(tileInstance14, -2);
		Instance* tileInstance15 = new Instance(position8, eulerAngles3, scale, &m_tileMesh, &m_texturedShader, &m_tileTexture);
		m_scene->AddInstance(tileInstance15, 2);
		Instance* tileInstance16 = new Instance(position8, eulerAngles4, scale, &m_tileMesh, &m_texturedShader, &m_tileTexture);
		m_scene->AddInstance(tileInstance16, -2);
		Instance* tileInstance17 = new Instance(position9, eulerAngles5, scale, &m_tileMesh, &m_texturedShader, &m_tileTexture);
		m_scene->AddInstance(tileInstance17, -1);
		Instance* tileInstance18 = new Instance(position9, eulerAngles6, scale, &m_tileMesh, &m_texturedShader, &m_tileTexture);
		m_scene->AddInstance(tileInstance18, -2);
		Instance* tileInstance19 = new Instance(position10, eulerAngles5, scale, &m_tileMesh, &m_texturedShader, &m_tileTexture);
		m_scene->AddInstance(tileInstance19, -1);
		Instance* tileInstance20 = new Instance(position10, eulerAngles6, scale, &m_tileMesh, &m_texturedShader, &m_tileTexture);
		m_scene->AddInstance(tileInstance20, -2);
		Instance* tileInstance21 = new Instance(position11, eulerAngles5, scale, &m_tileMesh, &m_texturedShader, &m_tileTexture);
		m_scene->AddInstance(tileInstance21, -1);
		Instance* tileInstance22 = new Instance(position11, eulerAngles6, scale, &m_tileMesh, &m_texturedShader, &m_tileTexture);
		m_scene->AddInstance(tileInstance22, -2);
		Instance* tileInstance23 = new Instance(position12, eulerAngles5, scale, &m_tileMesh, &m_texturedShader, &m_tileTexture);
		m_scene->AddInstance(tileInstance23, -1);
		Instance* tileInstance24 = new Instance(position12, eulerAngles6, scale, &m_tileMesh, &m_texturedShader, &m_tileTexture);
		m_scene->AddInstance(tileInstance24, -2);

		glm::vec3 ambient  = glm::vec3(0.100000f, 0.100000f, 0.100000f);
		glm::vec3 diffuse  = glm::vec3(1.000000f, 1.000000f, 1.000000f);
		glm::vec3 specular = glm::vec3(0.350000f, 0.350000f, 0.350000f);
		float specularPower = 34.0f;

		tileInstance1->addMaterial(ambient, diffuse, specular, specularPower);
		tileInstance1->setDimensions(dimensions);
		tileInstance2->addMaterial(ambient, diffuse, specular, specularPower);
		tileInstance2->setDimensions(dimensions);
		tileInstance3->addMaterial(ambient, diffuse, specular, specularPower);
		tileInstance3->setDimensions(dimensions);
		tileInstance4->addMaterial(ambient, diffuse, specular, specularPower);
		tileInstance4->setDimensions(dimensions);
		tileInstance5->addMaterial(ambient, diffuse, specular, specularPower);
		tileInstance5->setDimensions(dimensions);
		tileInstance6->addMaterial(ambient, diffuse, specular, specularPower);
		tileInstance6->setDimensions(dimensions);
		tileInstance7->addMaterial(ambient, diffuse, specular, specularPower);
		tileInstance7->setDimensions(dimensions);
		tileInstance8->addMaterial(ambient, diffuse, specular, specularPower);
		tileInstance8->setDimensions(dimensions);
		tileInstance9->addMaterial(ambient, diffuse, specular, specularPower);
		tileInstance9->setDimensions(dimensions);
		tileInstance10->addMaterial(ambient, diffuse, specular, specularPower);
		tileInstance10->setDimensions(dimensions);
		tileInstance11->addMaterial(ambient, diffuse, specular, specularPower);
		tileInstance11->setDimensions(dimensions);
		tileInstance12->addMaterial(ambient, diffuse, specular, specularPower);
		tileInstance12->setDimensions(dimensions);
		tileInstance13->addMaterial(ambient, diffuse, specular, specularPower);
		tileInstance13->setDimensions(dimensions);
		tileInstance14->addMaterial(ambient, diffuse, specular, specularPower);
		tileInstance14->setDimensions(dimensions);
		tileInstance15->addMaterial(ambient, diffuse, specular, specularPower);
		tileInstance15->setDimensions(dimensions);
		tileInstance16->addMaterial(ambient, diffuse, specular, specularPower);
		tileInstance16->setDimensions(dimensions);
		tileInstance17->addMaterial(ambient, diffuse, specular, specularPower);
		tileInstance17->setDimensions(dimensions);
		tileInstance18->addMaterial(ambient, diffuse, specular, specularPower);
		tileInstance18->setDimensions(dimensions);
		tileInstance19->addMaterial(ambient, diffuse, specular, specularPower);
		tileInstance19->setDimensions(dimensions);
		tileInstance20->addMaterial(ambient, diffuse, specular, specularPower);
		tileInstance20->setDimensions(dimensions);
		tileInstance21->addMaterial(ambient, diffuse, specular, specularPower);
		tileInstance21->setDimensions(dimensions);
		tileInstance22->addMaterial(ambient, diffuse, specular, specularPower);
		tileInstance22->setDimensions(dimensions);
		tileInstance23->addMaterial(ambient, diffuse, specular, specularPower);
		tileInstance23->setDimensions(dimensions);
		tileInstance24->addMaterial(ambient, diffuse, specular, specularPower);
		tileInstance24->setDimensions(dimensions);
	}

	//create a fullscreen quad for post processing
	m_postMesh.initialiseFullscreenQuad();
	//create instance with the post processing render target
	m_postProcessingInstance = new Instance(glm::vec3(0), glm::vec3(0), glm::vec3(1), &m_postMesh, &m_depthShader, nullptr, &m_postTarget);

	return true;
}


void Application3D::shutdown() {

	Gizmos::destroy();
	delete m_scene;
}


void Application3D::update(float deltaTime) {

	// query time since application started
	float time = getTime();

	//update scene time
	m_scene->setTime(time);

	//update camera position
	m_camera->update(deltaTime);

	//use an ImGUI window to change the light direction and colour at runtime
	ImGui::Begin("Light Settings");
	ImGui::DragFloat3("Sunlight Direction", &(m_scene->getLight().direction[0]), 0.01f, -1.0f, 1.0f);
	ImGui::DragFloat3("Sunlight Colour", &(m_scene->getLight().colour[0]), 0.01f, 0.0f, 2.0f);

	if (ImGui::Button("Box Blur"))
	{
		m_postProcessingActive = true;
		m_postProcessingInstance->swapShader(&m_boxBlurShader);
		m_scene->setWireFrame(false);
	}
	if (ImGui::Button("Sharpen"))
	{
		m_postProcessingActive = true;
		m_postProcessingInstance->swapShader(&m_sharpenShader);
		m_scene->setWireFrame(false);
	}
	if (ImGui::Button("Chromatic Aberration"))
	{
		m_postProcessingActive = true;
		m_postProcessingInstance->swapShader(&m_chromaticShader);
		m_scene->setWireFrame(false);
	}
	if (ImGui::Button("Wire Frame"))
	{
		m_postProcessingActive = false;
		m_scene->setWireFrame(true);
	}
	if (ImGui::Button("No Effects"))
	{
		m_postProcessingActive = false;
		m_scene->setWireFrame(false);
	}
	if (ImGui::Button("Toggle Grid"))
	{
		m_showGrid = !m_showGrid;
	}

	ImGui::End();

	//set the background colour based on the y angle of the sunlight
	float lightLevel = m_scene->getLight().direction[1] * -0.35f + 0.4f;
	setBackgroundColour(0.05f, lightLevel/1.5f, lightLevel, 1.0f);

	// wipe the gizmos clean for this frame
	Gizmos::clear();

	// draw a simple grid with gizmos
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i < 21; ++i) {
		Gizmos::addLine(vec3(-10 + i, 0, 10),
						vec3(-10 + i, 0, -10),
						i == 10 ? white : black);
		Gizmos::addLine(vec3(10, 0, -10 + i),
						vec3(-10, 0, -10 + i),
						i == 10 ? white : black);
	}

	// add a transform so that we can see the axis
	Gizmos::addTransform(mat4(1));

	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}


void Application3D::draw() {
	
	//get the pvm for the sunlight
	glm::vec3 lightDirection = glm::normalize(glm::vec3(m_scene->getLight().direction * -1.0f));
	glm::mat4 lightProjection = glm::ortho<float>(-10, 10, -10, 10, -10, 10);
	glm::mat4 lightView = glm::lookAt(lightDirection, glm::vec3(0), glm::vec3(0, 1, 0));
	glm::mat4 lightMatrix = lightProjection * lightView;
	
	// shadow pass: bind our shadow map target and clear the depth 
	m_shadowTarget.bind();
	glViewport(0, 0, 2048, 2048);
	clearScreen();

	m_shadowGenShader.bind();

	// bind the light matrix 
	int loc = glGetUniformLocation(m_shadowGenShader.getHandle(), "lightMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(lightMatrix[0][0]));

	//draw all shadow casters - ie everything but the water
	glCullFace(GL_FRONT);
	m_scene->drawRaw(2, &m_shadowGenShader);
	glCullFace(GL_BACK);

	//unbind render target and reset viewport size
	m_shadowTarget.unbind();
	glViewport(0, 0, 1280, 720);
	clearScreen();

	//bind the shadow use shader
	m_shadowUseShader.bind();

	//offset the light matrix from clip to texture space
	glm::mat4 textureSpaceOffset(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	);
	glm::mat4 offsetLightMatrix = textureSpaceOffset * lightMatrix;

	m_scene->draw(2);
	
	if (m_loadMirror)
	{
		SetRenderTarget();
	}

	clearScreen();

	//get camera matrices
	glm::mat4 projectionMatrix = m_camera->getProjectionMatrix((float)getWindowWidth(), (float)getWindowHeight());
	glm::mat4 viewMatrix = m_camera->getViewMatrix();

	if (m_postProcessingActive)
	{
		//bind post processing render target, draw scene and gizmos, then unbind and clear back buffer
		m_postTarget.bind();
		clearScreen();
		m_scene->draw(0);
		m_postTarget.unbind();
		clearScreen();

		//draw post processing quad
		m_postProcessingInstance->draw(m_scene);
	}
	else
	{
		m_scene->draw(0);
	}
}


void Application3D::SetRenderTarget()
{
	//bind the reflection target, and render the reflective texture to it
	m_reflectionTarget.bind();
	clearScreen();
	
	//get the camera
	Camera* cam = m_scene->getCamera();

	//get the cameras position and flip the y value
	glm::vec3 position = cam->getPosition();
	cam->setPosition(glm::vec3(position.x, -position.y, position.z));

	//get the camera's phi angle and flip it
	float phi = cam->getPhi();
	cam->setPhi(-phi);
	
	//draw everything that is above the water level
	m_scene->draw(1);

	//get camera matrices
	glm::mat4 projectionMatrix = m_camera->getProjectionMatrix((float)getWindowWidth(), (float)getWindowHeight());
	glm::mat4 viewMatrix = m_camera->getViewMatrix();

	if (m_showGrid)
	{
		Gizmos::draw(projectionMatrix * viewMatrix);
		Gizmos::draw2D((float)getWindowWidth(), (float)getWindowHeight());
	}

	//reset the camera position and angle
	cam->setPosition(position);
	cam->setPhi(phi);

	//switch to the refraction target and render the refractive image to it
	m_reflectionTarget.unbind();
	m_refractionTarget.bind();
	clearScreen();

	//draw everything that is under the water level
	m_scene->draw(-1);

	//unbind the refractive target
	m_refractionTarget.unbind();
}