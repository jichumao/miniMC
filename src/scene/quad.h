#pragma once

#include "drawable.h"

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class Quad : public Drawable
{
public:
    Quad(OpenGLContext* context, bool isPost = true);
    virtual void createVBOdata();
private:
    bool m_isPost;
};
