#pragma once
#include "smartpointerhelp.h"
#include "glm_includes.h"
#include "chunk.h"
#include <array>
#include <unordered_map>
#include <unordered_set>
#include "shaderprogram.h"
#include <mutex>
#include "terrain/queue.h"
#include "terrain/threadpool.h"
#include "terrain/vboworker.h"
#include "terrain/vboleader.h"
#include "texture.h"
#include "terrain/lsystem.h""
// Helper functions to convert (x, z) to and from hash map key
int64_t toKey(int x, int z);
glm::ivec2 toCoords(int64_t k);

// The container class for all of the Chunks in the game.
// Ultimately, while Terrain will always store all Chunks,
// not all Chunks will be drawn at any given time as the world
// expands.
class Terrain {
private:
    // Stores every Chunk according to the location of its lower-left corner
    // in world space.
    // We combine the X and Z coordinates of the Chunk's corner into one 64-bit int
    // so that we can use them as a key for the map, as objects like std::pairs or
    // glm::ivec2s are not hashable by default, so they cannot be used as keys.
    std::unordered_map<int64_t, uPtr<Chunk>> m_chunks;

    // We will designate every 64 x 64 area of the world's x-z plane
    // as one "terrain generation zone". Every time the player moves
    // near a portion of the world that has not yet been generated
    // (i.e. its lower-left coordinates are not in this set), a new
    // 4 x 4 collection of Chunks is created to represent that area
    // of the world.
    // The world that exists when the base code is run consists of exactly
    // one 64 x 64 area with its lower-left corner at (0, 0).
    // When milestone 1 has been implemented, the Player can move around the
    // world to add more "terrain generation zone" IDs to this set.
    // While only the 3 x 3 collection of terrain generation zones
    // surrounding the Player should be rendered, the Chunks
    // in the Terrain will never be deleted until the program is terminated.
    std::unordered_set<int64_t> m_generatedTerrain;

    glm::ivec2 m_minGeneratedZoneIndex;
    glm::ivec2 m_maxGeneratedZoneIndex;

    Texture m_blockTexture;

    OpenGLContext* mp_context;

    // A general-purpose thread pool with a work queue. The main thread submits
    // functions to it and available threads from the pool grab them from the
    // work queue and execute them.
    TerrainThreadPool m_threadPool;

    // A thread-safe queue where worker threads from the pool place newly
    // instantiated chunks. The main thread grabs them from there.
    TerrainWorkQueue<Chunk *> m_instantiatedChunksQueue;

    // A thread-safe queue where worker threads from the pool place chunks
    // for which they have created VBO data.
    TerrainWorkQueue<ChunkVBOData> m_newVBODataQueue;

    // A thread-safe queue of chunks that need their VBO data recreated. A
    // VBOLeader thread will pick up chunks from here and spawn VBOWorker threads
    // to recreate data. 
    TerrainWorkQueue<Chunk *> m_vboDataRecreationQueue;

    // A set of chunks that have VBO data already.
    std::unordered_set<Chunk *> m_chunksWithVBOData;

    // Number of zones around the player to draw at once.
    int m_drawRadius;

    // Number of zones around the player to instantiate.
    int m_expansionRadius;

public:
    Terrain(OpenGLContext *context, int drawRadius, int expansionRadius);
    ~Terrain();


    // Chunk management.

    // Do these world-space coordinates lie within a Chunk that exists?
    bool hasChunkAt(int x, int z) const;

    // Assuming a Chunk exists at these coords, return a mutable reference to it.
    uPtr<Chunk>& getChunkAt(int x, int z);

    // Assuming a Chunk exists at these coords, return a const reference to it.
    const uPtr<Chunk>& getChunkAt(int x, int z) const;

    glm::ivec2 getChunkIndexAt(int x, int z) const;

    // Links the chunk to existing neighbors around it.
    void linkChunk(Chunk *chunk);

    bool isChunkInitialized(int x, int z) const;


    // Block management.

    // Given a world-space coordinate (which may have negative values) return the
    // block stored at that point in space.
    BlockType getBlockAt(int x, int y, int z) const;
    BlockType getBlockAt(glm::vec3 p) const;

    // Given a world-space coordinate (which may have negative values) set the
    // block at that point in space to the given type.
    void setBlockAt(int x, int y, int z, BlockType t);

    void initTexture();

    void bindTexture(int texSlot);


    // Zone management.

    void initialize(glm::vec3 position);

    // Instantiates a zone for chunk (x,z).
    void instantiateZoneAt(int x, int z);

    // Instantiates a zone for world-coordinate position.
    void instantiateZoneAt(glm::vec3 position);

    // Instantiates a zone at zone index.
    void instantiateZoneAt(glm::ivec2 zoneIndex);

    inline glm::ivec2 getZoneIndexAt(int x, int z) const {
        return glm::ivec2(getZone1DIndex(x), getZone1DIndex(z));
    }

    inline int getZone1DIndex(int chunkXOrZ) const {
        return chunkXOrZ & ~63;
    }

    inline int getPrevZone1DIndex(int zoneXOrZ, int multiplier = 0) const {
        return getZone1DIndex((zoneXOrZ - 64*multiplier) - 1);
    }

    inline int getNextZone1DIndex(int zoneXOrZ, int multiplier = 0) const {
        return getZone1DIndex((zoneXOrZ + 64*multiplier) + 64);
    }

    // Picks up chunks created by concurrent workers, links them to neighbors,
    // and submits VBO requests to concurrent workers to create their VBO data.
    void receiveInstantiatedChunks();

    // Picks up chunks whose VBO data was created by concurrent workers and uploads
    // that data to the GPU in VBOs.
    void receiveNewChunksWithVBOData();

    // Instantiates m_expansionRadius x m_expansionRadius generation zones around
    // the given position. Each generation zone is 4x4 = 16 chunks.
    void expand(const glm::vec3& playerPosition);

    float getRadius() const;

    // Drawing.

    // Draws every chunk that falls within the bounding box described by the
    // min and max coords.
    void draw(int minX, int maxX, int minZ, int maxZ, ShaderProgram *shaderProgram);

    // Draws the m_drawRadius x m_drawRadius zones around the given world-space position.
    void draw(glm::vec3 position, ShaderProgram *shaderProgram);

    // Draws all the chunks that have VBO data.
    void draw(ShaderProgram *shaderProgram);

    void generateLSystem(glm::vec3 pos);
};
