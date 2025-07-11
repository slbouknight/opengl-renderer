/* LightingManager.cpp */
#include "LightingManager.h"
#include "Shader.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

//------------------------------------------------------------------------------
// Helper: single cube VAO for drawing point-light shapes
namespace {
    static unsigned int cubeVAO = 0;
    static unsigned int cubeVBO = 0;
    void initCube() {
        if (cubeVAO != 0)
            return;
        float vertices[] = {
            // positions
            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,

            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,

             0.5f,  0.5f,  0.5f,
             0.5f,  0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,

            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f,  0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
             0.5f,  0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }
}

//------------------------------------------------------------------------------
// DirectionalLight
DirectionalLight::DirectionalLight(const DirectionalLightDesc& desc)
    : m_desc(desc)
{
}

void DirectionalLight::uploadToShader(const Shader& shader, int /*index*/) const
{
    shader.setVec3("dirLight.direction", m_desc.direction);
    shader.setVec3("dirLight.ambient", m_desc.ambient);
    shader.setVec3("dirLight.diffuse", m_desc.diffuse);
    shader.setVec3("dirLight.specular", m_desc.specular);
}

void DirectionalLight::drawShape(const Shader& /*shader*/) const
{
    // No visual shape for a directional light
}

//------------------------------------------------------------------------------
// PointLight
PointLight::PointLight(const PointLightDesc& desc)
    : m_desc(desc)
{
}

void PointLight::uploadToShader(const Shader& shader, int index) const
{
    std::string prefix = "pointLights[" + std::to_string(index) + "]";
    shader.setVec3(prefix + ".position", m_desc.position);
    shader.setVec3(prefix + ".ambient", m_desc.ambient);
    shader.setVec3(prefix + ".diffuse", m_desc.diffuse);
    shader.setVec3(prefix + ".specular", m_desc.specular);
    shader.setFloat(prefix + ".constant", m_desc.constant);
    shader.setFloat(prefix + ".linear", m_desc.linear);
    shader.setFloat(prefix + ".quadratic", m_desc.quadratic);
}

void PointLight::drawShape(const Shader& shader) const
{
    initCube();
    glm::mat4 model = glm::translate(glm::mat4(1.0f), m_desc.position)
        * glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
    shader.setMat4("model", model);
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

//------------------------------------------------------------------------------
// SpotLight
SpotLight::SpotLight(const SpotLightDesc& desc)
    : m_desc(desc)
{
}

void SpotLight::uploadToShader(const Shader& shader, int /*index*/) const
{
    shader.setVec3("spotLight.position", m_desc.position);
    shader.setVec3("spotLight.direction", m_desc.direction);
    shader.setVec3("spotLight.ambient", m_desc.ambient);
    shader.setVec3("spotLight.diffuse", m_desc.diffuse);
    shader.setVec3("spotLight.specular", m_desc.specular);
    shader.setFloat("spotLight.constant", m_desc.constant);
    shader.setFloat("spotLight.linear", m_desc.linear);
    shader.setFloat("spotLight.quadratic", m_desc.quadratic);
    shader.setFloat("spotLight.cutOff", m_desc.cutOff);
    shader.setFloat("spotLight.outerCutOff", m_desc.outerCutOff);
}

void SpotLight::drawShape(const Shader& /*shader*/) const
{
    // Visualization of a spot light can be implemented here
}

//------------------------------------------------------------------------------
// LightingManager
void LightingManager::addDirectional(const DirectionalLightDesc& desc)
{
    m_lights.push_back(std::make_unique<DirectionalLight>(desc));
}

void LightingManager::addPoint(const PointLightDesc& desc)
{
    m_lights.push_back(std::make_unique<PointLight>(desc));
}

void LightingManager::addSpot(const SpotLightDesc& desc)
{
    m_lights.push_back(std::make_unique<SpotLight>(desc));
}

void LightingManager::uploadToShader(const Shader& shader) const
{
    int pointCount = 0;
    for (const auto& light : m_lights) {
        if (auto dl = dynamic_cast<DirectionalLight*>(light.get())) {
            dl->uploadToShader(shader);
        }
        else if (auto pl = dynamic_cast<PointLight*>(light.get())) {
            pl->uploadToShader(shader, pointCount);
            ++pointCount;
        }
        else if (auto sl = dynamic_cast<SpotLight*>(light.get())) {
            sl->uploadToShader(shader);
        }
    }
    shader.setInt("NR_POINT_LIGHTS", pointCount);
}

void LightingManager::drawShapes(const Shader& shader) const
{
    for (const auto& light : m_lights) {
        light->drawShape(shader);
    }
}
