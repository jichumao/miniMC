#pragma once

#include <drawable.h>
#include <smartpointerhelp.h>
#include <vector>
#include "lighting/lights/directional.h"

class ViewVolumeDisplay : public Drawable {
public:
    ViewVolumeDisplay(
        OpenGLContext *glContext,
        float l,
        float b,
        float n,
        float r,
        float t,
        float f,
        glm::mat4 lightView
    );

    ViewVolumeDisplay(OpenGLContext *glContext, const DirectionalLightVolume &volume);

    void createVBOdata();

    GLenum drawMode() override;

private:
    float m_l;
    float m_b;
    float m_n;
    float m_r;
    float m_t;
    float m_f;
    glm::mat4 m_lightView;

    std::vector<GLuint> createBufIdxData() const;

    std::vector<glm::vec4> createBufPosData() const;

    std::vector<glm::vec4> createBufColorData() const;
};
