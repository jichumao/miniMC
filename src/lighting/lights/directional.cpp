#include "directional.h"
#include "scene/camera.h"

#define NUM_FRUSTUM_CORNERS 8

DirectionalLight::DirectionalLight(glm::vec3 direction) : m_direction(direction)
{}

glm::mat4 DirectionalLight::getLightSpaceMatrix(float sinT, float cosT, glm::vec3 targetPos) {
    DirectionalLightVolume volume = getViewVolume(sinT, cosT, targetPos);

    m_direction = volume.direction;

    return glm::ortho(
        volume.l,
        volume.r,
        volume.b,
        volume.t,
        volume.f,
        volume.n
    ) * volume.viewMatrix;
}

glm::mat4 DirectionalLight::getViewMatrix() const {
    float radius = 40.f;
    glm::vec3 targetPos = glm::vec3(0.0f, 200.0f, 0.0f);
    glm::vec3 lightPos = (-2.0f * radius * m_direction) + targetPos;
    
    return glm::lookAt(lightPos, targetPos, glm::vec3(0.0f, 1.0f, 0.0f));
}

DirectionalLightVolume DirectionalLight::getViewVolume(float sinT, float cosT, glm::vec3 targetPos) const {
    DirectionalLightVolume volume;

    volume.direction = glm::normalize(glm::vec3(sinT, 0.2, cosT));

    float radius = 30.f;
    glm::vec3 lightPos = (-2.0f * radius * volume.direction) + targetPos;

    volume.viewMatrix = glm::lookAt(lightPos, targetPos, glm::vec3(0.0f, 1.0f, 0.0f));
    
    glm::vec4 targetPosLS = volume.viewMatrix * glm::vec4(targetPos, 1.0f);
    float targetPosHeight = targetPos.y - 80.0f;

    volume.l = targetPosLS.x - radius - 150;
    volume.b = targetPosLS.y - targetPosHeight;
    volume.n = targetPosLS.z - radius - 200;
    volume.r = targetPosLS.x + radius + 150;
    volume.t = targetPosLS.y + radius + 30;
    volume.f = targetPosLS.z + radius + 150;

    return volume;
}
