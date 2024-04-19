#include "spotlight.h"

SpotLight::SpotLight(glm::vec3 pos, glm::vec3 dir)
    : m_position(pos),
      m_direction(dir),
      m_camera(nullptr)
{

}

SpotLight::SpotLight(const Camera *camera)
    : m_position(),
      m_direction(),
      m_camera(camera)
{

}
