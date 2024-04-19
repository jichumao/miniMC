#pragma once
#include "scene/chunk.h"
#include <mutex>
#include "terrain/queue.h"

// Instantiates chunks. Meant to run concurrently in a thread.
class BlockTypeWorker {
public:
    // Construction also executes zone instantiation.
    BlockTypeWorker(
        glm::ivec2 zoneIndex,
        OpenGLContext *context,
        TerrainWorkQueue<Chunk *> *instantiatedChunksQueue
    );

private:
    // The Chunk constructor requires it.
    OpenGLContext *mp_context;

    // Thread-safe queue where this worker will place the chunks that it
    // instantiates, so that the main Terrain thread sees them.
    TerrainWorkQueue<Chunk *> *mp_instantiatedChunksQueue;

    // Creates all the chunks of the zone, sets block types according to
    // biome height functions, and places finished chunks in a queue shared
    // with the main Terrain thread.
    void instantiateChunksForZone(glm::ivec2 zoneIndex);

    // Sets block types according to biome height functions.
    void instantiateBiomesFor(Chunk *chunk);

    void setBlockAt(Chunk *chunk, int x, int y, int z, BlockType t);

    BlockType getBlockAt(Chunk *chunk, int x, int y, int z) const;

    void generateTree(Chunk *chunk, int x, int y, int z);

    void generateIcicle(Chunk *chunk, int x, int y, int z);

    void generateCacti(Chunk *chunk, int x, int y, int z);

    void generateTares(Chunk *chunk, int x, int y, int z);

    void generateDeadBush(Chunk *chunk, int x, int y, int z);

    void generateRedFlower(Chunk *chunk, int x, int y, int z);

    void generateMaze(Chunk *chunk, int x, int y, int z);

};
