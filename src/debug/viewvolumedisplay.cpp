#include "viewvolumedisplay.h"
#include <stdexcept>

ViewVolumeDisplay::ViewVolumeDisplay(
    OpenGLContext *glContext,
    float l,
    float b,
    float n,
    float r,
    float t,
    float f,
    glm::mat4 lightView
) : Drawable(glContext), m_l(l), m_b(b), m_n(n), m_r(r), m_t(t), m_f(f), m_lightView(lightView) {

}

ViewVolumeDisplay::ViewVolumeDisplay(
    OpenGLContext *glContext,
    const DirectionalLightVolume &volume
) : Drawable(glContext),
    m_l(volume.l),
    m_b(volume.b),
    m_n(volume.n),
    m_r(volume.r),
    m_t(volume.t),
    m_f(volume.f),
    m_lightView(volume.viewMatrix) 
{}

GLenum ViewVolumeDisplay::drawMode() {
    return GL_LINES;
}

void ViewVolumeDisplay::createVBOdata() {
    std::vector<GLuint> indices = createBufIdxData();
    std::vector<glm::vec4> positions = createBufPosData();
    std::vector<glm::vec4> colors = createBufColorData();

    m_count = indices.size();
    generateIdx();
    bindIdx();
    mp_context->glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        m_count * sizeof(GLuint),
        indices.data(),
        GL_STATIC_DRAW
    );

    generatePos();
    bindPos();
    mp_context->glBufferData(
        GL_ARRAY_BUFFER,
        positions.size() * sizeof(glm::vec4),
        positions.data(),
        GL_STATIC_DRAW
    );

    generateCol();
    bindCol();
    mp_context->glBufferData(
        GL_ARRAY_BUFFER,
        colors.size() * sizeof(glm::vec4),
        colors.data(),
        GL_STATIC_DRAW
    );
}

std::vector<GLuint> ViewVolumeDisplay::createBufIdxData() const {
    std::vector<GLuint> indices;

    for (GLuint i = 0; i < 24; ++i) {
        indices.push_back(i);
    }

    return indices;
}

std::vector<glm::vec4> ViewVolumeDisplay::createBufPosData() const {
    glm::mat4 lightToWorld = glm::inverse(m_lightView);
    std::vector<glm::vec4> positions {
        lightToWorld * glm::vec4(m_l, m_b, m_n, 1.0f),
        lightToWorld * glm::vec4(m_r, m_b, m_n, 1.0f),
        lightToWorld * glm::vec4(m_r, m_t, m_n, 1.0f),
        lightToWorld * glm::vec4(m_l, m_t, m_n, 1.0f),

        lightToWorld * glm::vec4(m_l, m_b, m_f, 1.0f),
        lightToWorld * glm::vec4(m_r, m_b, m_f, 1.0f),
        lightToWorld * glm::vec4(m_r, m_t, m_f, 1.0f),
        lightToWorld * glm::vec4(m_l, m_t, m_f, 1.0f),

        lightToWorld * glm::vec4(m_l, m_b, m_n, 1.0f),
        lightToWorld * glm::vec4(m_l, m_b, m_f, 1.0f),
        lightToWorld * glm::vec4(m_r, m_b, m_f, 1.0f),
        lightToWorld * glm::vec4(m_r, m_b, m_n, 1.0f),

        lightToWorld * glm::vec4(m_l, m_t, m_n, 1.0f),
        lightToWorld * glm::vec4(m_l, m_t, m_f, 1.0f),
        lightToWorld * glm::vec4(m_r, m_t, m_f, 1.0f),
        lightToWorld * glm::vec4(m_r, m_t, m_n, 1.0f),

        lightToWorld * glm::vec4(m_l, m_t, m_n, 1.0f),
        lightToWorld * glm::vec4(m_l, m_b, m_n, 1.0f),
        lightToWorld * glm::vec4(m_l, m_b, m_f, 1.0f),
        lightToWorld * glm::vec4(m_l, m_t, m_f, 1.0f),

        lightToWorld * glm::vec4(m_r, m_t, m_n, 1.0f),
        lightToWorld * glm::vec4(m_r, m_b, m_n, 1.0f),
        lightToWorld * glm::vec4(m_r, m_b, m_f, 1.0f),
        lightToWorld * glm::vec4(m_r, m_t, m_f, 1.0f)
    };

    return positions;
}

std::vector<glm::vec4> ViewVolumeDisplay::createBufColorData() const {
    std::vector<glm::vec4> colors {
        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),

        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),

        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),

        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),

        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),

        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
    };

    return colors;
}
