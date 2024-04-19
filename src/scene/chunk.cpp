#include "chunk.h"
#include <limits.h>
#include <iostream>

std::unordered_map<Direction, std::array<glm::vec4, 16>> faceToPosNor = {
    // Right face.
    { XPOS, {{
        // Position, normal, (dummy) color,(dummy) uv.
        glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
        glm::vec4(1, 0, 0, 0),
        glm::vec4(),
        glm::vec4(),

        // Position, normal, (dummy) color,(dummy) uv.
        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
        glm::vec4(1, 0, 0, 0),
        glm::vec4(),
        glm::vec4(),

        // Etc.
        glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
        glm::vec4(1, 0, 0, 0),
        glm::vec4(),
        glm::vec4(),

        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(1, 0, 0, 0),
        glm::vec4(),
        glm::vec4(),
    }} },
    // Left face.
    { XNEG, {{
        glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(-1, 0, 0, 0),
        glm::vec4(),
        glm::vec4(),

        glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
        glm::vec4(-1, 0, 0, 0),
        glm::vec4(),
        glm::vec4(),

        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
        glm::vec4(-1, 0, 0, 0),
        glm::vec4(),
        glm::vec4(),

        glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
        glm::vec4(-1, 0, 0, 0),
        glm::vec4(),
        glm::vec4(),
    }} },
    // Top face.
    { YPOS, {{
        glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(),
        glm::vec4(),

        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(),
        glm::vec4(),

        glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(),
        glm::vec4(),

        glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(),
        glm::vec4(),
    }} },
    // Bottom face.
    { YNEG, {{
        glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
        glm::vec4(0, -1, 0, 0),
        glm::vec4(),
        glm::vec4(),

        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
        glm::vec4(0, -1, 0, 0),
        glm::vec4(),
        glm::vec4(),

        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
        glm::vec4(0, -1, 0, 0),
        glm::vec4(),
        glm::vec4(),

        glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
        glm::vec4(0, -1, 0, 0),
        glm::vec4(),
        glm::vec4(),
    }} },
    // Front face.
    { ZPOS, {{
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(0, 0, 1, 0),
        glm::vec4(),
        glm::vec4(),

        glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
        glm::vec4(0, 0, 1, 0),
        glm::vec4(),
        glm::vec4(),

        glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
        glm::vec4(0, 0, 1, 0),
        glm::vec4(),
        glm::vec4(),

        glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(0, 0, 1, 0),
        glm::vec4(),
        glm::vec4(),
    }} },
    // Back face.
    { ZNEG, {{
        glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
        glm::vec4(0, 0, -1, 0),
        glm::vec4(),
        glm::vec4(),

        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
        glm::vec4(0, 0, -1, 0),
        glm::vec4(),
        glm::vec4(),

        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
        glm::vec4(0, 0, -1, 0),
        glm::vec4(),
        glm::vec4(),

        glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
        glm::vec4(0, 0, -1, 0),
        glm::vec4(),
        glm::vec4(),
    }} }
};

Chunk::Chunk(OpenGLContext* context) : Drawable(context), m_vboDataRecreationQueue(nullptr)
{}

Chunk::Chunk(OpenGLContext* context, int x, int z)
    : Drawable(context),
      m_blocks(),
      minX(x),
      minZ(z),
      m_neighbors{{XPOS, nullptr}, {XNEG, nullptr}, {ZPOS, nullptr}, {ZNEG, nullptr}},
      m_recreateVBOData(true),
      m_vboDataRecreationQueue(nullptr),
      m_initialized(false)
{
    std::fill_n(m_blocks.begin(), 65536, EMPTY);
    std::fill_n(m_blockColumnTopY.begin(), 256, 0);
}

// Does bounds checking with at()
BlockType Chunk::getBlockAt(unsigned int x, unsigned int y, unsigned int z) const {
    return m_blocks.at(x + 16 * y + 16 * 256 * z);
}

// Exists to get rid of compiler warnings about int -> unsigned int implicit conversion
BlockType Chunk::getBlockAt(int x, int y, int z) const {
    return getBlockAt(static_cast<unsigned int>(x), static_cast<unsigned int>(y), static_cast<unsigned int>(z));
}

// Does bounds checking with at()
void Chunk::setBlockAt(
    unsigned int x, unsigned int y, unsigned int z, BlockType t, bool recreateNeighbors
) {
    m_blocks.at(x + 16 * y + 16 * 256 * z) = t;
    updateBlockColumnTopY(x, z, y);
    markForVBODataRecreation();
    if (recreateNeighbors && t == BlockType::EMPTY) {
        markNeighborsForVBODataRecreation(x, y, z);
    }
}

const static std::unordered_map<Direction, Direction, EnumHash> oppositeDirection {
    {XPOS, XNEG},
    {XNEG, XPOS},
    {YPOS, YNEG},
    {YNEG, YPOS},
    {ZPOS, ZNEG},
    {ZNEG, ZPOS}
};

void Chunk::linkNeighbor(uPtr<Chunk> &neighbor, Direction dir) {
    if(neighbor != nullptr) {
        // TODO: might lead to deadlock.
        std::lock_guard lg(m_neighborsMutex);
        std::lock_guard lk(neighbor->m_neighborsMutex);

        this->m_neighbors[dir] = neighbor.get();
        neighbor->m_neighbors[oppositeDirection.at(dir)] = this;

        // VBO recreation is needed because the neighbor might occlude block faces.
        markForVBODataRecreation();
        neighbor->markForVBODataRecreation();
    }
}

std::array<bool, 6> Chunk::getBlockNeighbors(int x, int y, int z) const {
    std::array<bool, 6> hasNeighbors;

    for (int dir = 0; dir < 6; ++dir) {
        Direction direction = static_cast<Direction>(dir);
        hasNeighbors[dir] = hasBlockNeighbor(x, y, z, direction);
    }

    return hasNeighbors;
}

std::array<bool, 6> Chunk::getNoWaterBlockNeighbors(int x, int y, int z) const {
    std::array<bool, 6> hasNeighbors;

    for (int dir = 0; dir < 6; ++dir) {
        Direction direction = static_cast<Direction>(dir);
        hasNeighbors[dir] = hasNoWaterBlockNeighbor(x, y, z, direction);
    }

    return hasNeighbors;
}

float clamp(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

bool Chunk::hasBlockNeighbor(int x, int y, int z, Direction direction) const {
    int neighX = x, neighY = y, neighZ = z;

    switch (direction) {
    case XPOS:
        neighX++;
        break;
    case XNEG:
        neighX--;
        break;
    case YPOS:
        neighY++;
        break;
    case YNEG:
        neighY--;
        break;
    case ZPOS:
        neighZ++;
        break;
    case ZNEG:
        neighZ--;
        break;
    }

    BlockType neighType;

    neighY = clamp(neighY, 0, 255);

    if (neighX < 0 || neighX > 15 || neighZ < 0 || neighZ > 15) {
        neighType = getNeighborChunkBlockAt(neighX, y, neighZ, direction);
    } else {
        neighType = getBlockAt(neighX, neighY, neighZ);
    }

    return neighType != BlockType::EMPTY;
}

bool Chunk::hasNoWaterBlockNeighbor(int x, int y, int z, Direction direction) const {
    int neighX = x, neighY = y, neighZ = z;
    int flag = 0;

    switch (direction) {
    case XPOS:
        neighX++;
        break;
    case XNEG:
        neighX--;
        break;
    case YPOS:
        neighY++;
        break;
    case YNEG:
        neighY--;
        break;
    case ZPOS:
        neighZ++;
        break;
    case ZNEG:
        neighZ--;
        break;
    }

    BlockType neighType;

    neighY = clamp(neighY, 0, 255);

    if (neighX < 0 || neighX > 15 || neighZ < 0 || neighZ > 15) {
        neighType = getNeighborChunkBlockAt(neighX, y, neighZ, direction);

        if (neighType == BlockType::NONINIT){
            // If the adjacent block has not been initialized, we skip the rendering.
            return true;
        }

    } else {
        neighType = getBlockAt(neighX, neighY, neighZ);
    }

    return !(neighType == BlockType::EMPTY || neighType == BlockType::WATER);
}


BlockType Chunk::getNeighborChunkBlockAt(int x, int y, int z, Direction direction) const {
    std::lock_guard lg(m_neighborsMutex);
    if (auto search = m_neighbors.find(direction); search != m_neighbors.end()) {
        Chunk *neighChunk = search->second;
        if (neighChunk == nullptr) {
            return BlockType::NONINIT;
        }

        x = x < 0 ? 15 : x > 15 ? 0 : x;
        z = z < 0 ? 15 : z > 15 ? 0 : z;
        return neighChunk->getBlockAt(x, y, z);
    } else {
        return BlockType::EMPTY;
    }
}


std::pair<unsigned int, unsigned int> Chunk::getVisibleBlockYInterval(int x, int z) const {
    std::pair<unsigned int, unsigned int> yInterval;

    yInterval.second = m_blockColumnTopY.at(x + 16*z);
    yInterval.first = std::min({
        getBlockColumnTopY(static_cast<int>(x)-1, z),
        getBlockColumnTopY(x+1, z),
        getBlockColumnTopY(x, static_cast<int>(z)-1),
        getBlockColumnTopY(x, z+1),
        yInterval.second
    }) - 1;

    return yInterval;
}

// Gets top Y coordinate of given block column. If x < 0 or x > 15
// or z < 0 or z > 15, the corresponding neighbor chunk is searched.
//
// IMPORTANT: inputs x and z must be int's, not unsigned int's, to
// allow for negative values.
unsigned int Chunk::getBlockColumnTopY(int x, int z) const {
    // Not using % because it's tricky to use it with subtraction
    // of unsigned int's.

    bool onNeighbor = false;
    Direction neighDirection;

    unsigned int xPossiblyOnNeighbor = x;
    if (x < 0) {
        neighDirection = XNEG;
        xPossiblyOnNeighbor = 15;
        onNeighbor = true;
    } else if (x > 15) {
        neighDirection = XPOS;
        xPossiblyOnNeighbor = 0;
        onNeighbor = true;
    }

    unsigned int zPossiblyOnNeighbor = z;
    if (z < 0) {
        neighDirection = ZNEG;
        zPossiblyOnNeighbor = 15;
        onNeighbor = true;
    } else if (z > 15) {
        neighDirection = ZPOS;
        zPossiblyOnNeighbor = 0;
        onNeighbor = true;
    }

    if (onNeighbor) {
       std::lock_guard lg(m_neighborsMutex);
        if (auto search = m_neighbors.find(neighDirection); search != m_neighbors.end()) {
            Chunk *neighChunk = search->second;
            if (neighChunk == nullptr) {
                return UINT_MAX;
            }
            return neighChunk->m_blockColumnTopY.at(xPossiblyOnNeighbor + 16*zPossiblyOnNeighbor);
        }
    }

    return m_blockColumnTopY.at(x + 16*z);
}

void Chunk::updateBlockColumnTopY(unsigned int x, unsigned int z, unsigned int updatedBlockY) {
    BlockType updatedBlockType = getBlockAt(x, updatedBlockY, z);

    unsigned int topY = m_blockColumnTopY.at(x + 16*z);
    if (updatedBlockType != BlockType::EMPTY) {
        // The block should be visible.
        if (updatedBlockY > topY) {
            // This block is now the top visible block of its column.
            m_blockColumnTopY.at(x + 16*z) = updatedBlockY;
        }
    } else if (updatedBlockY == topY) {
        // The block was updated to EMPTY and it used to be the top visible
        // block of its column. Determine the new top visible block of the
        // column among the blocks below it.
        for (unsigned int y = topY-1; y >=0; --y) {
            if (getBlockAt(x, y, z) != BlockType::EMPTY) {
                m_blockColumnTopY.at(x + 16*z) = y;
                break;
            }
        }
    }
}

void Chunk::createVBOdata() {
    if (!m_recreateVBOData) {
        return;
    }

    std::vector<glm::vec4> posNorColData = createVertexAttrBufData();
    generatePos();
    bindPos();
    mp_context->glBufferData(
        GL_ARRAY_BUFFER,
        posNorColData.size() * sizeof(glm::vec4),
        posNorColData.data(),
        GL_STATIC_DRAW
    );

    const int dataCountPerFace = 16;
    int faceCount = posNorColData.size() / dataCountPerFace;
    std::vector<GLuint> indices = createBufIdxData(faceCount);
    generateIdx();
    bindIdx();
    mp_context->glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(GLuint),
        indices.data(),
        GL_STATIC_DRAW
    );

    m_count = indices.size();
}

// Uploads the given index and vertex attribute data for opaque blocks
// to a VBO on the GPU.
void Chunk::uploadOpaqueVBOdata(
    std::vector<GLuint> &indices,
    std::vector<glm::vec4> &vertexAttrs
) {
    generatePos();
    bindPos();
    mp_context->glBufferData(
        GL_ARRAY_BUFFER,
        vertexAttrs.size() * sizeof(glm::vec4),
        vertexAttrs.data(),
        GL_STATIC_DRAW
    );

    generateIdx();
    bindIdx();
    mp_context->glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(GLuint),
        indices.data(),
        GL_STATIC_DRAW
    );

    m_count = indices.size();
}

// Uploads the given index and vertex attribute data for transparent blocks
// to a VBO on the GPU.
void Chunk::uploadTransparentVBOdata(
    std::vector<GLuint> &indices,
    std::vector<glm::vec4> &vertexAttrs
) {
    generatePosTrans();
    bindPosTrans();
    mp_context->glBufferData(
        GL_ARRAY_BUFFER,
        vertexAttrs.size() * sizeof(glm::vec4),
        vertexAttrs.data(),
        GL_STATIC_DRAW
    );

    generateIdxTrans();
    bindIdxTrans();
    mp_context->glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(GLuint),
        indices.data(),
        GL_STATIC_DRAW
    );

    m_countTrans = indices.size();
}

std::vector<GLuint> Chunk::createBufIdxData(int faceCount) const {
    std::vector<GLuint> indices(faceCount * 6);

    for(int i = 0, idx = 0; i < faceCount; i++){
        indices[idx++] = i*4;
        indices[idx++] = i*4+1;
        indices[idx++] = i*4+2;
        indices[idx++] = i*4;
        indices[idx++] = i*4+2;
        indices[idx++] = i*4+3;
    }

    return indices;
}

std::vector<glm::vec4> Chunk::createVertexAttrBufData() const {
    std::vector<glm::vec4> posNorCol;

    for(unsigned int x = 0; x < 16; ++x) {
        for(unsigned int z = 0; z < 16; ++z) {
            for (unsigned int y = 0; y <= getBlockColumnTopY(x, z); ++y) {
                BlockType type = getBlockAt(x, y, z);
                int renderFlag = 0;

                // Only render non-empty block.
                if (type != BlockType::EMPTY && type == BlockType::WATER) {
                    for (int dir = 0; dir < 6; ++dir) {
                        Direction direction = static_cast<Direction>(dir);
                        // If there is a direction ajacent to EMPTY or WATER bock, we will render it.
                        if (hasNoWaterBlockNeighbor(x, y, z, direction) == 0) {
                            renderFlag = 1;
                        }
                    }
                    // Only render block adjacent to water and air.
                    if (renderFlag == 1){
                        std::vector<glm::vec4> blockPosNorCol = createNoWaterBlockBufPosNorColData(x, y, z);
                        posNorCol.insert(posNorCol.end(), blockPosNorCol.begin(), blockPosNorCol.end());
                    }
                }
            }
        }
    }

    return posNorCol;
}

std::vector<glm::vec4> Chunk::createWaterBufPosNorColData() const {
    std::vector<glm::vec4> posNorCol;

    for(unsigned int x = 0; x < 16; ++x) {
        for(unsigned int z = 0; z < 16; ++z) {
            for (unsigned int y = 0; y <= getBlockColumnTopY(x, z); ++y) {
                BlockType type = getBlockAt(x, y, z);
                int renderFlag = 0;

                // Only render WATER block.
                if (type == BlockType::WATER) {
                    for (int dir = 0; dir < 6; ++dir) {
                        Direction direction = static_cast<Direction>(dir);
                        // If there is a direction ajacent to EMPTY , we will render it.
                        if (hasBlockNeighbor(x, y, z, direction) == 0) {
                            renderFlag = 1;
                        }
                    }
                    // Only render block adjacent to water and air.
                    if (renderFlag == 1){
                        std::vector<glm::vec4> blockPosNorCol = createWaterBlockBufPosNorColData(x, y, z);
                        posNorCol.insert(posNorCol.end(), blockPosNorCol.begin(), blockPosNorCol.end());
                    }
                }
            }
        }
    }

    return posNorCol;
}

std::vector<glm::vec4> Chunk::createNoWaterVertexAttrBufData() const {
    std::vector<glm::vec4> posNorCol;

    for(unsigned int x = 0; x < 16; ++x) {
        for(unsigned int z = 0; z < 16; ++z) {
            for (unsigned int y = 0; y <= getBlockColumnTopY(x, z); ++y) {
                BlockType type = getBlockAt(x, y, z);
                int renderFlag = 0;

                // only render non-empty block
                if (type != BlockType::EMPTY && type != BlockType::WATER) {
                    for (int dir = 0; dir < 6; ++dir) {
                        Direction direction = static_cast<Direction>(dir);
                        // if there is a direction ajacent to EMPTY or WATER bock, we will render it
                        if (hasNoWaterBlockNeighbor(x, y, z, direction) == 0) {
                            renderFlag = 1;
                        }
                    }
                    // only render block adjacent to water and air
                    if (renderFlag == 1){
                        std::vector<glm::vec4> blockPosNorCol = createNoWaterBlockBufPosNorColData(x, y, z);
                        posNorCol.insert(posNorCol.end(), blockPosNorCol.begin(), blockPosNorCol.end());
                    }
                }
            }
        }
    }

    return posNorCol;
}

std::vector<glm::vec4> Chunk::createWaterBlockBufPosNorColData(int x, int y, int z) const {
    std::vector<glm::vec4> blockPosNorCol;

    std::array<bool, 6> blockNeighbors = getBlockNeighbors(x, y, z);
    for (int dir = Direction::XPOS; dir <= ZNEG; ++dir) {
        Direction direction = static_cast<Direction>(dir);

        bool hasBlockNeighbor = blockNeighbors[dir];

        // If there is a face direction adjacent to EMPTY or WATER, then render it.
        if (!hasBlockNeighbor) {
            std::vector<glm::vec4> facePosNorCol = createFaceVertexAttrBufData(direction, getBlockAt(x, y, z), glm::vec4(x, y, z, 0.0f));
            blockPosNorCol.insert(blockPosNorCol.end(), facePosNorCol.begin(), facePosNorCol.end());
        }
    }

    return blockPosNorCol;
}

std::vector<glm::vec4> Chunk::createNoWaterBlockBufPosNorColData(int x, int y, int z) const {
    std::vector<glm::vec4> blockPosNorCol;

    std::array<bool, 6> blockNeighbors = getNoWaterBlockNeighbors(x, y, z);
    for (int dir = Direction::XPOS; dir <= ZNEG; ++dir) {
        Direction direction = static_cast<Direction>(dir);

        bool hasNoWaterBlockNeighbor = blockNeighbors[dir];

        // If there is a face direction adjacent to EMPTY or WATER, then render it.
        if (!hasNoWaterBlockNeighbor) {
            std::vector<glm::vec4> facePosNorCol = createFaceVertexAttrBufData(direction, getBlockAt(x, y, z), glm::vec4(x, y, z, 0.0f));
            blockPosNorCol.insert(blockPosNorCol.end(), facePosNorCol.begin(), facePosNorCol.end());
        }
    }

    return blockPosNorCol;
}

std::vector<glm::vec4> Chunk::createBlockVertexAttrBufData(int x, int y, int z) const {
    std::vector<glm::vec4> blockPosNorCol;

    std::array<bool, 6> blockNeighbors = getBlockNeighbors(x, y, z);
    for (int dir = Direction::XPOS; dir <= ZNEG; ++dir) {
        Direction direction = static_cast<Direction>(dir);

        bool hasBlockNeighbor = blockNeighbors[dir];

        if (!hasBlockNeighbor) {
            std::vector<glm::vec4> facePosNorCol = createFaceVertexAttrBufData(direction, getBlockAt(x, y, z), glm::vec4(x, y, z, 0.0f));
            blockPosNorCol.insert(blockPosNorCol.end(), facePosNorCol.begin(), facePosNorCol.end());
        }
    }

    return blockPosNorCol;
}

std::vector<glm::vec4> Chunk::createFaceVertexAttrBufData(Direction direction, BlockType blockType, glm::vec4 offset) const {
    std::unordered_map<BlockType, glm::vec4> blockTypeToColor = {
        { EMPTY, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) },
        { GRASS, glm::vec4(95.f, 159.f, 53.f, 255.f) / 255.f },
        { DIRT, glm::vec4(121.f, 85.f, 58.f, 255.f) / 255.f },
        { STONE, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f) },
        { WATER, glm::vec4(0.f, 0.f, 0.75f, 1.0f) },
        { SNOW, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) },
        { LAVA, glm::vec4(1.0f, 0.2f, 0.2f, 1.0f) },
        { BEDROCK, glm::vec4(0.25f, 0.25f, 0.25f, 1.0f)}
    };

    std::vector<glm::vec4> facePosNorCol;

    std::array<glm::vec4, 16> &facePosNor = faceToPosNor[direction];
    facePosNorCol.insert(facePosNorCol.end(), facePosNor.begin(), facePosNor.end());

    glm::vec4 color = blockTypeToColor[blockType];

    glm::vec2 uv = blockFaceUVs.at(blockType).at(direction);
     // Transform to vec4 for easier data transfer.
    glm::vec4 uv4 = glm::vec4(uv, 0.0f, 0.0f);

    // i = 2 is the color for the fisrt point, we have 4 groups in entire iteration
    // offset is the (x,y) respect to the world
    for (int i = 2; i < 16; i += 4) {
        facePosNorCol[i-2] = facePosNorCol[i-2] + offset;
        facePosNorCol[i] = color;

        // UVs.
        switch (i) {
        case 2: // Left up.
            facePosNorCol[i + 1] = glm::vec4(uv + glm::vec2(0.0, 0.0625), 0.0f, 0.0f);
            break;
        case 6:  // Left down.
            facePosNorCol[i + 1] = glm::vec4(uv, 0.0f, 0.0f);
            break;
        case 10:  // Right down.
            facePosNorCol[i + 1] = glm::vec4(uv + glm::vec2(0.0625, 0.0), 0.0f, 0.0f);
            break;
        case 14: // Right up.
            facePosNorCol[i + 1] = glm::vec4(uv + glm::vec2(0.0625, 0.0625), 0.0f, 0.0f);
            break;

        }

        if (blockType ==  LAVA || blockType ==  WATER ){
            // Signal for animation.
            facePosNorCol[i + 1][2] = 1;
        }

        if (blockType !=  WATER ){
            // Signal for WATER.
            facePosNorCol[i + 1][3] = 1;
        }

    }

    return facePosNorCol;
}

// Creates index and vertex attribute data and places it in the given vectors.
void Chunk::createAndFillInData(
    std::vector<GLuint> *opaqueIndices,
    std::vector<glm::vec4> *opaqueVertexAttrs,
    std::vector<GLuint> *transparentIndices,
    std::vector<glm::vec4> *transparentVertexAttrs
) {
    *opaqueVertexAttrs = createNoWaterVertexAttrBufData();

    const int dataCountPerFace = 12;
    const int opaqueFaceCount = opaqueVertexAttrs->size() / dataCountPerFace;
    *opaqueIndices = createBufIdxData(opaqueFaceCount );

    *transparentVertexAttrs = createWaterBufPosNorColData();

    int transparentCount = transparentVertexAttrs->size() / dataCountPerFace;
    *transparentIndices = createBufIdxData(transparentCount);
}

// Adds itself to a queue for VBO data recreation.
void Chunk::markForVBODataRecreation() {
    if (m_vboDataRecreationQueue) {
        m_vboDataRecreationQueue->pushUnique(this);
    }

    // For old createVBOdata function.
    m_recreateVBOData = true;
}

// Adds neighbor chunks of given block to the queue for VBO data recreation.
void Chunk::markNeighborsForVBODataRecreation(
    int blockX, int blockY, int blockZ
) {
    Direction neighXDirection;
    if (blockX == 0 && hasBlockNeighbor(blockX, blockY, blockZ, XNEG)) {
        neighXDirection = XNEG;
    } else if (blockX == 15 && hasBlockNeighbor(blockX, blockY, blockZ, XPOS)) {
        neighXDirection = XPOS;
    }

    Direction neighZDirection;
    if (blockZ == 0 && hasBlockNeighbor(blockX, blockY, blockZ, ZNEG)) {
        neighZDirection = ZNEG;
    } else if (blockZ == 15 && hasBlockNeighbor(blockX, blockY, blockZ, ZPOS)) {
        neighZDirection = ZPOS;
    }

    std::lock_guard lg(m_neighborsMutex);
    if (auto search = m_neighbors.find(neighXDirection); search != m_neighbors.end()) {
        Chunk *neighChunk = search->second;
        if (neighChunk != nullptr) {
            neighChunk->markForVBODataRecreation();
        }
    }
    if (auto search = m_neighbors.find(neighZDirection); search != m_neighbors.end()) {
        Chunk *neighChunk = search->second;
        if (neighChunk != nullptr) {
            neighChunk->markForVBODataRecreation();
        }
    }
}

bool Chunk::isLiquid(BlockType b) {
    return (liquidBlocks.find(b) != liquidBlocks.end());
}

std::unordered_set<BlockType> Chunk::liquidBlocks = {
    WATER, LAVA
};
