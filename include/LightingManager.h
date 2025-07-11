/* LightingManager.h */
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
#include <string>

#include "Shader.h"

// Descriptor structs for light creation
// --------------------------------------
struct DirectionalLightDesc {
    glm::vec3 direction{ 0.0f, -1.0f,  0.0f };
    glm::vec3 ambient{ 0.2f };
    glm::vec3 diffuse{ 0.8f };
    glm::vec3 specular{ 0.5f };
};

struct PointLightDesc {
    glm::vec3 position{ 0.0f };
    glm::vec3 ambient{ 0.2f };
    glm::vec3 diffuse{ 0.8f };
    glm::vec3 specular{ 1.0f };
    float     constant{ 1.0f };
    float     linear{ 0.09f };
    float     quadratic{ 0.032f };
};

struct SpotLightDesc {
    glm::vec3 position{ 0.0f };
    glm::vec3 direction{ 0.0f, -1.0f, 0.0f };
    glm::vec3 ambient{ 0.0f };
    glm::vec3 diffuse{ 1.0f };
    glm::vec3 specular{ 1.0f };
    float     constant{ 1.0f };
    float     linear{ 0.09f };
    float     quadratic{ 0.032f };
    float     cutOff{ glm::cos(glm::radians(12.5f)) };
    float     outerCutOff{ glm::cos(glm::radians(15.0f)) };
};

// Abstract base for all light types
// ----------------------------------
class Light {
public:
    virtual ~Light() = default;
    // Upload the light's uniforms to the shader; index for arrays
    virtual void uploadToShader(const Shader& shader, int index = 0) const = 0;
    // Draw the light's visual shape
    virtual void drawShape(const Shader& shader) const = 0;
};

// Concrete light implementations
// ------------------------------
class DirectionalLight : public Light {
public:
    DirectionalLight(const DirectionalLightDesc& desc);
    void uploadToShader(const Shader& shader, int index = 0) const override;
    void drawShape(const Shader& shader) const override;

private:
    DirectionalLightDesc m_desc;
};

class PointLight : public Light {
public:
    PointLight(const PointLightDesc& desc);
    void uploadToShader(const Shader& shader, int index = 0) const override;
    void drawShape(const Shader& shader) const override;

private:
    PointLightDesc m_desc;
};

class SpotLight : public Light {
public:
    SpotLight(const SpotLightDesc& desc);
    void uploadToShader(const Shader& shader, int index = 0) const override;
    void drawShape(const Shader& shader) const override;

private:
    SpotLightDesc m_desc;
};

// Manager for all lights
// ----------------------
class LightingManager {
public:
    // Factory methods
    void addDirectional(const DirectionalLightDesc& desc);
    void addPoint(const PointLightDesc& desc);
    void addSpot(const SpotLightDesc& desc);

    // Upload all light uniforms to shader
    void uploadToShader(const Shader& shader) const;
    // Draw all light shapes
    void drawShapes(const Shader& shader) const;

private:
    std::vector<std::unique_ptr<Light>> m_lights;
};
