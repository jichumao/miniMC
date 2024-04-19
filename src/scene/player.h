#pragma once
#include "entity.h"
#include "camera.h"
#include "terrain.h"
#include "inventory.h"

enum class State {stop, move, max};

class Player : public Entity {
private:
    glm::vec3 m_velocity, m_acceleration;
    Camera m_camera;
    Terrain &mcr_terrain;

    bool flightMode; // determine if the flight mode is ON

    // determine if current movement collide in X or Z axis (with idx 0 or 2)
    bool checkXZCollision(int idx, const Terrain &terrain);

    // determine if current movement collide in Y axis (ground)
    bool checkYCollision(const Terrain &terrain);

    // implement jumping when the player is on ground &
    // swimming upwards when the player is under water/lava
    void jump(const Terrain &terrain, InputBundle &inputs);

    // remove the block within 3 units from the camera pos by left-clicking
    void removeBlock(InputBundle &inputs, Terrain &terrain);

    // place the block adjacent to the block face the player are looking at
    // within 3 units by right-clicking
    void placeBlock(InputBundle &inputs, Terrain &terrain);

    void processInputs(InputBundle &inputs);
    void computePhysics(float dT, const Terrain &terrain, InputBundle &inputs);

    Inventory inventory; // a temporary inventory setup


public:
    // Readonly public reference to our camera
    // for easy access from MyGL
    const Camera& mcr_camera;

    Player(glm::vec3 pos, Terrain &terrain);
    virtual ~Player() override;

    void setCameraWidthHeight(unsigned int w, unsigned int h);
    glm::mat4 getCameraViewProj() const;

    void tick(float dT, InputBundle &input) override;

    // Player overrides all of Entity's movement
    // functions so that it transforms its camera
    // by the same amount as it transforms itself.
    void moveAlongVector(glm::vec3 dir) override;
    void moveForwardLocal(float amount) override;
    void moveRightLocal(float amount) override;
    void moveUpLocal(float amount) override;
    void moveForwardGlobal(float amount) override;
    void moveRightGlobal(float amount) override;
    void moveUpGlobal(float amount) override;
    void rotateOnForwardLocal(float degrees) override;
    void rotateOnRightLocal(float degrees) override;
    void rotateOnUpLocal(float degrees) override;
    void rotateOnForwardGlobal(float degrees) override;
    void rotateOnRightGlobal(float degrees) override;
    void rotateOnUpGlobal(float degrees) override;

    // For sending the Player's data to the GUI
    // for display
    QString posAsQString() const;
    QString velAsQString() const;
    QString accAsQString() const;
    QString lookAsQString() const;

    void toggleFlightMode(); // toggle between flight mode and normal mode
    bool isButtonPressed(InputBundle &inputs); // determine if any key related to player movement
                                               // is pressed
    bool isMoving(bool y); // determine if the player is moving
    State currentState(float dT, InputBundle &inputs); // determine the current kinematics

    BlockType blockColliding;

    void rotateCamera(InputBundle &input); // rotate camera view based on cursor position

    bool gridMarch(glm::vec3 rayOrigin, glm::vec3 rayDirection, const Terrain &terrain,
                   float *out_dist, glm::ivec3 *out_blockHit);
    bool gridMarchPrevBlock(glm::vec3 rayOrigin, glm::vec3 rayDirection, const Terrain &terrain,
                            glm::ivec3 *out_prevBlock, glm::ivec3 *out_blockHit);

    bool isUnderWater(const Terrain &terrain, InputBundle &inputs);
    bool isUnderLava(const Terrain &terrain, InputBundle &inputs);

    // determine if the given position is liquid
    bool isLiquid(const Terrain &terrain, glm::ivec3* pos);

    bool isWalking(); // determine if the player is walking
};
