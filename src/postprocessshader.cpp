#include "postprocessshader.h"
#include <QDateTime>

PostProcessShader::PostProcessShader(OpenGLContext *context) : ShaderProgram(context) {}

PostProcessShader::~PostProcessShader() {}

void PostProcessShader::draw(Drawable& d, int textureSlot = 0)
{


    context->printGLErrorLog();
}
