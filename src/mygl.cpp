#include "mygl.h"
#include <glm_includes.h>
#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <QDateTime>
#include "math/transform.h"
#include "debug/viewvolumedisplay.h"

#define TERRAIN_DRAW_RADIUS 10
#define TERRAIN_EXPANSION_RADIUS 5

MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      m_worldAxes(this),
      m_progLambert(this), m_progFlat(this), m_progInstanced(this),
      m_progUnderwater(this), m_progLava(this), m_progNoOp(this), m_quad(this), m_skyQuad(this, false),
      m_progShadowMap(this), m_progShadowMapQuad(this), m_progSky(this),
      m_frameBuffer(this, this->width(), this->height(), this->devicePixelRatio()),
      m_terrain(this, TERRAIN_DRAW_RADIUS, TERRAIN_EXPANSION_RADIUS),
      m_isUnderWater(false), m_isUnderLava(false),
    //   m_player(glm::vec3(48.f, 148.f, 48.f), m_terrain), m_time(0.f),
      m_player(glm::vec3(0.f, 148.f, 0.f), m_terrain), m_time(0.f),
      m_shadowMap(this), m_shadowMapQuad(this),
      m_spotLight(&m_player.mcr_camera), m_directionalLight(),
      prevFrameTime(QDateTime::currentMSecsSinceEpoch())
{
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
    // Tell the timer to redraw 60 times per second
    m_timer.start(16);
    setFocusPolicy(Qt::ClickFocus);

    m_inputs = InputBundle();

    setMouseTracking(true); // MyGL will track the mouse's movements even if a mouse button is not pressed
    setCursor(Qt::CrossCursor); // Set a cross cursor

    // Setup Sounds
    mainTheme.setSource(QUrl::fromLocalFile(":/sounds/main.wav"));
    mainTheme.setLoopCount(QSoundEffect::Infinite);
    mainTheme.setVolume(0.2f);
    mainTheme.play();

    walk.setSource(QUrl::fromLocalFile(":/sounds/walk.wav"));
    walk.setLoopCount(QSoundEffect::Infinite);
    walk.setVolume(0.5f);

    underwaterSound.setSource(QUrl::fromLocalFile(":/sounds/underwater.wav"));
    underwaterSound.setLoopCount(QSoundEffect::Infinite);
    underwaterSound.setVolume(0.5f);

    lavaSound.setSource(QUrl::fromLocalFile(":/sounds/lava.wav"));
    lavaSound.setLoopCount(QSoundEffect::Infinite);
    lavaSound.setVolume(0.5f);
}

MyGL::~MyGL() {
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    m_quad.destroyVBOdata();
    m_skyQuad.destroyVBOdata();
    m_frameBuffer.destroy();
}

void MyGL::moveMouseToCenter() {
    QCursor::setPos(this->mapToGlobal(QPoint(width() / 2, height() / 2)));
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.37f, 0.74f, 1.0f, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    // Create the instance of the post-process quad
    m_quad.createVBOdata();
    m_skyQuad.createVBOdata();

    //Create the instance of the world axes
    m_worldAxes.createVBOdata();

    // Initiailize frame buffer
    m_frameBuffer.create();

    // Create and set up shaders.
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");
    m_progInstanced.create(":/glsl/instanced.vert.glsl", ":/glsl/lambert.frag.glsl");
    m_progUnderwater.create(":/glsl/post/overlay.vert.glsl", ":/glsl/post/underwater.frag.glsl");
    m_progLava.create(":/glsl/post/overlay.vert.glsl", ":/glsl/post/lava.frag.glsl");
    m_progNoOp.create(":/glsl/post/overlay.vert.glsl", ":/glsl/post/overlay.frag.glsl");
    m_progShadowMap.create(":/glsl/shadow.vert.glsl", ":/glsl/shadow.frag.glsl");
    m_progSky.create(":/glsl/sky.vert.glsl", ":/glsl/sky.frag.glsl");

    // Set a color with which to draw geometry.
    // This will ultimately not be used when you change
    // your program to render Chunks with vertex colors
    // and UV coordinates
    m_progLambert.setGeometryColor(glm::vec4(0,1,0,1));

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize the terrain around the player's position.
    // initTexture() is included.
    m_terrain.initialize(m_player.getPosition());

    m_shadowMap.initialize(this->width(), this->height(), m_player.mcr_camera.getViewProj());

    m_shadowMapQuad.createVBOdata();
}

void MyGL::resizeGL(int w, int h) {
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_player.setCameraWidthHeight(static_cast<unsigned int>(w), static_cast<unsigned int>(h));
    glm::mat4 viewproj = m_player.mcr_camera.getViewProj();
    glm::mat4 view = m_player.mcr_camera.getView();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    m_progLambert.setViewMatrix(view);
    m_progFlat.setViewProjMatrix(viewproj);
    m_progUnderwater.setViewProjMatrix(viewproj);
    // TODO: resize shadow map.
    // m_progShadowMap.setViewProjMatrix(viewproj);
    m_progSky.setViewProjMatrix(glm::inverse(viewproj));

    m_frameBuffer.resize(this->width(), this->height(), this->devicePixelRatio());
    m_frameBuffer.destroy();
    m_frameBuffer.create();

    m_progNoOp.setDimensions(glm::ivec2(w * this->devicePixelRatio(), h * this->devicePixelRatio()));
    m_progUnderwater.setDimensions(glm::ivec2(w * this->devicePixelRatio(), h * this->devicePixelRatio()));
    m_progLava.setDimensions(glm::ivec2(w * this->devicePixelRatio(), h * this->devicePixelRatio()));
    // TODO: resize shadow map.
    // m_progShadowMap.setDimensions(glm::ivec2(w * this->devicePixelRatio(), h * this->devicePixelRatio()));

    m_frameBuffer.resize(this->width(), this->height(), this->devicePixelRatio());
    m_frameBuffer.destroy();
    m_frameBuffer.create();

    m_progSky.useMe();
    glUniform2i(m_progSky.unifDimensions, width(), height());
    glUniform3f(m_progSky.unifEye, m_player.mcr_camera.getCurrPos().x, m_player.mcr_camera.getCurrPos().y, m_player.mcr_camera.getCurrPos().z);

    printGLErrorLog();
}


// MyGL's constructor links tick() to a timer that fires 60 times per second.
// We're treating MyGL as our game engine class, so we're going to perform
// all per-frame actions here, such as performing physics updates on all
// entities in the scene.
void MyGL::tick() {
    // Compute the time delta.
    long long currFrameTime = QDateTime::currentMSecsSinceEpoch();
    if (prevFrameTime == 0) {
        // TODO: need to initialize prevFrameTime to a reasonable value,
        // so that first deltTime isn't garbage.
        prevFrameTime = currFrameTime;
        return;
    }

    float deltaTime = (currFrameTime - prevFrameTime) / 1000.f;

    prevFrameTime = currFrameTime;
    m_player.tick(deltaTime, m_inputs);

    // Expand the terrain only after the latest player position
    // is computed.
    m_terrain.expand(m_player.mcr_camera.getPosition());

    m_isUnderWater = m_player.isUnderWater(m_terrain, m_inputs);
    m_isUnderLava = m_player.isUnderLava(m_terrain, m_inputs);

    // Calls paintGL() as part of a larger QOpenGLWidget pipeline.
    update();

    // Update the info in the secondary window displaying player data.
    sendPlayerDataToGUI();
}

void MyGL::sendPlayerDataToGUI() const {
    emit sig_sendPlayerPos(m_player.posAsQString());
    emit sig_sendPlayerVel(m_player.velAsQString());
    emit sig_sendPlayerAcc(m_player.accAsQString());
    emit sig_sendPlayerLook(m_player.lookAsQString());
    glm::vec2 pPos(m_player.mcr_position.x, m_player.mcr_position.z);
    glm::ivec2 chunk(16 * glm::ivec2(glm::floor(pPos / 16.f)));
    glm::ivec2 zone(64 * glm::ivec2(glm::floor(pPos / 64.f)));
    emit sig_sendPlayerChunk(QString::fromStdString("( " + std::to_string(chunk.x) + ", " + std::to_string(chunk.y) + " )"));
    emit sig_sendPlayerTerrainZone(QString::fromStdString("( " + std::to_string(zone.x) + ", " + std::to_string(zone.y) + " )"));
}

void MyGL::playWalkingSounds(){
    if (m_player.blockColliding == GRASS || m_player.blockColliding == STONE ||
        m_player.blockColliding == DIRT || m_player.blockColliding == SNOW ||
        m_player.blockColliding == SAND || m_player.blockColliding == SNOWGRASS ||
        m_player.blockColliding == ICE) {
        if (m_player.isWalking()) {
            if (!walk.isPlaying()) {
                walk.play();
            }
        }
        else {
            walk.stop();
        }
    }
    else {
        stopWalkingSounds();
    }
}

void MyGL::stopWalkingSounds(){
    if (walk.isPlaying()) {
        walk.stop();
    }
}

// This function is called whenever update() is called.
// MyGL's constructor links update() to a timer that fires 60 times per second,
// so paintGL() called at a rate of 60 frames per second.
void MyGL::paintGL() {
    // Clear the screen so that we only see newly drawn images.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind FrameBuffer for Overlay
    m_frameBuffer.bindFrameBuffer();
    glViewport(0, 0, this->width() * this->devicePixelRatio(), this->height() * this->devicePixelRatio());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shadowPass();

    mainPass();

    glDisable(GL_DEPTH_TEST);
    m_progFlat.setModelMatrix(glm::mat4());
    m_progFlat.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    //m_progFlat.draw(m_worldAxes);
    glEnable(GL_DEPTH_TEST);

    m_progLambert.setTime(m_time);
    m_progFlat.setTime(m_time);
    m_progInstanced.setTime(m_time);
    m_progLava.setTime(m_time);
    m_progUnderwater.setTime(m_time);

   glBindFramebuffer(GL_FRAMEBUFFER, this->defaultFramebufferObject());
   glViewport(0,0,this->width() * this->devicePixelRatio(), this->height() * this->devicePixelRatio());
//   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   m_frameBuffer.bindToTextureSlot(2);

   //playWalkingSounds();

   if (m_isUnderWater) {
       stopWalkingSounds();
       if (!underwaterSound.isPlaying()) {
           underwaterSound.play();
       }
       m_progUnderwater.setBlockTextureSampler(m_frameBuffer.getTextureSlot());
       m_progUnderwater.drawOverlay(m_quad);
   }
    else if (m_isUnderLava) {
       stopWalkingSounds();
       if (!lavaSound.isPlaying()) {
           lavaSound.play();
       }
       m_progLava.setBlockTextureSampler(m_frameBuffer.getTextureSlot());
       m_progLava.drawOverlay(m_quad);
   } else {
       if (underwaterSound.isPlaying()) {
           underwaterSound.stop();
       }
       if (lavaSound.isPlaying()) {
           lavaSound.stop();
       }

       playWalkingSounds();
       m_progNoOp.setBlockTextureSampler(m_frameBuffer.getTextureSlot());
       // m_progNoOp.drawOverlay(m_quad);
   }

    m_time++;
}

void MyGL::renderTerrain(ShaderProgram &prog) {
    m_terrain.draw(m_player.getPosition(), &prog);
}

void MyGL::shadowPass() {
    m_shadowMap.bindToWrite();

    m_sinT = sin(m_time * 0.001);
    m_cosT = cos(m_time * 0.001);
    m_sinT = 0.5 + 0.01*sin(m_time * 0.1);
    m_cosT = 0.5 + 0.01*sin(m_time * 0.1);
    m_sinT = 0.2;
    m_cosT = 0.9;

    glm::vec3 playerPos = m_player.getPosition();
    glm::ivec2 chunkIndex = m_terrain.getZoneIndexAt(playerPos.x, playerPos.z);
    m_lightTargetPos = glm::vec3(chunkIndex.s, playerPos.y, chunkIndex.t);

    m_progShadowMap.setViewProjMatrix(
        m_directionalLight.getLightSpaceMatrix(m_sinT, m_cosT, m_lightTargetPos)
    );

    // glEnable(GL_POLYGON_OFFSET_FILL);
    // glPolygonOffset(10.1f, 4.0f);
    glCullFace(GL_FRONT);
    renderTerrain(m_progShadowMap);
    glCullFace(GL_BACK);
    // glDisable(GL_POLYGON_OFFSET_FILL);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Restore viewport.
    glViewport(0, 0, this->width() * this->devicePixelRatio(), this->height() * this->devicePixelRatio());
}

void MyGL::mainPass() {
    static unsigned int time = 0;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_progLambert.setViewProjMatrix(
        m_player.mcr_camera.getViewProj()
    );

    m_progLambert.setViewMatrix(
        m_player.mcr_camera.getView()
    );

    m_shadowMap.bindToRead(GL_TEXTURE1);

    m_progLambert.setLightMatrix(
        m_directionalLight.getLightSpaceMatrix(m_sinT, m_cosT, m_lightTargetPos)
    );

    m_progLambert.setShadowMapMatrix(
        m_shadowMap.getUnhomScreenToTextureSpaceMatrix()
    );

    DirectionalLightVolume dlv = m_directionalLight.getViewVolume(m_sinT, m_cosT, m_lightTargetPos);

    m_progLambert.setSunDirection(glm::vec4(dlv.direction, 0.0f));

    // // Debug.
    // ViewVolumeDisplay vv(this, dlv);
    // vv.createVBOdata();
    // // glDisable(GL_DEPTH_TEST);
    // m_progFlat.draw(vv);
    // // glEnable(GL_DEPTH_TEST);

    m_progSky.useMe();
    // m_progSky.setModelMatrix(
    //     // glm::translate(glm::mat4(1.0f), glm::vec3(0, 120, 0))
    //     // * 
    //     glm::scale(glm::mat4(1.0f), glm::vec3(100, 100, 1))
    // );
    m_progSky.setViewProjMatrix(glm::inverse(m_player.mcr_camera.getViewProj()));
    // m_progSky.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    glUniform3f(m_progSky.unifEye, m_player.mcr_camera.getCurrPos().x, m_player.mcr_camera.getCurrPos().y, m_player.mcr_camera.getCurrPos().z);
    glUniform1f(m_progSky.unifTime, ++time);
    glUniform2i(m_progSky.unifDimensions, width(), height());
    // glDepthRange(0.999, 1);
    m_progSky.draw(m_skyQuad);
    // glDepthRange(-1, 1);

    m_progLambert.useMe();
    m_terrain.bindTexture(0);
    renderTerrain(m_progLambert);
}

void MyGL::keyPressEvent(QKeyEvent *e) {
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used, but I really dislike their
    // syntax so I chose to be lazy and use a long
    // chain of if statements instead
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        m_player.rotateOnUpGlobal(-amount);
    } else if (e->key() == Qt::Key_Left) {
        m_player.rotateOnUpGlobal(amount);
    } else if (e->key() == Qt::Key_Up) {
        m_player.rotateOnRightLocal(-amount);
    } else if (e->key() == Qt::Key_Down) {
        m_player.rotateOnRightLocal(amount);
    } else if (e->key() == Qt::Key_W) {
        m_inputs.wPressed = true;
    } else if (e->key() == Qt::Key_S) {
        m_inputs.sPressed = true;
    } else if (e->key() == Qt::Key_D) {
        m_inputs.dPressed = true;
    } else if (e->key() == Qt::Key_A) {
        m_inputs.aPressed = true;
    } else if (e->key() == Qt::Key_Q) {
        m_inputs.qPressed = true;
    } else if (e->key() == Qt::Key_E) {
        m_inputs.ePressed = true;
    } else if (e->key() == Qt::Key_F) {
        m_player.toggleFlightMode();
    } else if (e->key() == Qt::Key_Space) {
        m_inputs.spacePressed = true;
    } else if (e->key() == Qt::Key_L) {
        glm::vec3 pos = m_player.getPosition();
        m_terrain.generateLSystem(pos);
    }
}

void MyGL::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_W) {
        m_inputs.wPressed = false;
    } else if (e->key() == Qt::Key_S) {
        m_inputs.sPressed = false;
    } else if (e->key() == Qt::Key_D) {
        m_inputs.dPressed = false;
    } else if (e->key() == Qt::Key_A) {
        m_inputs.aPressed = false;
    } else if (e->key() == Qt::Key_Q) {
        m_inputs.qPressed = false;
    } else if (e->key() == Qt::Key_E) {
        m_inputs.ePressed = false;
    } else if (e->key() == Qt::Key_Space) {
        m_inputs.spacePressed = false;
    }
}

void MyGL::mouseMoveEvent(QMouseEvent *e) {
    // TODO
    m_inputs.mouseX = e->pos().x();
    m_inputs.mouseY = e->pos().y();

    m_player.rotateCamera(m_inputs);
    moveMouseToCenter();
}

void MyGL::mousePressEvent(QMouseEvent *e) {
    // TODO
    switch (e->button()) {
    case (Qt::LeftButton):
        m_inputs.leftButtonPressed = true;
        m_inputs.leftButtonPressProcessed = false;
        break;
    case (Qt::RightButton):
        m_inputs.rightButtonPressed = true;
        m_inputs.rightButtonPressProcessed = false;
        break;
    default:
        return;
    }
}

void MyGL::mouseReleaseEvent(QMouseEvent *e) {
    switch (e->button()) {
    case (Qt::LeftButton):
        m_inputs.leftButtonPressed = false;
        break;
    case (Qt::RightButton):
        m_inputs.rightButtonPressed = false;
        break;
    default:
        return;
    }
}
