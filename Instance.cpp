#include "Instance.h"
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Application3D.h"
#include "Scene.h"
#include <glm/gtc/matrix_transform.hpp>
#include "gl_core_4_4.h"

Instance::Instance(glm::mat4 transform, aie::OBJMesh* OBJmesh, aie::ShaderProgram* shader, aie::Texture* texture, aie::RenderTarget* renderTarget1, aie::RenderTarget* renderTarget2)
{
    m_transform = transform;
    m_OBJmesh = OBJmesh;
    m_mesh = nullptr;
    m_shader = shader;
    m_texture = texture;
    m_renderTarget1 = renderTarget1;
    m_renderTarget2 = renderTarget2;
}


Instance::Instance(glm::vec3 position, glm::vec3 eulerAngles, glm::vec3 scale, aie::OBJMesh* OBJmesh, aie::ShaderProgram* shader, aie::Texture* texture, aie::RenderTarget* renderTarget1, aie::RenderTarget* renderTarget2)
{
    m_transform = makeTransform(position, eulerAngles, scale);
    m_OBJmesh = OBJmesh;
    m_mesh = nullptr;
    m_shader = shader;
    m_texture = texture;
    m_renderTarget1 = renderTarget1;
    m_renderTarget2 = renderTarget2;
}


Instance::Instance(glm::mat4 transform, Mesh* mesh, aie::ShaderProgram* shader, aie::Texture* texture, aie::RenderTarget* renderTarget1, aie::RenderTarget* renderTarget2)
{
    m_transform = transform;
    m_OBJmesh = nullptr;
    m_mesh = mesh;
    m_shader = shader;
    m_texture = texture;
    m_renderTarget1 = renderTarget1;
    m_renderTarget2 = renderTarget2;
}


Instance::Instance(glm::vec3 position, glm::vec3 eulerAngles, glm::vec3 scale, Mesh* mesh, aie::ShaderProgram* shader, aie::Texture* texture, aie::RenderTarget* renderTarget1, aie::RenderTarget* renderTarget2)
{
    m_transform = makeTransform(position, eulerAngles, scale);
    m_OBJmesh = nullptr;
    m_mesh = mesh;
    m_shader = shader;
    m_texture = texture;
    m_renderTarget1 = renderTarget1;
    m_renderTarget2 = renderTarget2;
}


//creates a mat4 transform from given values
glm::mat4 Instance::makeTransform(glm::vec3 position, glm::vec3 eulerAngles, glm::vec3 scale)
{
    return glm::translate(glm::mat4(1), position)
        * glm::rotate(glm::mat4(1), glm::radians(eulerAngles.z), glm::vec3(0, 0, 1))
        * glm::rotate(glm::mat4(1), glm::radians(eulerAngles.y), glm::vec3(0, 1, 0))
        * glm::rotate(glm::mat4(1), glm::radians(eulerAngles.x), glm::vec3(1, 0, 0))
        * glm::scale(glm::mat4(1), scale);
}


//draws the instanced object with the given shader and scene lighting
void Instance::draw(Scene* scene, aie::ShaderProgram* tempShader)
{
    //if a shader was passed through, use it to render, if not use the stored shader
    aie::ShaderProgram* shader;
    if (tempShader != nullptr)
        shader = tempShader;
    else
        shader = m_shader;
    
    // set the shader pipeline 
    shader->bind();

    // bind transform and other uniforms 
    if (glGetUniformLocation(shader->getHandle(), "ProjectionViewModel") >= 0)
    {
        auto pvm = scene->getCamera()->getProjectionMatrix(scene->getWindowSize().x, scene->getWindowSize().y) * scene->getCamera()->getViewMatrix() * m_transform;
        shader->bindUniform("ProjectionViewModel", pvm);
    }

    //bind lighting and camera
    if (glGetUniformLocation(shader->getHandle(), "ModelMatrix") >= 0)
        shader->bindUniform("ModelMatrix", m_transform);
    if (glGetUniformLocation(shader->getHandle(), "AmbientColour") >= 0)
        shader->bindUniform("AmbientColour", scene->getAmbientLight());
    if (glGetUniformLocation(shader->getHandle(), "LightColour") >= 0)
        shader->bindUniform("LightColour", scene->getLight().colour);
    if (glGetUniformLocation(shader->getHandle(), "LightDirection") >= 0)
        shader->bindUniform("LightDirection", scene->getLight().direction);
    if (glGetUniformLocation(shader->getHandle(), "LightMatrix") >= 0)
        shader->bindUniform("LightMatrix", scene->getLightMatrix());
    if (glGetUniformLocation(shader->getHandle(), "offsetLightMatrix") >= 0)
        shader->bindUniform("offsetLightMatrix", scene->getOffsetLightMatrix());
    if (glGetUniformLocation(shader->getHandle(), "cameraPosition") >= 0)
        shader->bindUniform("cameraPosition", scene->getCamera()->getPosition());

    //bind point lights
    int numLights = scene->getNumLights();
    if (glGetUniformLocation(shader->getHandle(), "numLights") >= 0)
        shader->bindUniform("numLights", numLights);
    if (glGetUniformLocation(shader->getHandle(), "PointLightPosition") >= 0)
        shader->bindUniform("PointLightPosition", numLights, scene->getPointlightPositions());
    if (glGetUniformLocation(shader->getHandle(), "PointLightColour") >= 0)
        shader->bindUniform("PointLightColour", numLights, scene->getPointlightColours());

    //bind shadow map
    if (glGetUniformLocation(shader->getHandle(), "shadowMap") >= 0)
    {
        scene->getShadowTarget()->bindDepthTarget(7);
        shader->bindUniform("shadowMap", 7);
    }
    if (glGetUniformLocation(shader->getHandle(), "shadowBiasMin") >= 0)
        shader->bindUniform("shadowBiasMin", scene->getShadowBias().x);
    if (glGetUniformLocation(shader->getHandle(), "shadowBiasMax") >= 0)
        shader->bindUniform("shadowBiasMax", scene->getShadowBias().y);

    //bind time
    if (glGetUniformLocation(shader->getHandle(), "time") >= 0)
        shader->bindUniform("time", scene->getTime());

    //bind dimensions
    if (glGetUniformLocation(shader->getHandle(), "dimensions") >= 0)
        shader->bindUniform("dimensions", m_dimensions);

    //if K values are set, bind K values separately
    if (m_materialManualLoad)
    {
        if (glGetUniformLocation(shader->getHandle(), "Ka") >= 0)
            shader->bindUniform("Ka", m_ambient); 
        if (glGetUniformLocation(shader->getHandle(), "Kd") >= 0)
            shader->bindUniform("Kd", m_diffuse); 
        if (glGetUniformLocation(shader->getHandle(), "Ks") >= 0)
            shader->bindUniform("Ks", m_specular); 
        if (glGetUniformLocation(shader->getHandle(), "specularPower") >= 0)
            shader->bindUniform("specularPower", m_specularPower);
    }

    //if textured, bind texture
    if (m_texture != nullptr)
    {
        if (glGetUniformLocation(shader->getHandle(), "diffuseTexture") >= 0)
        {
            shader->bindUniform("diffuseTexture", 1);
            m_texture->bind(1);
        }
    }
    //if using render target as texture, bind it
    else if (m_renderTarget1 != nullptr)
    {
        if (glGetUniformLocation(shader->getHandle(), "diffuseTexture1") >= 0)
            shader->bindUniform("diffuseTexture1", 1);
        if (glGetUniformLocation(shader->getHandle(), "diffuseTexture2") >= 0)
            shader->bindUniform("diffuseTexture2", 2);
        if (glGetUniformLocation(shader->getHandle(), "colourTarget") >= 0)
            shader->bindUniform("colourTarget", 1);

        m_renderTarget1->getTarget(0).bind(1);

        if (m_renderTarget2 != nullptr)
            m_renderTarget2->getTarget(0).bind(2);
    }

    // draw mesh 
    if (m_OBJmesh != nullptr)
    {
        m_OBJmesh->draw();
    }
    else if (m_mesh != nullptr)
    {
        m_mesh->draw();
    }
}


//draws the instanced object binding only the pvm
void Instance::drawRaw(Scene* scene, aie::ShaderProgram* tempShader)
{
    //if a shader was passed through, use it to render, if not use the stored shader
    aie::ShaderProgram* shader;
    if (tempShader != nullptr)
        shader = tempShader;
    else
        shader = m_shader;
    
    if (glGetUniformLocation(shader->getHandle(), "ProjectionViewModel") >= 0)
    {
        auto pvm = scene->getCamera()->getProjectionMatrix(scene->getWindowSize().x, scene->getWindowSize().y) * scene->getCamera()->getViewMatrix() * m_transform;
        shader->bindUniform("ProjectionViewModel", pvm);
    }

    if (glGetUniformLocation(shader->getHandle(), "ModelMatrix") >= 0)
        shader->bindUniform("ModelMatrix", m_transform);
    
    // draw mesh 
    if (m_OBJmesh != nullptr)
    {
        m_OBJmesh->draw();
    }
    else if (m_mesh != nullptr)
    {
        m_mesh->draw();
    }
}


//adds material lighting data
void Instance::addMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float specularPower)
{
    m_materialManualLoad = true;
    
    m_ambient = ambient;
    m_diffuse = diffuse;
    m_specular = specular;
    m_specularPower = specularPower;
}


//adds parameters for shaders to use
void Instance::setDimensions(int dimensions)
{
    m_dimensions = dimensions;
}