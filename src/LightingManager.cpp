#include "LightingManager.h"
#include "Shader.h" // Your existing wrapper

#include <string>

void LightingManager::addLight(const Light& light) {
    lights.push_back(light);
}

void LightingManager::removeLight(size_t index) {
    if (index < lights.size()) {
        lights.erase(lights.begin() + index);
    }
}

void LightingManager::clearLights() {
    lights.clear();
}

void LightingManager::initializeDefaultLights() {
    // Add 1 directional, 4 point, 1 spotlight
    Light dir;
    dir.type = LightType::Directional;
    dir.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    dir.ambient = glm::vec3(0.05f);
    dir.diffuse = glm::vec3(0.4f);
    dir.specular = glm::vec3(0.5f);
    lights.push_back(dir);

    glm::vec3 pointPositions[] = {
        { 0.7f,  0.2f,  2.0f},
        { 2.3f, -3.3f, -4.0f},
        {-4.0f,  2.0f, -12.0f},
        { 0.0f,  0.0f, -3.0f}
    };

    for (const auto& pos : pointPositions) {
        Light pt;
        pt.type = LightType::Point;
        pt.position = pos;
        lights.push_back(pt);
    }

    // You can optionally add a spotlight later (usually follows the camera)
}

void LightingManager::uploadToShader(const Shader& shader, const glm::vec3& cameraPosition) const {
    int pointIndex = 0;

    for (size_t i = 0; i < lights.size(); ++i) {
        const Light& light = lights[i];

        switch (light.type) {
        case LightType::Directional:
            shader.setVec3("dirLight.direction", light.direction);
            shader.setVec3("dirLight.ambient", light.ambient);
            shader.setVec3("dirLight.diffuse", light.diffuse);
            shader.setVec3("dirLight.specular", light.specular);
            break;

        case LightType::Point: {
            std::string prefix = "pointLights[" + std::to_string(pointIndex) + "]";
            shader.setVec3(prefix + ".position", light.position);
            shader.setVec3(prefix + ".ambient", light.ambient);
            shader.setVec3(prefix + ".diffuse", light.diffuse);
            shader.setVec3(prefix + ".specular", light.specular);
            shader.setFloat(prefix + ".constant", light.constant);
            shader.setFloat(prefix + ".linear", light.linear);
            shader.setFloat(prefix + ".quadratic", light.quadratic);
            pointIndex++;
            break;
        }

        case LightType::Spot:
            shader.setVec3("spotLight.position", light.position);
            shader.setVec3("spotLight.direction", light.direction);
            shader.setVec3("spotLight.ambient", light.ambient);
            shader.setVec3("spotLight.diffuse", light.diffuse);
            shader.setVec3("spotLight.specular", light.specular);
            shader.setFloat("spotLight.constant", light.constant);
            shader.setFloat("spotLight.linear", light.linear);
            shader.setFloat("spotLight.quadratic", light.quadratic);
            shader.setFloat("spotLight.cutOff", light.cutOff);
            shader.setFloat("spotLight.outerCutOff", light.outerCutOff);
            break;
        }
    }

    shader.setVec3("viewPos", cameraPosition);
}
