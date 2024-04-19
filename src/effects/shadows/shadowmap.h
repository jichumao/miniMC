#pragma once
#include "openglcontext.h"
#include "glm_includes.h"
#include "scene/camera.h"

class ShadowMap {
public:
    ShadowMap(OpenGLContext *context);

    bool initialize(unsigned int width, unsigned int height, glm::mat4 matrix);

    // Binds the shadow map framebuffer for writing the depth map to the
    // shadow map texture.
    void bindToWrite();

    // Binds the shadow map texture to the given texture unit for sampling.
    void bindToRead(GLenum textureUnit);

    inline glm::mat4 getUnhomScreenToTextureSpaceMatrix() const {
        return glm::mat4(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
        );
    }

private:
    OpenGLContext *mp_context;

    const Camera *mp_camera;

    glm::mat4 m_matrix;

    // The texture.
    GLuint m_shadowMap;

    // The framebuffer.
    GLuint m_fbo;

    unsigned int m_width;

    unsigned int m_height;
};
