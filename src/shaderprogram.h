#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <openglcontext.h>
#include <glm_includes.h>
#include <glm/glm.hpp>

#include "drawable.h"

class ShaderProgram
{
public:
    GLuint vertShader; // A handle for the vertex shader stored in this shader program
    GLuint fragShader; // A handle for the fragment shader stored in this shader program
    GLuint prog;       // A handle for the linked shader program stored in this class

    int attrPos; // A handle for the "in" vec4 representing vertex position in the vertex shader
    int attrNor; // A handle for the "in" vec4 representing vertex normal in the vertex shader
    int attrCol; // A handle for the "in" vec4 representing vertex color in the vertex shader
    int attrUV; // A handle for the "in" vec4 representing vertex uv in the vertex shader

    int attrPosOffset; // A handle for a vec3 used only in the instanced rendering shader

    int unifModel; // A handle for the "uniform" mat4 representing model matrix in the vertex shader
    int unifModelInvTr; // A handle for the "uniform" mat4 representing inverse transpose of the model matrix in the vertex shader
    int unifView;
    int unifViewProj; // A handle for the "uniform" mat4 representing combined projection and view matrices in the vertex shader
    int unifColor; // A handle for the "uniform" vec4 representing color of geometry in the vertex shader
    int unifUV;

    int unifSampler2D; // A handle to the "uniform" sampler2D that will be used to read the texture containing the scene render
    int unifTime; // A handle for the "uniform" float representing time in the shader
    int unifDimensions; // A handle for the "uniform" vec2 u_Dimensions
    int unifShadowMap;
    int unifModelLightProj;
    int unifUnhomScreenToTexture;
    int unifSunDirection;
    int unifEye;

public:
    ShaderProgram(OpenGLContext* context);

    // Sets up the requisite GL data and shaders from the given .glsl files.
    void create(const char *vertfile, const char *fragfile);

    // Tells our OpenGL context to use this shader to draw things.
    void useMe();

    // Pass the given model matrix to this shader on the GPU.
    void setModelMatrix(const glm::mat4 &model);

    void setViewMatrix(const glm::mat4 &v);

    // Pass the given Projection * View matrix to this shader on the GPU.
    void setViewProjMatrix(const glm::mat4 &vp);

    void setLightMatrix(const glm::mat4 &mlp);

    void setShadowMapMatrix(const glm::mat4 &mlps);

    // Pass the given color to this shader on the GPU.
    void setGeometryColor(glm::vec4 color);

    // Draw the given object to our screen using this ShaderProgram's shaders.
    void draw(Drawable &d);

    // Draw the given object to our screen multiple times using instanced rendering.
    void drawInstanced(InstancedDrawable &d);

    void drawInterleaved(Drawable &d, bool trans);

    // Draw Overlay
    void drawOverlay(Drawable &d);

    // Utility function used in create().
    char* textFileRead(const char*);

    // Utility function that prints any shader compilation errors to the console.
    void printShaderInfoLog(int shader);

    // Utility function that prints any shader linking errors to the console.
    void printLinkInfoLog(int prog);

    QString qTextFileRead(const char*);

    void setBlockTextureSampler(int slot);

    void setTime(int t);

    // Set dimension.
    void setDimensions(glm::ivec2 dims);

    void setSunDirection(glm::vec4 sunDirection);

protected:
    OpenGLContext* context;   // Since Qt's OpenGL support is done through classes like QOpenGLFunctions_3_2_Core,
                            // we need to pass our OpenGL context to the Drawable in order to call GL functions
                            // from within this class.
};


#endif // SHADERPROGRAM_H
