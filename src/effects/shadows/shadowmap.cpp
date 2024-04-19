#include "shadowmap.h"

ShadowMap::ShadowMap(OpenGLContext *context)
    : mp_context(context), m_shadowMap(0), m_fbo(0), m_width(0), m_height(0)
{}

bool ShadowMap::initialize(unsigned int width, unsigned int height, glm::mat4 matrix) {
    mp_context->glGenTextures(1, &m_shadowMap);
    mp_context->glBindTexture(GL_TEXTURE_2D, m_shadowMap);
    // Each element is a single depth value in the range [0,1].
    mp_context->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    mp_context->glGenFramebuffers(1, &m_fbo);
    mp_context->glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    // Attach the shadow map texture to the framebuffer object's depth buffer. Last parameter
    // is mipmap level.
    mp_context->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMap, 0);
    mp_context->glReadBuffer(GL_NONE);

    if (mp_context->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        return false;
    }

    m_width = width;
    m_height = height;
    m_matrix = matrix;

    mp_context->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

// Binds the shadow map framebuffer for writing the depth map to the
// shadow map texture.
void ShadowMap::bindToWrite() {
    mp_context->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    mp_context->glEnable(GL_DEPTH_TEST);
    mp_context->glViewport(0, 0, m_width, m_height);
    mp_context->glClear(GL_DEPTH_BUFFER_BIT);
}

// Binds the shadow map texture to the given texture unit for sampling.
void ShadowMap::bindToRead(GLenum textureUnit) {
    mp_context->glActiveTexture(textureUnit);
    mp_context->glBindTexture(GL_TEXTURE_2D, m_shadowMap);
}