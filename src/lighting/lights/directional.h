#pragma once
#include <glm_includes.h>

class CascadedShadowMap;

struct DirectionalLightVolume {
    float l;
    float b;
    float n;
    float r;
    float t;
    float f;
    glm::vec3 direction;
    glm::mat4 viewMatrix;
};

class DirectionalLight {
public:
    DirectionalLight(glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 getLightSpaceMatrix(float sinT, float cosT, glm::vec3 targetPos = glm::vec3(0.0f, 200.0f, 0.0f));

    glm::mat4 getViewMatrix() const;

    inline glm::vec3 getDirection() const {
        return m_direction;
    }

    DirectionalLightVolume getViewVolume(float sinT, float cosT, glm::vec3 targetPos = glm::vec3(0.0f, 200.0f, 0.0f)) const;

    // TODO: remove.
    friend class CascadedShadowMap;

private:
    glm::vec3 m_direction;
};
