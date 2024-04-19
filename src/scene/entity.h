#pragma once
#include "glm_includes.h"

struct InputBundle {
    bool wPressed, aPressed, sPressed, dPressed;
    bool qPressed, ePressed; // accelerating along up vector
                             // only available when the flight mode is ON
    bool spacePressed; // add a vertical component to the player's velocity
                       // to make them jump
                       // only available when the flight mode is OFF
    float mouseX, mouseY;
    bool leftButtonPressed, rightButtonPressed;
    bool gravity; // determines if the player is subject to gravity or terrain collisions
                  // related to flight mode: gravity false = flight mode ON, and vice versa

    // For the player to specify whether the last leftButtonPressed and rightButtonPressed
    // have been handled.
    bool leftButtonPressProcessed;
    bool rightButtonPressProcessed;

    // terrain checking
    bool underWater;
    bool underLava;

    InputBundle()
        : wPressed(false), aPressed(false), sPressed(false),
          dPressed(false),
          qPressed(false), ePressed(false),
          spacePressed(false), mouseX(0.f), mouseY(0.f),
          leftButtonPressed(false), rightButtonPressed(false),
          gravity(false), leftButtonPressProcessed(false),
          rightButtonPressProcessed(false),
          underWater(false), underLava(false)
    {}
};

class Entity {
protected:
    // Vectors that define the axes of our local coordinate system
    glm::vec3 m_forward, m_right, m_up;
    // The origin of our local coordinate system
    glm::vec3 m_position;

public:
    // A readonly reference to position for external use
    const glm::vec3& mcr_position;

    // Various constructors
    Entity();
    Entity(glm::vec3 pos);
    Entity(const Entity &e);
    virtual ~Entity();

    inline const glm::vec3 &getPosition() const {
        return mcr_position;
    }

    // To be called by MyGL::tick()
    virtual void tick(float dT, InputBundle &input) = 0;

    // Translate along the given vector
    virtual void moveAlongVector(glm::vec3 dir);

    // Translate along one of our local axes
    virtual void moveForwardLocal(float amount);
    virtual void moveRightLocal(float amount);
    virtual void moveUpLocal(float amount);

    // Translate along one of the world axes
    virtual void moveForwardGlobal(float amount);
    virtual void moveRightGlobal(float amount);
    virtual void moveUpGlobal(float amount);

    // Rotate about one of our local axes
    virtual void rotateOnForwardLocal(float degrees);
    virtual void rotateOnRightLocal(float degrees);
    virtual void rotateOnUpLocal(float degrees);

    // Rotate about one of the world axes
    virtual void rotateOnForwardGlobal(float degrees);
    virtual void rotateOnRightGlobal(float degrees);
    virtual void rotateOnUpGlobal(float degrees);
};
