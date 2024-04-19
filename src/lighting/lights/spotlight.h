#pragma once
#include "glm_includes.h"
#include "math/transform.h"
#include "scene/camera.h"

class BaseLight {
public:
    glm::vec3 m_color = glm::vec3(1.0f, 1.0f, 1.0f);
    float AmbientIntensity = 0.0f;
    float DiffuseIntensity = 0.0f;
};

class SpotLight : public BaseLight {
public:
    glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_direction = glm::vec3(1.0f, 0.0f, 0.0f);

    float m_cutoff = 20.0f;

    SpotLight(glm::vec3 pos, glm::vec3 dir);

    SpotLight(const Camera *m_camera);

    const Camera *m_camera;
};
