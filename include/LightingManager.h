#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

class Shader;

enum class LightType {
    Directional,
    Point,
    Spot
};

struct Light {
    LightType type;

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);

    glm::vec3 ambient = glm::vec3(0.05f);
    glm::vec3 diffuse = glm::vec3(0.8f);
    glm::vec3 specular = glm::vec3(1.0f);

    // attenuation (point/spot)
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    // spot light angles
    float cutOff = glm::cos(glm::radians(12.5f));
    float outerCutOff = glm::cos(glm::radians(15.0f));
};

class LightingManager {
public:
    std::vector<Light> lights;

    void addLight(const Light& light);
    void removeLight(size_t index);
    void clearLights();

    // Sets all light uniforms on the shader
    void uploadToShader(const Shader& shader, const glm::vec3& cameraPosition) const;

    // Optional: call once to pre-fill with some lights
    void initializeDefaultLights();
};
