#include "player.h"
#include <QString>

Player::Player(glm::vec3 pos, Terrain &terrain)
    : Entity(pos), m_velocity(0,0,0), m_acceleration(0,0,0),
      m_camera(pos + glm::vec3(0, 1.5f, 0)), mcr_terrain(terrain),
      flightMode(true),
      mcr_camera(m_camera)
{}

Player::~Player()
{}

void Player::tick(float dT, InputBundle &input) {
    removeBlock(input, mcr_terrain);
    placeBlock(input, mcr_terrain);
    processInputs(input);
    computePhysics(dT, mcr_terrain, input);
}

void Player::processInputs(InputBundle &inputs) {
    // TODO: Update the Player's velocity and acceleration based on the
    // state of the inputs.
    glm::vec3 accel(0.f);
    glm::vec3 forward(m_camera.getForward());
    glm::vec3 right(m_camera.getRight());

    if (!flightMode) {
        forward.y = 0.f;
        right.y = 0.f;
    }

    // set traditional WASD controls, which is independent of
    // whether the flight mode is toggled on
    if (inputs.wPressed) {
        accel += forward;
    }
    if (inputs.aPressed) {
        accel -= right;
    }
    if (inputs.sPressed) {
        accel -= forward;
    }
    if (inputs.dPressed) {
        accel += right;
    }

    // in flight mode, enable the player to move along the up vector by pressing E or Q
    if (flightMode) {
        if (inputs.ePressed) {
            accel += glm::vec3(0.f, 1.f, 0.f);
        }
        if (inputs.qPressed) {
            accel -= glm::vec3(0.f, 1.f, 0.f);
        }
    } else {
        // when the flight mode is not active, set acceleration on Y axis to 0
        accel[1] = 0.f;
    }

    // TODO: test the default accelaration factor (current: 30)

    // normalize and set velocity and accelaration
    // if any button related to player movement is pressed, apply the accelaration vector obtained above
    if (isButtonPressed(inputs) && glm::length(accel) > 0) {
        m_acceleration = glm::normalize(accel) * 30.f;
    } else if (isMoving(true) && flightMode) {
        // in flight mode, if there is no movement command from player, set a negative accelaration
        // based on the current velocity to slow the player down
        m_acceleration = glm::normalize(-m_velocity) * 30.f;
    } else if (isMoving(false) && !flightMode) {
        // in gravity mode, if there is no movement command from player, set a negative accelaration
        // based on the current velocity with X and Z direction only to slow the player down
        m_acceleration = glm::normalize(glm::vec3(-m_velocity[0], 0.f, -m_velocity[2])) * 30.f;
    } else {
        // otherwise, apply no accelaration
        m_acceleration = glm::vec3(0.f);
    }

    // when the flight mode is not active, set a default gravitational acceleration
    // at the surface of the Earth
    if (!flightMode) {
        m_acceleration[1] = -9.8067f;
    }
}

void Player::computePhysics(float dT, const Terrain &terrain, InputBundle &inputs) {
    // TODO: Update the Player's position based on its acceleration
    // and velocity, and also perform collision detection.
    glm::vec3 displacement(0.f);

    // variable to adjust the velocity while under water/lava
    float swim = 1.f;

    switch (currentState(dT, inputs)){
    case (State::max):
        // TODO: test the default velocity factor (current: 20)

        // if the state is marked max, prevent the player from further accelarating
        m_velocity = glm::normalize(m_velocity + m_acceleration * dT) * 20.f;
        m_acceleration = glm::vec3(0.f);
        break;
    case (State::stop):
        // if the state is stop, stop the player
        m_velocity = glm::vec3(0.f);
        break;
    case (State::move):
        // otherwise, determine the velocity
        m_velocity += m_acceleration * dT;
        break;
    }

    if (!checkXZCollision(0, terrain)) {
        m_velocity[0] = 0.f;
    }
    if (!checkXZCollision(2, terrain)) {
        m_velocity[2] = 0.f;
    }
    if (!checkYCollision(terrain)) {
        m_velocity[1] = 0.f;
    }

    if (inputs.spacePressed) {
        jump(terrain, inputs);
    }

    if (!flightMode && (isUnderWater(terrain, inputs) || isUnderLava(terrain, inputs))) {
        swim = 2.f/3.f;
    }

    m_velocity = swim * m_velocity;
    displacement = m_velocity * dT;
    moveAlongVector(displacement);
}

void Player::setCameraWidthHeight(unsigned int w, unsigned int h) {
    m_camera.setWidthHeight(w, h);
}

void Player::moveAlongVector(glm::vec3 dir) {
    Entity::moveAlongVector(dir);
    m_camera.moveAlongVector(dir);
}
void Player::moveForwardLocal(float amount) {
    Entity::moveForwardLocal(amount);
    m_camera.moveForwardLocal(amount);
}
void Player::moveRightLocal(float amount) {
    Entity::moveRightLocal(amount);
    m_camera.moveRightLocal(amount);
}
void Player::moveUpLocal(float amount) {
    Entity::moveUpLocal(amount);
    m_camera.moveUpLocal(amount);
}
void Player::moveForwardGlobal(float amount) {
    Entity::moveForwardGlobal(amount);
    m_camera.moveForwardGlobal(amount);
}
void Player::moveRightGlobal(float amount) {
    Entity::moveRightGlobal(amount);
    m_camera.moveRightGlobal(amount);
}
void Player::moveUpGlobal(float amount) {
    Entity::moveUpGlobal(amount);
    m_camera.moveUpGlobal(amount);
}
void Player::rotateOnForwardLocal(float degrees) {
    Entity::rotateOnForwardLocal(degrees);
    m_camera.rotateOnForwardLocal(degrees);
}
void Player::rotateOnRightLocal(float degrees) {
    Entity::rotateOnRightLocal(degrees);
    m_camera.rotateOnRightLocal(degrees);
}
void Player::rotateOnUpLocal(float degrees) {
    Entity::rotateOnUpLocal(degrees);
    m_camera.rotateOnUpLocal(degrees);
}
void Player::rotateOnForwardGlobal(float degrees) {
    Entity::rotateOnForwardGlobal(degrees);
    m_camera.rotateOnForwardGlobal(degrees);
}
void Player::rotateOnRightGlobal(float degrees) {
    Entity::rotateOnRightGlobal(degrees);
    m_camera.rotateOnRightGlobal(degrees);
}
void Player::rotateOnUpGlobal(float degrees) {
    Entity::rotateOnUpGlobal(degrees);
    m_camera.rotateOnUpGlobal(degrees);
}

QString Player::posAsQString() const {
    std::string str("( " + std::to_string(m_position.x) + ", " + std::to_string(m_position.y) + ", " + std::to_string(m_position.z) + ")");
    return QString::fromStdString(str);
}
QString Player::velAsQString() const {
    std::string str("( " + std::to_string(m_velocity.x) + ", " + std::to_string(m_velocity.y) + ", " + std::to_string(m_velocity.z) + ")");
    return QString::fromStdString(str);
}
QString Player::accAsQString() const {
    std::string str("( " + std::to_string(m_acceleration.x) + ", " + std::to_string(m_acceleration.y) + ", " + std::to_string(m_acceleration.z) + ")");
    return QString::fromStdString(str);
}
QString Player::lookAsQString() const {
    std::string str("( " + std::to_string(m_forward.x) + ", " + std::to_string(m_forward.y) + ", " + std::to_string(m_forward.z) + ")");
    return QString::fromStdString(str);
}

void Player::toggleFlightMode() {
    if (flightMode) {
        flightMode = false;
    } else {
        flightMode = true;
        blockColliding = EMPTY;
    }
}

bool Player::isButtonPressed(InputBundle &inputs) {
    if (!flightMode) {
        return inputs.wPressed || inputs.aPressed || inputs.sPressed || inputs.dPressed ||
               inputs.spacePressed;
    }

    return inputs.wPressed || inputs.aPressed || inputs.sPressed || inputs.dPressed ||
           inputs.qPressed || inputs.ePressed;

}

bool Player::isMoving(bool y)
{
    for (int i = 0; i < 3; ++i) {
        // determine if y-velocity needs to be checked (for gravity mode)
        if (i == 1 && !y) {
            continue;
        }
        if (abs(m_velocity[i]) > 0) {
            return true;
        }
    }
    return false;
}

State Player::currentState(float dT, InputBundle &inputs) {
    // TODO: test the default velocity maximum (current: 20)
    // if the current velocity is greater than the set maximum, mark the state as max
    if (glm::length(m_velocity + m_acceleration * dT) >= 20.f) {
        return State::max;
    }

    // if the current accelaration will overweigh the current velocity to stop the player
    // while there is no further movement command from the player, mark the state as stop
    if (glm::dot(m_velocity, m_acceleration) < 0 &&
        glm::length(m_acceleration * dT) > glm::length(m_velocity) && ! isButtonPressed(inputs)) {
        return State::stop;
    }

    // otherwise, mark the state as move
    return State::move;
}

bool Player::checkXZCollision(int idx, const Terrain &terrain) {
    if (idx != 0 && idx != 2) {
        return true;
    }

    if (flightMode) {
        return true;
    }

    glm::ivec3 blockHit(0);
    float d = 0.f;

    // check the forward direction
    glm::vec3 forward(0.f);
    if (m_forward[idx] > 0.f) {
        forward[idx] = 0.5f;
    } else if (m_forward[idx] < 0.f) {
        forward[idx] = -0.5f;
    } else {
        return false;
    }

    float horizontalTolerance = 0.15f; // 1/2 - 1/2 * √2/2
    std::array<glm::vec3, 3> corner;

    for (int i = 0; i < 4; ++i) {
        float deg = 45.f + i * 90.f;
        float rad = glm::radians(deg);

        // check whether the orientation of player and the velocity is the same
        float direction = (deg >= 90.f && deg <= 270.f) ? -1.f : 1.f;

        glm::vec3 forwardDeg = glm::vec3(glm::rotate(glm::mat4(), rad, glm::vec3(0, 1, 0))
                                         * glm::vec4(forward, 0.f));
        glm::vec3 cameraCorner = m_camera.getCurrPos() + forwardDeg + glm::vec3(0.f, 0.5f, 0.f);
        glm::vec3 midCorner(cameraCorner - glm::vec3(0.f, 1.f, 0.f));
        glm::vec3 playerCorner(cameraCorner - glm::vec3(0.f, 2.f, 0.f));
        corner[0] = cameraCorner;
        corner[1] = midCorner;
        corner[2] = playerCorner;

        for (auto& c: corner) {
            bool cornerHit = gridMarch(c, direction * forward, terrain, &d, &blockHit);
            if (cornerHit && d < horizontalTolerance && m_velocity[idx] * forwardDeg[idx] >= 0
                && !isLiquid(terrain, &blockHit)) {
                return false;
            }
        }
    }

    return true;
}

bool Player::checkYCollision(const Terrain &terrain) {
    if (flightMode) {
        return true;
    }

    glm::ivec3 hitGround(0);
    glm::ivec3 hitCeiling(0);
    float outDistanceNegY = 0.f;
    float outDistancePosY = 0.f;

    // check if the player touches the ground
    float negYTolerance = 0.4f;
    float posYTolerance = 0.4f;
    bool playerHitGround = gridMarch(m_position, glm::vec3(0.f, -1.f, 0.f), terrain,
                                     &outDistanceNegY, &hitGround);
    bool playerHitCeiling = gridMarch(m_camera.getCurrPos(), glm::vec3(0.f, 1.f, 0.f),
                                      terrain, &outDistancePosY, &hitCeiling);
    blockColliding = terrain.getBlockAt(hitGround.x, hitGround.y, hitGround.z);

    if (playerHitGround && outDistanceNegY < negYTolerance && m_velocity[1] <= 0 &&
        !isLiquid(terrain, &hitGround)) {
        return false;
    }

    if (playerHitCeiling && outDistancePosY < posYTolerance && m_velocity[1] >= 0 &&
        !isLiquid(terrain, &hitCeiling)) {
        return false;
    }

    return true;
}

void Player::rotateCamera(InputBundle &input) {
    // evaluate differences in theta and phi
    float dTheta = input.mouseX - m_camera.getScreenCenter()[0];
    float dPhi = input.mouseY - m_camera.getScreenCenter()[1];

    // clamp theta and phi
    dTheta = std::clamp(dTheta, -360.f, 360.f);
    dPhi = std::clamp(dPhi, -360.f, 360.f);

    // adjust the angle
    float scalar = 0.1f;

    // avoid phi falling out of range (-90 ~ 90)
    float tolerance = 0.98f;
    if (m_camera.getForward()[1] >= tolerance && dPhi < 0) {
        rotateOnRightLocal(0.f);
    } else if (m_camera.getForward()[1] <= -tolerance && dPhi > 0) {
        rotateOnRightLocal(0.f);
    } else {
        rotateOnRightLocal(-dPhi * scalar);
    }

    // apply no restriction on theta
    rotateOnUpGlobal(-dTheta * scalar);

}

// From Lecture Slides
bool Player::gridMarch(glm::vec3 rayOrigin, glm::vec3 rayDirection, const Terrain &terrain, float *out_dist, glm::ivec3 *out_blockHit) {
    float maxLen = glm::length(rayDirection); // Farthest we search
    glm::ivec3 currCell = glm::ivec3(glm::floor(rayOrigin));
    rayDirection = glm::normalize(rayDirection); // Now all t values represent world dist.

    float curr_t = 0.f;
    while (curr_t < maxLen) {
        float min_t = glm::sqrt(3.f);
        float interfaceAxis = -1; // Track axis for which t is smallest
        for (int i = 0; i < 3; ++i) { // Iterate over the three axes
            if (rayDirection[i] != 0) { // Is ray parallel to axis i?
                float offset = glm::max(0.f, glm::sign(rayDirection[i]));
                // If the player is *exactly* on an interface then
                // they'll never move if they're looking in a negative direction
                if (currCell[i] == rayOrigin[i] && offset == 0.f) {
                    offset = -1.f;
                }
                int nextIntercept = currCell[i] + offset;
                float axis_t = (nextIntercept - rayOrigin[i]) / rayDirection[i];
                axis_t = glm::min(axis_t, maxLen); // Clamp to max len to avoid super out of bounds errors
                if (axis_t < min_t) {
                    min_t = axis_t;
                    interfaceAxis = i;
                }
            }
        }

        if (interfaceAxis == -1) {
            throw std::out_of_range("interfaceAxis was -1 after the for loop in gridMarch!");
        }

        curr_t += min_t;
        rayOrigin += rayDirection * min_t;
        glm::ivec3 offset(0);
        // Sets it to 0 if sign is +, -1 if sign is -
        offset[interfaceAxis] = glm::min(0.f, glm::sign(rayDirection[interfaceAxis]));
        currCell = glm::ivec3(glm::floor(rayOrigin)) + offset;
        // If currCell contains something other than EMPTY, return curr_t
        BlockType cellType = terrain.getBlockAt(currCell.x, currCell.y, currCell.z);
        // blockColliding = cellType;
        if (cellType != EMPTY) {
            *out_blockHit = currCell;
            *out_dist = glm::min(maxLen, curr_t);
            return true;
        }
    }
    *out_dist = glm::min(maxLen, curr_t);
    return false;
}

// Based on lecture slides. Revised for placing blocks
bool Player::gridMarchPrevBlock(glm::vec3 rayOrigin, glm::vec3 rayDirection, const Terrain &terrain,
                                glm::ivec3 *out_prevBlock, glm::ivec3 *out_blockHit) {
    float maxLen = glm::length(rayDirection); // Farthest we search
    glm::ivec3 currCell = glm::ivec3(glm::floor(rayOrigin));
    rayDirection = glm::normalize(rayDirection); // Now all t values represent world dist.

    float curr_t = 0.f;
    while (curr_t < maxLen) {
        float min_t = glm::sqrt(3.f);
        float interfaceAxis = -1; // Track axis for which t is smallest
        for (int i = 0; i < 3; ++i) { // Iterate over the three axes
            if (rayDirection[i] != 0) { // Is ray parallel to axis i?
                float offset = glm::max(0.f, glm::sign(rayDirection[i]));
                // If the player is *exactly* on an interface then
                // they'll never move if they're looking in a negative direction
                if (currCell[i] == rayOrigin[i] && offset == 0.f) {
                    offset = -1.f;
                }
                int nextIntercept = currCell[i] + offset;
                float axis_t = (nextIntercept - rayOrigin[i]) / rayDirection[i];
                axis_t = glm::min(axis_t, maxLen); // Clamp to max len to avoid super out of bounds errors
                if (axis_t < min_t) {
                    min_t = axis_t;
                    interfaceAxis = i;
                }
            }
        }

        if (interfaceAxis == -1) {
            throw std::out_of_range("interfaceAxis was -1 after the for loop in gridMarch!");
        }

        curr_t += min_t;
        rayOrigin += rayDirection * min_t;
        glm::ivec3 offset(0);
        // Sets it to 0 if sign is +, -1 if sign is -
        offset[interfaceAxis] = glm::min(0.f, glm::sign(rayDirection[interfaceAxis]));

        glm::ivec3 prevOffset(0);
        prevOffset[interfaceAxis] = glm::sign(rayDirection[interfaceAxis]);

        currCell = glm::ivec3(glm::floor(rayOrigin)) + offset;

        // If currCell contains something other than EMPTY, return curr_t
        BlockType cellType = terrain.getBlockAt(currCell.x, currCell.y, currCell.z);

        // blockColliding = cellType;
        if (cellType != EMPTY) {
            *out_blockHit = currCell;
            *out_prevBlock = currCell - prevOffset;
            BlockType prevCellType = terrain.getBlockAt((*out_prevBlock).x,
                                                        (*out_prevBlock).y, (*out_prevBlock).z);
            return (prevCellType == EMPTY);
        }
    }
    return false;
}

void Player::jump(const Terrain &terrain, InputBundle &inputs) {
    if (isUnderLava(terrain, inputs) || isUnderWater(terrain, inputs)) {
        m_velocity[1] = 5.f;
        return;
    }

    if (flightMode || m_velocity[1] != 0.f) {
        return;
    }

    // TODO: test the factor
    m_velocity[1] = 5.f;
}

void Player::removeBlock(InputBundle &inputs, Terrain &terrain) {
    if (!inputs.leftButtonPressed || inputs.leftButtonPressProcessed) {
        // No leftButtonPressed event or the last leftButtonPressed event
        // might have been processed already.
        return;
    }

    glm::ivec3 outBlockHit(0);
    float outDistance = 0.f;
    glm::vec3 cameraRay(3.f * m_camera.getForward());
    bool cameraHit = gridMarch(
        m_camera.getCurrPos(),
        cameraRay,
        terrain,
        &outDistance,
        &outBlockHit
    );

    if (!cameraHit) {
        return;
    }


    BlockType removedBlockType = terrain.getBlockAt(glm::vec3(outBlockHit));

    // Remove hit block if the block is not BEDROCK (unbreakable)
    if (removedBlockType != BEDROCK) {
        terrain.setBlockAt(outBlockHit[0], outBlockHit[1], outBlockHit[2], EMPTY);
    }

    inputs.leftButtonPressProcessed = true;
}

void Player::placeBlock(InputBundle &inputs, Terrain &terrain) {
    if (!inputs.rightButtonPressed || inputs.rightButtonPressProcessed) {
        // No rightButtonPressed event or the last rightButtonPressed event
        // might have been processed already.
        return;
    }

    glm::ivec3 outBlockHit(0);
    glm::ivec3 outPrevBlockHit(0);
    glm::vec3 cameraRay(3.f * m_camera.getForward());

    bool newBlockHit = gridMarchPrevBlock(
        m_camera.getCurrPos(),
        cameraRay,
        terrain,
        &outPrevBlockHit,
        &outBlockHit
    );

    if (!newBlockHit) {
        return;
    }

    // TODO: we place STONE blocks, but we aren't sure what type is the right one.
    terrain.setBlockAt(outPrevBlockHit.x, outPrevBlockHit.y, outPrevBlockHit.z, BlockType::STONE);

    inputs.rightButtonPressProcessed = true;
}

bool Player::isUnderWater(const Terrain &terrain, InputBundle &input) {
    input.underWater = false;
    glm::vec3 topLeftVertex = this->m_position + glm::vec3(0.5f, 1.5f, 0.5f);
    for (int x = 0; x <= 1; x++) {
        for (int z = 0; z <= 1; z++) {
            if(terrain.hasChunkAt(x,z) && terrain.isChunkInitialized(x, z)){
                glm::vec3 p = glm::vec3(floor(topLeftVertex.x) + x, floor(topLeftVertex.y - 0.005f),
                                        floor(topLeftVertex.z) + z);
                if (terrain.getBlockAt(p) == WATER) {
                    input.underWater = true;
                }
            }
        }
    }
    return input.underWater;
}

bool Player::isUnderLava(const Terrain &terrain, InputBundle &input) {
    input.underLava = false;
    glm::vec3 topLeftVertex = this->m_position + glm::vec3(0.5f, 1.5f, 0.5f);
    for (int x = 0; x <= 1; x++) {
        for (int z = 0; z <= 1; z++) {
            if(terrain.hasChunkAt(x,z) && terrain.isChunkInitialized(x, z)){
                glm::vec3 p = glm::vec3(floor(topLeftVertex.x) + x, floor(topLeftVertex.y - 0.005f),
                                        floor(topLeftVertex.z) + z);
                if (terrain.getBlockAt(p) == LAVA) {
                    input.underLava = true;
                }
            }

        }
    }
    return input.underLava;
}

bool Player::isLiquid(const Terrain &terrain, glm::ivec3* pos) {
    BlockType blockType = terrain.getBlockAt((*pos).x, (*pos).y, (*pos).z);
    return Chunk::isLiquid(blockType);
}

bool Player::isWalking() {
    if (m_velocity.x != 0 || m_velocity.z != 0) {
        return true;
    }
    else {
        return false;
    }
}
