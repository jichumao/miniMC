#pragma once

#include "smartpointerhelp.h"
#include "glm_includes.h"
#include <array>
#include <unordered_map>
#include <cstddef>
#include "drawable.h"
#include <mutex>
#include "terrain/queue.h"
#include "chunkhelper.h"
#include <unordered_set>

// C++ 11 allows us to define the size of an enum. This lets us use only one byte
// of memory to store our different block types. By default, the size of a C++ enum
// is that of an int (so, usually four bytes). This *does* limit us to only 256 different
// block types, but in the scope of this project we'll never get anywhere near that many.


// One Chunk is a 16 x 256 x 16 section of the world,
// containing all the Minecraft blocks in that area.
// We divide the world into Chunks in order to make
// recomputing its VBO data faster by not having to
// render all the world at once, while also not having
// to render the world block by block.

class Chunk : public Drawable {
private:
    // All of the blocks contained within this chunk.
    std::array<BlockType, 65536> m_blocks;

    // Origin coordinates of the chunk.
    int minX, minZ;

    // This chunk's four neighbors to the north, south, east, and west
    // The third input to this map just lets us use a Direction as
    // a key for this map.
    std::unordered_map<Direction, Chunk*, EnumHash> m_neighbors;
    mutable std::recursive_mutex m_neighborsMutex;

    // Highest non-EMPTY block of each column. One column per xz coordinate.
    std::array<unsigned int, 256> m_blockColumnTopY;

    std::atomic_bool m_recreateVBOData;

    // A thread-safe queue for the chunk to place itself for VBO data recreation
    // when something in it changes (e.g. type of a block or linked neighbor).
    TerrainWorkQueue<Chunk *> *m_vboDataRecreationQueue;

public:
    Chunk(OpenGLContext* context);
    Chunk(OpenGLContext* context, int x, int z);

    inline glm::ivec2 getXZ() const {
        return glm::ivec2(minX, minZ);
    }

    BlockType getBlockAt(unsigned int x, unsigned int y, unsigned int z) const;

    BlockType getBlockAt(int x, int y, int z) const;

    void setBlockAt(
        unsigned int x, unsigned int y, unsigned int z, BlockType t, bool recreateNeighbors = false
    );

    void linkNeighbor(uPtr<Chunk>& neighbor, Direction dir);

    // Uploads data to VBO on the GPU. Drawable interface.
    void createVBOdata() override;

    // Uploads the given index and vertex attribute data for opaque blocks
    // to a VBO on the GPU.
    void uploadOpaqueVBOdata(
        std::vector<GLuint> &indices,
        std::vector<glm::vec4> &vertexAttrs
    );

    // Uploads the given index and vertex attribute data for transparent blocks
    // to a VBO on the GPU.
    void uploadTransparentVBOdata(
        std::vector<GLuint> &indices,
        std::vector<glm::vec4> &vertexAttrs
    );

    // Creates index and vertex attribute data and places it in the given vectors.
    void createAndFillInData(
        std::vector<GLuint> *opaqueIndices,
        std::vector<glm::vec4> *opaqueVertexAttrs,
        std::vector<GLuint> *transparentIndices,
        std::vector<glm::vec4> *transparentVertexAttrs
    );

    inline void setVBODataRecreationQueue(TerrainWorkQueue<Chunk *> *vboDataRecreationQueue) {
        m_vboDataRecreationQueue = vboDataRecreationQueue;
    }

    std::array<bool, 6> getNoWaterBlockNeighbors(int x, int y, int z) const;
    bool hasNoWaterBlockNeighbor(int x, int y, int z, Direction direction) const;
    std::vector<glm::vec4> createNoWaterBlockBufPosNorColData(int x, int y, int z) const;
    std::vector<glm::vec4> createWaterBlockBufPosNorColData(int x, int y, int z) const;
    std::vector<glm::vec4> createWaterBufPosNorColData() const ;

    // A collection of liquid blocks that player can pass without collision
    static std::unordered_set<BlockType> liquidBlocks;

    // Determine if a given block is liquid
    static bool isLiquid(BlockType b);

    std::atomic_bool m_initialized;

private:
    // Neighbor management.

    std::array<bool, 6> getBlockNeighbors(int x, int y, int z) const;

    bool hasBlockNeighbor(int x, int y, int z, Direction direction) const;

    BlockType getNeighborChunkBlockAt(int x, int y, int z, Direction direction) const;


    // Visibility/occlusion.

    std::pair<unsigned int, unsigned int> getVisibleBlockYInterval(int x, int z) const;

    void updateBlockColumnTopY(unsigned int x, unsigned int z, unsigned int updatedBlockY);

    // Gets top Y coordinate of given block column. (x, z)
    // may be on a neighbor chunk.
    unsigned int getBlockColumnTopY(int x, int z) const;


    // VBO data creation.

    std::vector<GLuint> createBufIdxData(int faceCount) const;

    std::vector<glm::vec4> createVertexAttrBufData() const;

    std::vector<glm::vec4> createNoWaterVertexAttrBufData() const;

    std::vector<glm::vec4> createBlockVertexAttrBufData(int x, int y, int z) const;

    std::vector<glm::vec4> createFaceVertexAttrBufData(Direction direction, BlockType blockType, glm::vec4 offset) const;

    // Add chunk to a queue for VBO data recreation.
    void markForVBODataRecreation();

    // Add neighbor chunks of given block to the queue for VBO data recreation.
    void markNeighborsForVBODataRecreation(int blockX, int blockY, int blockZ);
};
