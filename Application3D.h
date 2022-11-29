#pragma once

#include "Application.h"
#include "Shader.h"
#include "Mesh.h"
#include "OBJMesh.h"
#include "RenderTarget.h"
#include <glm/mat4x4.hpp>

class Instance;
class Camera;
class Scene;

class Application3D : public aie::Application 
{
public:

	Application3D();
	virtual ~Application3D();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	void SetRenderTarget();

protected:

	aie::ShaderProgram m_simpleShader;
	aie::ShaderProgram m_phongShader;
	aie::ShaderProgram m_texturedShader;
	aie::ShaderProgram m_normalMapShader;
	aie::ShaderProgram m_screenSpaceShader;
	aie::ShaderProgram m_postShader;
	aie::ShaderProgram m_boxBlurShader;
	aie::ShaderProgram m_sharpenShader;
	aie::ShaderProgram m_chromaticShader;
	aie::ShaderProgram m_waterShader;
	aie::ShaderProgram m_shadowGenShader;
	aie::ShaderProgram m_shadowUseShader;
	aie::ShaderProgram m_depthShader;

	bool m_loadQuad   = false;
	bool m_loadMirror = true;
	bool m_loadBunny  = true;
	bool m_loadBuddha = true;
	bool m_loadDragon = true;
	bool m_loadSpear  = true;
	bool m_loadWalls  = true;
	bool m_postProcessingActive = false;
	bool m_showGrid = false;

	Mesh m_mirrorMesh;
	Mesh m_quadMesh;
	Mesh m_postMesh;
	Mesh m_DOFOutFocusMesh;
	aie::OBJMesh m_bunnyMesh;
	aie::OBJMesh m_buddhaMesh;
	aie::OBJMesh m_dragonMesh;
	aie::OBJMesh m_spearMesh;

	Mesh m_tileMesh;
	aie::Texture m_tileTexture;

	Camera* m_camera;

	Scene* m_scene;

	aie::RenderTarget m_reflectionTarget;
	aie::RenderTarget m_refractionTarget;
	aie::RenderTarget m_postTarget;
	aie::RenderTarget m_shadowTarget;

	Instance* m_postProcessingInstance = nullptr;
	Instance* m_waterInstance = nullptr;
};