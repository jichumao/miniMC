#include "terrain.h"
#include <stdexcept>
#include <iostream>
#include <thread>
#include "terrain/blocktypeworker.h"
#include "terrain/vboworker.h"

#define MINECRAFT_BLOCK_TEXTURE_SLOT 0

Terrain::Terrain(OpenGLContext *context, int drawRadius, int expansionRadius)
    : m_chunks(),
      m_generatedTerrain(),
      m_blockTexture(context),
      mp_context(context),
      m_chunksWithVBOData(),
      m_drawRadius(drawRadius),
      m_expansionRadius(expansionRadius),
      m_minGeneratedZoneIndex(INT_MAX, INT_MAX),
      m_maxGeneratedZoneIndex(INT_MIN, INT_MIN)
{}

Terrain::~Terrain() {
    // TODO: destroy VBO data, like this:
    // m_geomCube.destroyVBOdata();
}

// Combine two 32-bit ints into one 64-bit int
// where the upper 32 bits are X and the lower 32 bits are Z
int64_t toKey(int x, int z) {
    int64_t xz = 0xffffffffffffffff;
    int64_t x64 = x;
    int64_t z64 = z;

    // Set all lower 32 bits to 1 so we can & with Z later
    xz = (xz & (x64 << 32)) | 0x00000000ffffffff;

    // Set all upper 32 bits to 1 so we can & with XZ
    z64 = z64 | 0xffffffff00000000;

    // Combine
    xz = xz & z64;
    return xz;
}

glm::ivec2 toCoords(int64_t k) {
    // Z is lower 32 bits
    int64_t z = k & 0x00000000ffffffff;
    // If the most significant bit of Z is 1, then it's a negative number
    // so we have to set all the upper 32 bits to 1.
    // Note the 8    V
    if(z & 0x0000000080000000) {
        z = z | 0xffffffff00000000;
    }
    int64_t x = (k >> 32);

    return glm::ivec2(x, z);
}

// Surround calls to this with try-catch if you don't know whether
// the coordinates at x, y, z have a corresponding Chunk
BlockType Terrain::getBlockAt(int x, int y, int z) const
{
    if(hasChunkAt(x, z)) {
        // Just disallow action below or above min/max height,
        // but don't crash the game over it.
        if(y < 0 || y >= 256) {
            return EMPTY;
        }
        const uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        return c->getBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                             static_cast<unsigned int>(y),
                             static_cast<unsigned int>(z - chunkOrigin.y));
    }
    else {
        throw std::out_of_range("Coordinates " + std::to_string(x) +
                                " " + std::to_string(y) + " " +
                                std::to_string(z) + " have no Chunk!");
    }
}

BlockType Terrain::getBlockAt(glm::vec3 p) const {
    return getBlockAt(p.x, p.y, p.z);
}

void Terrain::setBlockAt(int x, int y, int z, BlockType t) {
    if(hasChunkAt(x, z)) {
        uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        c->setBlockAt(
            static_cast<unsigned int>(x - chunkOrigin.x),
            static_cast<unsigned int>(y),
            static_cast<unsigned int>(z - chunkOrigin.y),
            t,
            true
        );
    }
    else {
        throw std::out_of_range("Coordinates " + std::to_string(x) +
                                " " + std::to_string(y) + " " +
                                std::to_string(z) + " have no Chunk!");
    }
}

void Terrain::initTexture() {
    m_blockTexture.create(":/textures/minecraft_textures_all.png");
    m_blockTexture.load(MINECRAFT_BLOCK_TEXTURE_SLOT);
}

bool Terrain::hasChunkAt(int x, int z) const {
    // Map x and z to their nearest Chunk corner
    // By flooring x and z, then multiplying by 16,
    // we clamp (x, z) to its nearest Chunk-space corner,
    // then scale back to a world-space location.
    // Note that floor() lets us handle negative numbers
    // correctly, as floor(-1 / 16.f) gives us -1, as
    // opposed to (int)(-1 / 16.f) giving us 0 (incorrect!).
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks.find(toKey(16 * xFloor, 16 * zFloor)) != m_chunks.end();
}

bool Terrain::isChunkInitialized(int x, int z) const {
    const uPtr<Chunk>& chunk = getChunkAt(x, z);
    if (chunk && chunk->m_initialized) 
        return true;
    else 
        return false;
}

uPtr<Chunk>& Terrain::getChunkAt(int x, int z) {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks[toKey(16 * xFloor, 16 * zFloor)];
}

const uPtr<Chunk>& Terrain::getChunkAt(int x, int z) const {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks.at(toKey(16 * xFloor, 16 * zFloor));
}

glm::ivec2 Terrain::getChunkIndexAt(int x, int z) const {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return glm::ivec2(16 * xFloor, 16 * zFloor);
}

// Links the chunk to existing neighbors around it.
void Terrain::linkChunk(Chunk *chunk) {
    glm::ivec2 xz = chunk->getXZ();
    int x = xz.s, z = xz.t;

    if(hasChunkAt(x, z + 16)) {
        auto &chunkNorth = m_chunks[toKey(x, z + 16)];
        chunk->linkNeighbor(chunkNorth, ZPOS);
    }
    if(hasChunkAt(x, z - 16)) {
        auto &chunkSouth = m_chunks[toKey(x, z - 16)];
        chunk->linkNeighbor(chunkSouth, ZNEG);
    }
    if(hasChunkAt(x + 16, z)) {
        auto &chunkEast = m_chunks[toKey(x + 16, z)];
        chunk->linkNeighbor(chunkEast, XPOS);
    }
    if(hasChunkAt(x - 16, z)) {
        auto &chunkWest = m_chunks[toKey(x - 16, z)];
        chunk->linkNeighbor(chunkWest, XNEG);
    }
}

void Terrain::initialize(glm::vec3 position) {
    initTexture();

    m_threadPool.submit([this]{
        VBOLeader m_vboLeader(
            &m_threadPool,
            &m_vboDataRecreationQueue,
            &m_newVBODataQueue
        );
        m_vboLeader.run();
    });

    expand(position);
}

// Instantiates a zone for chunk (x,z).
void Terrain::instantiateZoneAt(int x, int z) {
    auto zoneIndex = glm::ivec2(getZone1DIndex(x), getZone1DIndex(z));
    instantiateZoneAt(zoneIndex);
}

// Instantiates a zone for world-coordinate position.
void Terrain::instantiateZoneAt(glm::vec3 position) {
    glm::ivec2 chunkIndex = getChunkIndexAt(position.x, position.z);
    glm::ivec2 zoneIndex = getZoneIndexAt(chunkIndex.s, chunkIndex.t);
    instantiateZoneAt(zoneIndex);
}

// Instantiates a zone at zone index.
void Terrain::instantiateZoneAt(glm::ivec2 zoneIndex) {
    if (auto search = m_generatedTerrain.find(toKey(zoneIndex.s, zoneIndex.t)); search != m_generatedTerrain.end()) {
        // Zone has been generated already.
        return;
    }

    // Instantiate all the chunks of this zone concurrently. We call
    // receiveInstantiatedChunks() at the next tick to receive new chunks that are
    // ready, link them with others, and generate their VBO data.
    m_threadPool.submit([this, zoneIndex]{
        BlockTypeWorker w(zoneIndex, mp_context, &m_instantiatedChunksQueue);
    });

    // Consider it generated.
    m_generatedTerrain.insert(toKey(zoneIndex.s, zoneIndex.t));

    if (zoneIndex.s < m_minGeneratedZoneIndex.s) m_minGeneratedZoneIndex.s = zoneIndex.s;
    if (zoneIndex.t < m_minGeneratedZoneIndex.t) m_minGeneratedZoneIndex.t = zoneIndex.t;
    if (zoneIndex.s > m_maxGeneratedZoneIndex.s) m_maxGeneratedZoneIndex.s = zoneIndex.s;
    if (zoneIndex.t > m_maxGeneratedZoneIndex.t) m_maxGeneratedZoneIndex.t = zoneIndex.t;
}

// Picks up chunks created by concurrent workers, links them to neighbors,
// and submits VBO requests to concurrent workers to create their VBO data.
void Terrain::receiveInstantiatedChunks() {
    std::vector<Chunk *> newChunks;

    Chunk *chunk;
    while (m_instantiatedChunksQueue.tryPop(chunk)) {
        glm::ivec2 xz = chunk->getXZ();
        int x = xz.s, z = xz.t;
        m_chunks[toKey(x, z)] = std::unique_ptr<Chunk>(chunk);
        linkChunk(chunk);

        newChunks.push_back(chunk);
    }

    // Create VBO data for all the new chunks, concurrently. We call
    // receiveNewChunksWithVBOData at the next tick to upload that data (if ready)
    // to the GPU.
    for (Chunk *chunk : newChunks) {
        chunk->setVBODataRecreationQueue(&m_vboDataRecreationQueue);
        m_vboDataRecreationQueue.pushUnique(chunk);
    }
}

// Picks up chunks whose VBO data was created by concurrent workers and uploads
// that data to the GPU in VBOs.
void Terrain::receiveNewChunksWithVBOData() {
    ChunkVBOData data;
    while (m_newVBODataQueue.tryPop(data)) {
        data.chunk->uploadOpaqueVBOdata(data.opaqueIndices, data.opaqueVertexAttrs);
        data.chunk->uploadTransparentVBOdata(data.transparentIndices, data.transparentVertexAttrs);
        m_chunksWithVBOData.insert(data.chunk);
        data.chunk->m_initialized = true;
    }
}

// Instantiates m_expansionRadius x m_expansionRadius generation zones around
// the given position. Each generation zone is 4x4 = 16 chunks.
void Terrain::expand(const glm::vec3& position) {
    glm::ivec2 currentChunkIndex = getChunkIndexAt(position.x, position.z);

    int initialCoord = -64*(m_expansionRadius/2);
    for (int i = 0; i < m_expansionRadius; ++i) {
        for (int j = 0; j < m_expansionRadius; ++j) {
            instantiateZoneAt(
                currentChunkIndex.s + (initialCoord + 64*i),
                currentChunkIndex.t + (initialCoord + 64*j)
            );
        }
    }

    receiveInstantiatedChunks();
    receiveNewChunksWithVBOData();
}

float Terrain::getRadius() const {
    return glm::length(glm::vec2((m_maxGeneratedZoneIndex + glm::ivec2(64.f, 64.f)) - m_minGeneratedZoneIndex)) / 2;
}

void Terrain::draw(int minX, int maxX, int minZ, int maxZ, ShaderProgram *shaderProgram) {
    m_blockTexture.bind(MINECRAFT_BLOCK_TEXTURE_SLOT);
    shaderProgram->setBlockTextureSampler(MINECRAFT_BLOCK_TEXTURE_SLOT);

    std::vector<Chunk *> chunksInRangeWithVBOData;

    for(int x = minX; x <= maxX; x += 16) {
        for(int z = minZ; z <= maxZ; z += 16) {
            // Opaque draws.
            auto &chunk = getChunkAt(x, z);
            if (auto search = m_chunksWithVBOData.find(chunk.get()); search != m_chunksWithVBOData.end()) {
                shaderProgram->setModelMatrix(glm::translate(glm::mat4(), glm::vec3(x, 0.0f, z)));
                shaderProgram->drawInterleaved(*chunk, false);
                chunksInRangeWithVBOData.push_back(chunk.get());
            }
        }
    }

    // Transparent draws.
    for (Chunk *chunk : chunksInRangeWithVBOData) {
        glm::ivec2 xz = chunk->getXZ();
        int x = xz.s, z = xz.t;
        shaderProgram->setModelMatrix(glm::translate(glm::mat4(), glm::vec3(x, 0.0f, z)));
        shaderProgram->drawInterleaved(*chunk, true);
    }
}

// Draws the m_drawRadius x m_drawRadius zones around the given world-space position.
void Terrain::draw(glm::vec3 position, ShaderProgram *shaderProgram) {
    glm::ivec2 currentChunkIndex = getChunkIndexAt(position.x, position.z);
    glm::ivec2 currentZoneIndex = getZoneIndexAt(currentChunkIndex.s, currentChunkIndex.t);

    draw(
        getPrevZone1DIndex(currentZoneIndex.s, m_drawRadius),
        getNextZone1DIndex(currentZoneIndex.s, m_drawRadius),
        getPrevZone1DIndex(currentZoneIndex.t, m_drawRadius),
        getNextZone1DIndex(currentZoneIndex.t, m_drawRadius),
        shaderProgram
    );
}

void Terrain::bindTexture(int texSlot = 0){
    m_blockTexture.bind(texSlot);
}

enum drawDirection : unsigned char
{
    POSX, POSZ, NEGX, NEGZ
};

void Terrain::generateLSystem(glm::vec3 pos){

    // Generate L system and strings
    LSystem lsys = LSystem();
    glm::vec3 startPos = glm::vec3 (int(pos.x) , int(pos.y) - 10 , int(pos.z));
    std::string lSystemString = lsys.generateLSystem("X", 4);


    int currentIndex = 0;
    std::vector<drawDirection> drawDirections = {POSX, NEGZ, NEGX, POSZ};
    int lengthOfLine = 3;

    // Draw structure
    glm::vec3 turtlePos = startPos;

    for (int y0 = startPos[1] ; y0 > startPos[1] - 4; y0--) {
        for (int x0 = 0; x0 < 33; x0++) {
            for (int z0 = 0 ; z0 < 33; z0++) {
                setBlockAt(startPos[0] - 1 + x0, y0, startPos[2] - 1 + z0, CUCURBIT);
            }
        }
    }

    for (char c : lSystemString) {

        switch (c) {
        case 'X':
            break;
        case 'Y':
            break;
        case '+':
            // rotate 90 degree
            currentIndex = (currentIndex + 1) % 4;
            break;
        case '-':
            // rotate -90 degree
            if (currentIndex - 1 < 0) {
                currentIndex = 3;
            }else{
                currentIndex = (currentIndex - 1) % 4;
            }
            break;
        case 'F':
            //  place n blocks in this direction
            for (int i = 0; i < lengthOfLine; ++i) { //
            setBlockAt(turtlePos.x, turtlePos.y, turtlePos.z, EMPTY);
            setBlockAt(turtlePos.x, turtlePos.y - 1, turtlePos.z, EMPTY);
            setBlockAt(turtlePos.x, turtlePos.y - 2, turtlePos.z, EMPTY);
            // update turtlePos
            switch (drawDirections[currentIndex]) {
            case POSX:
                if(i!=lengthOfLine-1){
                    turtlePos.x += 1;
                }
                break;
            case POSZ:
                if(i!=lengthOfLine-1){
                    turtlePos.z += 1;
                }
                break;
            case NEGX:
                if(i!=lengthOfLine-1){
                    turtlePos.x -= 1;
                }
                break;
            case NEGZ:
                if(i!=lengthOfLine-1){
                    turtlePos.z -= 1;
                }
                break;
                }
            }
            break;
        default:
            break;
        }

    }

}
