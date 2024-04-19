#pragma once
#include "openglcontext.h"
#include "glm_includes.h"
#include "scene/camera.h"
#include "lighting/lights/directional.h"

#define NUM_CASCADES 3

class CascadedShadowMap {
public:
    CascadedShadowMap(OpenGLContext *context, DirectionalLight &light, const Camera &camera);

    ~CascadedShadowMap();

    bool initialize(unsigned int width, unsigned int height, glm::mat4 matrix);

    // Binds the shadow map framebuffer for writing the depth map to the
    // shadow map texture.
    void bindToWrite(unsigned int cascade);

    // Binds the shadow map texture to the given texture unit for sampling.
    void bindToRead(
        GLenum firstCascadeTextureUnit,
        GLenum secondCascadeTextureUnit,
        GLenum thirdCascadeTextureUnit
    );

    inline glm::mat4 getUnhomScreenToTextureSpaceMatrix() const {
        return glm::mat4(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
        );
    }

    void computeOrthoProj();

private:
    OpenGLContext *mp_context;

    const Camera &mr_camera;

    glm::mat4 m_matrix;

    // The texture.
    GLuint m_shadowMap[NUM_CASCADES];

    // The framebuffer.
    GLuint m_fbo;

    unsigned int m_width;

    unsigned int m_height;

    float m_cascadeEnd[NUM_CASCADES + 1];

    glm::vec3 m_lightDirection;

    DirectionalLight &m_light;

    DirectionalLightVolume m_orthoVolumes[NUM_CASCADES];
};