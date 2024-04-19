#ifndef POSTPROCESSSHADER_H
#define POSTPROCESSSHADER_H

#include "shaderprogram.h"

class PostProcessShader : public ShaderProgram
{
public:
    PostProcessShader(OpenGLContext* context);
    virtual ~PostProcessShader();

    void draw(Drawable &d, int textureSlot);
};

#endif // POSTPROCESSSHADER_H
