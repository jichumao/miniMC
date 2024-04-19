#include "cascadedshadowmap.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

#define ZERO_MEM(a) memset(a, 0, sizeof(a))

#define NUM_FRUSTUM_CORNERS 8

CascadedShadowMap::CascadedShadowMap(
    OpenGLContext *context,
    DirectionalLight &light,
    const Camera &camera
)
    : mp_context(context), m_fbo(0), m_width(0), m_height(0), m_light(light), m_lightDirection(light.m_direction), mr_camera(camera)
{
    ZERO_MEM(m_shadowMap);

    m_cascadeEnd[0] = camera.m_near_clip;
    m_cascadeEnd[1] = 25.0f,
    m_cascadeEnd[2] = 90.0f,
    m_cascadeEnd[3] = camera.m_far_clip;

    for (unsigned int i = 0 ; i < NUM_CASCADES; ++i) {

    }
}

CascadedShadowMap::~CascadedShadowMap() {
    if (m_fbo != 0) {
        mp_context->glDeleteFramebuffers(1, &m_fbo);
    }

    glDeleteTextures(ARRAY_SIZE_IN_ELEMENTS(m_shadowMap), m_shadowMap);
}

bool CascadedShadowMap::initialize(unsigned int width, unsigned int height, glm::mat4 matrix) {
    mp_context->glGenTextures(ARRAY_SIZE_IN_ELEMENTS(m_shadowMap), m_shadowMap);

    for (uint i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(m_shadowMap) ; i++) {
        mp_context->glBindTexture(GL_TEXTURE_2D, m_shadowMap[i]);
        // Each element is a single depth value in the range [0,1].
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    mp_context->glGenFramebuffers(1, &m_fbo);
    mp_context->glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    // Attach the shadow map texture to the framebuffer object's depth buffer. Last parameter
    // is mipmap level.
    mp_context->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMap[0], 0);
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
void CascadedShadowMap::bindToWrite(unsigned int cascade) {
    mp_context->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    mp_context->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMap[cascade], 0);
    mp_context->glEnable(GL_DEPTH_TEST);
    mp_context->glViewport(0, 0, m_width, m_height);
    mp_context->glClear(GL_DEPTH_BUFFER_BIT);
}

// Binds the shadow map texture to the given texture unit for sampling.
void CascadedShadowMap::bindToRead(
    GLenum firstCascadeTextureUnit,
    GLenum secondCascadeTextureUnit,
    GLenum thirdCascadeTextureUnit
) {
    mp_context->glActiveTexture(firstCascadeTextureUnit);
    mp_context->glBindTexture(GL_TEXTURE_2D, m_shadowMap[0]);

    mp_context->glActiveTexture(secondCascadeTextureUnit);
    mp_context->glBindTexture(GL_TEXTURE_2D, m_shadowMap[1]);

    mp_context->glActiveTexture(thirdCascadeTextureUnit);
    mp_context->glBindTexture(GL_TEXTURE_2D, m_shadowMap[2]);
}

void CascadedShadowMap::computeOrthoProj() {
    glm::mat4 LightM = m_light.getViewMatrix();

    float ar = mr_camera.m_height / mr_camera.m_width;

    float tanHalfHFOV = tanf(glm::radians(mr_camera.m_fovy / 2.0f));
    float tanHalfVFOV = tanf(glm::radians((mr_camera.m_fovy * ar) / 2.0f));

    for (unsigned int i = 0 ; i < NUM_CASCADES ; i++) {
        float xn = m_cascadeEnd[i]     * tanHalfHFOV;
        float xf = m_cascadeEnd[i + 1] * tanHalfHFOV;
        float yn = m_cascadeEnd[i]     * tanHalfVFOV;
        float yf = m_cascadeEnd[i + 1] * tanHalfVFOV;

        glm::vec4 frustumCorners[NUM_FRUSTUM_CORNERS] = {
            // near face
            glm::vec4(xn,   yn, m_cascadeEnd[i], 1.0),
            glm::vec4(-xn,  yn, m_cascadeEnd[i], 1.0),
            glm::vec4(xn,  -yn, m_cascadeEnd[i], 1.0),
            glm::vec4(-xn, -yn, m_cascadeEnd[i], 1.0),

            // far face
            glm::vec4(xf,   yf, m_cascadeEnd[i + 1], 1.0),
            glm::vec4(-xf,  yf, m_cascadeEnd[i + 1], 1.0),
            glm::vec4(xf,  -yf, m_cascadeEnd[i + 1], 1.0),
            glm::vec4(-xf, -yf, m_cascadeEnd[i + 1], 1.0)
        };

        glm::vec4 frustumCornersL[NUM_FRUSTUM_CORNERS];

        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::min();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::min();
        float minZ = std::numeric_limits<float>::max();
        float maxZ = std::numeric_limits<float>::min();

        glm::mat4 worldToCamView = glm::inverse(mr_camera.getView());
        for (unsigned int j = 0; j < NUM_FRUSTUM_CORNERS; j++) {
            glm::vec4 vW = worldToCamView * frustumCorners[j];
            frustumCornersL[j] = LightM * vW;

            minX = std::min(minX, frustumCornersL[j].x);
            maxX = std::max(maxX, frustumCornersL[j].x);
            minY = std::min(minY, frustumCornersL[j].y);
            maxY = std::max(maxY, frustumCornersL[j].y);
            minZ = std::min(minZ, frustumCornersL[j].z);
            maxZ = std::max(maxZ, frustumCornersL[j].z);
        }

        m_orthoVolumes[i].r = maxX;
        m_orthoVolumes[i].l = minX;
        m_orthoVolumes[i].b = minY;
        m_orthoVolumes[i].t = maxY;
        m_orthoVolumes[i].f = maxZ;
        m_orthoVolumes[i].n = minZ;
    }
}
